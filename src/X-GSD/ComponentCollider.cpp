#include <X-GSD/ComponentCollider.hpp>

#include <X-GSD/Game.hpp> // Included here to avoid circular reference

using namespace xgsd;

ComponentCollider::ComponentCollider(sf::FloatRect rectBounds)
: mRectBounds(rectBounds)
{
    // Load resources here (RAII)
    
#ifdef DEBUG
    mDebugTriggeredRecently = false;
    mDebugTriggeredDuration = ONE_SECOND;
    mDebugTriggeredTime = HiResDuration(0);
    
    mDebugRectangle.setSize(sf::Vector2f(mRectBounds.width, mRectBounds.height));
    mDebugRectangle.setOutlineColor(sf::Color::Green);
    mDebugRectangle.setFillColor(sf::Color::Transparent);
#endif
}

void ComponentCollider::onEntityAttach()
{
    // Check if the entity has a RigidBody component or not to decide the type of collider (static or dynamic)
    if (entity->getComponent<ComponentRigidBody>()) {
        Game::instance().getPhysicsEngine().addDynamicCollider(this);
        mStatic = false;
    }
    else {
        Game::instance().getPhysicsEngine().addStaticCollider(this);
        mStatic = true;
    }
    
    // Check if the entity has a Sprite component and mRectBounds has not been set yet
    if (mRectBounds == sf::FloatRect()) {
        if (auto sprite = entity->getComponent<ComponentSprite>()) {
            mRectBounds = sprite->getGlobalBounds();
#ifdef DEBUG
            mDebugRectangle.setSize(sf::Vector2f(mRectBounds.width, mRectBounds.height));
#endif
        }
    }
    
#ifdef DEBUG
    mDebugVisible = Game::instance().isDebugRenderingEnabled();
    mDebugRectangle.setOutlineThickness(1.f/entity->mTransformable.getScale().x);
#endif
    
}

void ComponentCollider::update(const HiResDuration &dt)
{
#ifdef DEBUG
    // Slowly return to green color of the debug rectangle
    if (mDebugTriggeredRecently) {
        mDebugRectangle.setOutlineColor(sf::Color(255 - (float)mDebugTriggeredTime.count() / (float)mDebugTriggeredDuration.count() * 255.f, (float)mDebugTriggeredTime.count() / (float)mDebugTriggeredDuration.count() * 255.f, 0));
        
        mDebugTriggeredTime += dt;
        
        // If the transition time has passed, reset variables
        if (mDebugTriggeredTime >= mDebugTriggeredDuration) {
            mDebugTriggeredRecently = false;
            mDebugTriggeredTime = HiResDuration(0);
            mLastCollidedEntity = nullptr;
        }
    }
#endif
}

void ComponentCollider::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
#ifdef DEBUG
    if (mDebugVisible)
        target.draw(mDebugRectangle, states);
#endif
}

void ComponentCollider::handleEvent(const Event &event)
{
#ifdef DEBUG
    // TODO: Get control/button binding dynamically
    if (event.type == Event::System && event.systemEvent.key.code == sf::Keyboard::V) {
        mDebugVisible = Game::instance().isDebugRenderingEnabled();
        mDebugRectangle.setOutlineThickness(1.f/entity->mTransformable.getScale().x);
    }
#endif
}

void ComponentCollider::collisionHandler(Entity *theOtherEntity, sf::FloatRect collision)
{
#ifdef DEBUG
    // Give visual feedback for debugging collisions
    mDebugTriggeredRecently = true;
    mDebugRectangle.setOutlineColor(sf::Color::Red);
    mDebugTriggeredTime = HiResDuration(0);
#endif
    
#ifdef DETAILED_DEBUG
    // Avoid throwing too many debug messages of collisions between the same colliders
    if (mLastCollidedEntity != theOtherEntity) {
        mLastCollidedEntity = theOtherEntity;
        DBGMSGC("COLLISION: [" << entity->getName() << "] collided with [" << theOtherEntity->getName() << "]");
    }
#endif
}

void ComponentCollider::setStatic(bool option)
{
    // If mStatic is equal to option, there is no need to do anything as the Collider is already the desired type
    if (mStatic != option) {
        
        // Swap the collider from one type to the other
        if (option) {
            Game::instance().getPhysicsEngine().addStaticCollider(this);
            Game::instance().getPhysicsEngine().deleteDynamicCollider(this);
        }
        else {
            Game::instance().getPhysicsEngine().addDynamicCollider(this);
            Game::instance().getPhysicsEngine().deleteStaticCollider(this);
        }
    }
    
}

bool ComponentCollider::isStatic() {
    return mStatic;
}

void ComponentCollider::setRectBounds(sf::FloatRect rect)
{
    mRectBounds = rect;
#ifdef DEBUG
    mDebugRectangle.setSize(sf::Vector2f(mRectBounds.width, mRectBounds.height));
#endif
}

sf::FloatRect ComponentCollider::getRectBounds()
{
    return mRectBounds;
}

ComponentCollider::~ComponentCollider()
{
    // Cleanup
    if (mStatic)
        Game::instance().getPhysicsEngine().deleteStaticCollider(this);
    else
        Game::instance().getPhysicsEngine().deleteDynamicCollider(this);
}
