#include <X-GSD/Scene.hpp>

/*
 ResourcePath.hpp is not provided with X-GSD because its
 implementation is OS-dependent.
 You can easily create your own as an envelop of the OS specific
 function to reach the resource folder.
 You can also use the implementation provided with SFML templates
 for specific platforms-OSs (for example, they provide one for
 Xcode in their downloads section of SFML webpage).
 */
#include "ResourcePath.hpp"
#include "ControllersRegistration.hpp" // Include here to avoid reference cycles

#include <json/json.h>

#include <cassert>
#include <stdexcept>
#include <fstream>

using namespace xgsd;

Scene::Scene(sf::RenderWindow& window)
: mWindow(window)
, mSceneView(mWindow.getView())
, mName("")
, mSceneGraph(new SceneGraphNode)
, mPhysicsEngine(Game::instance().getPhysicsEngine())
, mPaused(false)
, mTransitionState(none)
, mTransitionTime(HiResDuration(0))
, mTransitionEnabled(true)
, mTransitionDuration(ONE_SECOND)
, mSceneChangeRequest(false)
{
    // Load resources here (RAII)
    
    mTransitionFadingRectangle.setSize(sf::Vector2f(mWindow.getSize().x, mWindow.getSize().y));
}

// TODO: Prefab loading
void Scene::readJsonSceneFile(std::string jsonPath)
{
    Json::Value root;   // will contains the root value after parsing
    Json::Reader reader;
    
    DBGMSGC("Scene JSON file to parse: " << resourcePath() << jsonPath);
    
    std::ifstream jsonFile(resourcePath() + jsonPath, std::ifstream::binary);
    
    bool parsingSuccessful = reader.parse(jsonFile, root);
    if ( !parsingSuccessful )
    {
        // report to the user the failure and their locations in the document.
        throw std::runtime_error("Failed to parse file\n" + reader.getFormattedErrorMessages());
    }
    
    
    ///////////////////////////////////////////////
    //  1.-  Fill scene info and load resources  //
    ///////////////////////////////////////////////
    
    // Get the value of the member of root named 'name', return a "" string if there is no such member
    std::string sceneName = root.get("name", "").asString();
    DBGMSGC("Scene name: " << sceneName);
    if (sceneName == "")
        throw std::runtime_error("SceneManager::loadSceneFromFile - Failed to load " + resourcePath() + jsonPath + "  - No 'name' found");
    
    // Fill the scene's member variables
    mName = sceneName;
    
    // Get the value of the member of root named 'resources', return a 'null' value if
    // there is no such member.
    const Json::Value resources = root["resources"];
    if (!resources)
        DBGMSGC("No resources to load on this scene");
    else {
        
        const Json::Value textures = resources["textures"];
        
        // Iterate through textures
        for (auto texture : textures) {
            
            std::string name = texture.get("name", "").asString();
            std::string path = texture.get("path", "").asString();
            
            if (name == "")
                throw std::runtime_error("SceneManager::loadSceneFromFile - Failed to load " + resourcePath() + jsonPath + "  - No 'name' (textures) found");
            if (path == "")
                throw std::runtime_error("SceneManager::loadSceneFromFile - Failed to load " + resourcePath() + jsonPath + "  - No 'path' (textures) found");
            
            // Load texture into the scene's texture manager
            mTextureManager->load(name, path);
        }
        
        const Json::Value fonts = resources["fonts"];
        
        // Iterate through fonts
        for (auto iter = fonts.begin(); iter != fonts.end(); iter++) {
            
            std::string name = iter->get("name", "").asString();
            std::string path = iter->get("path", "").asString();
            if (name == "")
                throw std::runtime_error("SceneManager::loadSceneFromFile - Failed to load " + resourcePath() + jsonPath + "  - No 'name' (fonts) found");
            if (path == "")
                throw std::runtime_error("SceneManager::loadSceneFromFile - Failed to load " + resourcePath() + jsonPath + "  - No 'path' (fonts) found");
            
            // Load font into the scene's font manager
            mFontManager->load(name, path);
        }
        
        
        const Json::Value sounds = resources["sounds"];
        
        // Iterate through sounds
        for (auto sound : sounds) {
            
            std::string name = sound.get("name", "").asString();
            std::string path = sound.get("path", "").asString();
            if (name == "")
                throw std::runtime_error("SceneManager::loadSceneFromFile - Failed to load " + resourcePath() + jsonPath + "  - No 'name' (sounds) found");
            if (path == "")
                throw std::runtime_error("SceneManager::loadSceneFromFile - Failed to load " + resourcePath() + jsonPath + "  - No 'path' (sounds) found");
            
            // Load font into the scene's font manager
            mSoundManager->load(name, path);
        }
    }
    
    ////////////////////////////////////////////
    //  2.-  Create entities and components   //
    ////////////////////////////////////////////
    
    // Collection to store entities whose parent entity was not available at its time of creation. They will try to attach to its parents again after all entities load.
    std::vector<std::pair<Entity::Ptr, std::string>> delayedAttachmentEntities;
    
    // Temporary map to store entity-parent names to check circle parent reference between entities (a.parent = b, b.parent = a)
    std::unordered_map<std::string, std::string> entitiyParentRelations;
    
    // Register user-defined controllers
    ControllersRegistration controllersRegistration;
    controllersRegistration.registerControllers(mControllersManager);
    
    
    // Get the value of the member of root named 'entities', return a 'null' value if there is no such member.
    const Json::Value entities = root["entities"];
    if (!entities)
        throw std::runtime_error("SceneManager::loadSceneFromFile - Failed to load " + resourcePath() + jsonPath + "  - No 'entities' found");
    
    // Iterate through entities
    for (auto entity : entities) {
        
        std::string name = entity.get("name", "").asString();
        if (name == "")
            throw std::runtime_error("SceneManager::loadSceneFromFile - Failed to load " + resourcePath() + jsonPath + "  - No 'name' (entity) found");
        
        // Create the entity
        Entity::Ptr newEntity(new Entity(name));
        
        // Set the Transformable
        sf::Transformable newTransformable;
        
        const Json::Value transform = entity["transform"];
        if (!transform.isNull()) {
            
            // If there is a "transform" in the JSON file, read its contents and use it. Else, keep the default transformable zeroed values
            const Json::Value origin = transform["origin"];
            const Json::Value position = transform["position"];
            const Json::Value scale = transform["scale"];
            const Json::Value rotation = transform["rotation"];
            
            
            // Set the origin (from the JSON if any, or a default value)
            if (!origin)
                newTransformable.setOrigin(0, 0);
            else
                newTransformable.setOrigin(origin["x"].asFloat(), origin["y"].asFloat()); // TODO: Add relative to bounding box
            
            // Set the position (from the JSON if any, or a default value)
            if (!position)
                newTransformable.setPosition(0, 0);
            else if (position["relative"].asBool())
                newTransformable.setPosition(position["x"].asFloat() * mWindow.getView().getSize().x, position["y"].asFloat() * mWindow.getView().getSize().y); // TODO: Relative to the parent node instead of view
            else
                newTransformable.setPosition(position["x"].asFloat(), position["y"].asFloat());
            
            // Set the scale (from the JSON if any, or a default value)
            if (!scale)
                newTransformable.setScale(1, 1);
            else
                newTransformable.setScale(scale["x"].asFloat(), scale["y"].asFloat());
            
            // Set the rotation (from the JSON if any, or a default value)
            if (!rotation)
                newTransformable.setRotation(0);
            else
                newTransformable.setRotation(rotation.asFloat());
        }
        
        newEntity->mTransformable = newTransformable;
        
        // Get the "components" JSON element of this entity
        const Json::Value components = entity["components"];
        
        if (!components.isNull()) {
            
            // Look for a ComponentSprite. If it existis, create and add it to the entity.
            const Json::Value sprite = components["ComponentSprite"];
            
            if (!sprite.isNull()) {
                
                bool globalTexture = sprite["globalTexture"].asBool();
                std::string textureName = sprite.get("texture", "").asString();
                
                if (textureName == "")
                    throw std::runtime_error("SceneManager::loadSceneFromFile - Failed to load " + resourcePath() + jsonPath + "  - No 'texture' found in 'ComponentSprite'");
                
                const Json::Value rect = sprite["textureRect"];
                if (!rect) {
                    Component::Ptr componentSprite(new ComponentSprite(globalTexture ? Game::instance().getGlobalTextureManager().get(textureName) : mTextureManager->get(textureName)));
                    newEntity->addComponent(std::move(componentSprite));
                }
                else {
                    sf::IntRect textureRect;
                    
                    textureRect.top = rect["top"].asInt();
                    textureRect.left = rect["left"].asInt();
                    textureRect.height = rect["height"].asInt();
                    textureRect.width = rect["width"].asInt();
                    
                    Component::Ptr componentSprite(new ComponentSprite(globalTexture ? Game::instance().getGlobalTextureManager().get(textureName) : mTextureManager->get(textureName), textureRect));
                    newEntity->addComponent(std::move(componentSprite));
                }
            }
            
            // Look for a ComponentCollider. If it existis, create and add it to the entity.
            const Json::Value collider = components["ComponentCollider"];
            if (!collider.isNull()) {
                
                const Json::Value rect = collider["boundsRect"];
                sf::FloatRect boundsRect;
                
                if (!rect) {
                    boundsRect = sf::FloatRect(0, 0, 0, 0);
                }
                else {
                    boundsRect.top = rect["top"].asFloat();
                    boundsRect.left = rect["left"].asFloat();
                    boundsRect.height = rect["height"].asFloat();
                    boundsRect.width = rect["width"].asFloat();
                }
                
                Component::Ptr componentCollider(new ComponentCollider(boundsRect));
                newEntity->addComponent(std::move(componentCollider));
            }
            
            // Look for a ComponentRigidBody. If it existis, create and add it to the entity.
            const Json::Value rigidBody = components["ComponentRigidBody"];
            if (!rigidBody.isNull()) {
                
                bool kinematic = rigidBody["kinematic"].asBool();
                Component::Ptr componentRigidBody(new ComponentRigidBody(kinematic));
                newEntity->addComponent(std::move(componentRigidBody));
            }
            
            // Look for a list of user-defined controllers
            const Json::Value controllers = components["controllers"];
            
            // Iterate through controllers
            for (auto controller : controllers) {
                
                // TODO: Define a way of loading controller-specific values, i.e: number of lives, speed or jump height. It may be implemented with a map from string to... any? or to string and parse to int/float if needed?
                std::string type = controller["type"].asString();
                Component:: Ptr newController(mControllersManager.getController(type));
                newEntity->addComponent(std::move(newController));
            }
            
        } // End of components
        
        
        // And finally attach the entity to the scene's root node or the specified parent entity
        std::string parentEntityName = entity.get("parentEntityName", "root").asString();
        
        if (parentEntityName == "root")
            mSceneGraph->requestAttach(std::move(newEntity));
        else {
            // Look for the parent entity
            auto parentEntity = Entity::getEntityNamed(parentEntityName);
            
            if (!parentEntity) {
                // If it hasn't been created yet, store this entity in a temporal collection
                delayedAttachmentEntities.push_back(std::make_pair(std::move(newEntity), parentEntityName));
            }
            else
                parentEntity->requestAttach(std::move(newEntity));
        }
        
        // Store parent name for circular parent reference checking
        if (parentEntityName != "root")
            entitiyParentRelations[name] = parentEntityName;
        
    } // End of entities loop
    
    
    /////////////////////////////////////////////////
    //  3.-  Perform final operations and checks   //
    /////////////////////////////////////////////////
    
    
    // Try to attach again the stored entities whose parent entities were not available
    for (auto iter = delayedAttachmentEntities.begin(); iter != delayedAttachmentEntities.end(); ++iter) {
        // Look for the parent entity
        auto parentEntity = Entity::getEntityNamed(iter->second);
        
        if (!parentEntity)
            throw std::runtime_error("The specified parent entity [" + iter->second + "] does not exist");
        parentEntity->requestAttach(std::move(iter->first));
    }
    
    
    // Check circle parent reference between entities (a.parent = b, b.parent = a)
    for (auto iter = entitiyParentRelations.begin(); iter != entitiyParentRelations.end(); ++iter)
    {
        std::string entityName = iter->first;
        std::string parentName = iter->second;
        
        if (entitiyParentRelations[parentName] == entityName)
            throw std::runtime_error("SceneManager::loadSceneFromFile - Failed to load " + resourcePath() + jsonPath + "  - Circular parent reference: [" + entityName + "] parent is [" + parentName + "], and [" + parentName + "] parent is [" + entityName + "].");
    }
    
    // Ensure scene graph operations (attachments) get done
    mSceneGraph->performPendingSceneGraphOperations();
}


void Scene::addNode(SceneGraphNode::Ptr node)
{
    mSceneGraph->requestAttach(std::move(node));
}

void Scene::loadSceneFromFile(std::string jsonPath)
{
    mSceneChangeRequest = true;
    mNextScenePath = jsonPath;
    
    if (!mTransitionEnabled)
        performSceneChange();
    else
        mTransitionState = out;
}

void Scene::performSceneChange()
{
    // Unload current scene's resources, nodes, values, etc.
    unloadScene();
    
    // Load the new scene's resources, entities, etc.
    readJsonSceneFile(mNextScenePath);
    
    mNextScenePath = "";
}

void Scene::unloadScene()
{
    mName = "";
    
    // Reset the scene graph
    mSceneGraph.reset(new SceneGraphNode);
    
    // Reset resources managers
    mFontManager.reset(new FontManager);
    mTextureManager.reset(new TextureManager);
    mSoundManager.reset(new SoundManager);
    
}

void Scene::update(const HiResDuration &dt)
{
    if (mPaused)
        mSceneGraph->onPause(dt);
    
    // If a scene change is requested, don't update (for safety)
    else if (!mSceneChangeRequest) {
        mPhysicsEngine.checkCollisions();
        mSceneGraph->update(dt);
        mSceneGraph->performPendingSceneGraphOperations();
    }
    
    // Perform the scene change transition when requested
    else if (mTransitionEnabled)
        updateTransition(dt);
}

void Scene::render()
{
    // If a scene change is requested, don't render (for safety)
    if (!mSceneChangeRequest)
        mWindow.draw(*mSceneGraph);
    else if (mTransitionEnabled)
        renderTransition();
}

void Scene::handleEvent(const Event &event)
{
    mSceneGraph->handleEvent(event);
}

// Simple transition update method (linear fade-in/fade-out to/from black color)
void Scene::updateTransition(const HiResDuration &dt)
{
    // Update transition according to its state
    switch (mTransitionState) {
        case none:
            break;
            
        case in:
            // Decrease opacity as time moves forward
            mTransitionFadingRectangle.setFillColor(sf::Color(0, 0, 0, 255 - (float)mTransitionTime.count() / (float)mTransitionDuration.count() * 255.f));
            
            mTransitionTime += dt;
            
            // If the transition time has passed, change state to finished.
            if (mTransitionTime >= mTransitionDuration) {
                mTransitionState = inFinished;
                mTransitionTime = HiResDuration(0);
            }
            break;
            
            
        case out:
            // Increase opacity as time moves forward
            mTransitionFadingRectangle.setFillColor(sf::Color(0, 0, 0, (float)mTransitionTime.count() / (float)mTransitionDuration.count() * 255.f));
            
            mTransitionTime += dt;
            
            // If the transition time has passed, change state to finished.
            if (mTransitionTime >= mTransitionDuration) {
                mTransitionState = outFinished;
                mTransitionTime = HiResDuration(0);
            }
            break;
            
        case inFinished:
            mTransitionState = none;
            mSceneChangeRequest = false;
            break;
            
        case outFinished:
            mTransitionFadingRectangle.setFillColor(sf::Color::Black);
            
            // Load the next scene
            performSceneChange();
            
            mTransitionState = in;
            break;
    }
}

void Scene::renderTransition()
{
    // Draw the mFadingRectangle according to transition state
    switch (mTransitionState) {
        case none:
        case inFinished:
        case in:
        case out:
            mWindow.draw(*mSceneGraph);
        case outFinished:
            mWindow.draw(mTransitionFadingRectangle);
            break;
    }
}

std::string Scene::getName()
{
    return mName;
}

bool Scene::isTransitionEnabled()
{
    return mTransitionEnabled;
}

void Scene::setTransitionEnabled(bool option)
{
    mTransitionEnabled = option;
}

void Scene::pauseGame()
{
    mPaused = true;
}

void Scene::resumeGame()
{
    mPaused = false;
}

Scene::~Scene()
{
    // Cleanup
}
