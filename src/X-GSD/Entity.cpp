#include <X-GSD/Entity.hpp>

using namespace xgsd;

// Static initialization
std::unordered_map<std::string, Entity*> Entity::entities;

Entity* Entity::getEntityNamed(std::string name)
{
    auto found = entities.find(name);
    if (found != entities.end())
        return found->second;
    else
        return nullptr;
}

Entity::Entity(std::string name)
: mName(name)
{
    // Load resources here (RAII)
    
    // Insert a reference of the new Entity in a collection of pairs name-entityPointer
    auto inserted = entities.insert(std::make_pair(name, this));
    
    if (inserted.second == false)
        throw std::runtime_error("Insertion of entity with name [" + name + "] failed.");
}


void Entity::addComponent(Component::Ptr component)
{
    // Add this entity's reference to the component's pointer
    component->entity = this;
    
    // Notify the component that it has been added to an entity
    component->attachedToEntity();
    
    // Store the component according to its type
    mComponents[std::type_index(typeid(*component))] = std::move(component);
}

void Entity::onAttachThis()
{
    // Perform onEntityAttach call of components/controllers
    for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
    {
        iter->second->onEntityAttach();
    }
}

void Entity::onDetachThis()
{
    // Perform onEntityDetach call of components/controllers
    for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
    {
        iter->second->onEntityDetach();
    }
}

void Entity::updateThis(const HiResDuration& dt)
{
    // Perform update call of components/controllers
    for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
    {
        iter->second->update(dt);
    }
}


void Entity::onPauseThis(const HiResDuration& dt)
{
    // Perform update call of components/controllers
    for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
    {
        iter->second->onPause(dt);
    }
}


void Entity::drawThis(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Perform draw call of components/controllers
    for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
    {
        iter->second->draw(target, states);
    }
}


void Entity::handleEventThis(const Event& event)
{
    // Perform handleEvent call of components/controllers
    for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
    {
        iter->second->handleEvent(event);
    }
}

void Entity::collisionHandler(Entity *theOtherEntity, sf::FloatRect collision)
{
    // Perform collisionHandler call of components/controllers
    for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
    {
        iter->second->collisionHandler(theOtherEntity, collision);
    }
}

std::string Entity::getName()
{
    return mName;
}

Entity::~Entity()
{
    // Cleanup
    
    Entity::entities.erase(mName);
}
