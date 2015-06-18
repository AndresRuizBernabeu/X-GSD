#include <X-GSD/ComponentRigidBody.hpp>

#include <X-GSD/ComponentCollider.hpp>
#include <X-GSD/PhysicsEngine.hpp>

using namespace xgsd;

ComponentRigidBody::ComponentRigidBody(bool isKinematic)
: mKinematic(isKinematic)
, mPausedPhysics(false)
{
    // Load resources here (RAII)
    
    // Set a default gravity force
    mPhysics.setForce(sf::Vector2f(0.f, 200)); // TODO: Change this to be done only if an "affectedByGravity" flag is true
}

void ComponentRigidBody::onEntityAttach()
{
    // Check if the Entity has a collider attached
    if (auto collider = entity->getComponent<ComponentCollider>()) {
        collider->setStatic(false); // If it existed, make it dynamic now
    }
    else {
        DBGMSGC("WARNING: a RigidBody has been attached to " << entity->getName() << " but it has no Collider. This may cause undesired behaviour.");
    }
}

void ComponentRigidBody::update(const HiResDuration &dt)
{
    // Do not update physics if is kinematic. Its movement will be updated manually elsewhere
    if (mKinematic || mPausedPhysics)
        return;
    
    // Advance the physics with RK4 integration
    PhysicsEngine::integrateRK4(mPhysics, mLastPhysicsState, entity->mTransformable, mLastTransformable, dt);
}

void ComponentRigidBody::returnToLastPhysicsState()
{
    mPhysics = mLastPhysicsState;
    entity->mTransformable = mLastTransformable;
}

PhysicState& ComponentRigidBody::getPhysicsState()
{
    return mPhysics;
}

void ComponentRigidBody::pausePhysics()
{
    if (mKinematic)
        DBGMSGC("Warning: Tried to pause the physics simulation of a kinematic ComponentRigidBody.");
    else
        mPausedPhysics = true;
}

void ComponentRigidBody::resumePhysics()
{
    if (mKinematic)
        DBGMSGC("Warning: Tried to resume the physics simulation of a kinematic ComponentRigidBody.");
    else
        mPausedPhysics = false;
}


ComponentRigidBody::~ComponentRigidBody()
{
    // Cleanup
}