#include <X-GSD/PhysicsEngine.hpp>

#include <SFML/Graphics/Rect.hpp>

using namespace xgsd;

// Very basic collision detection algorithm based on axis-aligned bounding boxes intersection
void PhysicsEngine::checkCollisions() {
    
    // Check between dynamic colliders first (entities which have a collider and a rigidBody)
    for (auto iterD = dynamicColliders.begin(); iterD != dynamicColliders.end(); ++iterD) {
        
        // If the entity containing this collider is pending of destruction, skip it
        if ((*iterD)->entity->isDestroyPending())
            continue;
        
        sf::FloatRect intersection;
        
        // Get the global bounds rect of one entity
        sf::FloatRect rectD = (*iterD)->entity->getWorldTransform().transformRect((*iterD)->getRectBounds());
        
        // Check between dynamic colliders (entities which have a collider and a rigidBody)
        for (auto iterD2 = std::next(iterD); iterD2 != dynamicColliders.end(); ++iterD2) {
            
            /* The starting point of the iterator is the next item of iterD because dynamic colliders
             must check collision between them avoiding repetition (1-2 is the same as 2-1). Also,
             it avoids self-collision detecton (1-1, 2-2, etc).
             */
            
            // If the entity containing this collider is pending of destruction, skip it
            if ((*iterD2)->entity->isDestroyPending() || (*iterD)->entity->isDestroyPending())
                continue;
            
            // Get the global bounds rect of the other entity
            sf::FloatRect rectD2 = (*iterD2)->entity->getWorldTransform().transformRect((*iterD2)->getRectBounds());
            
            // Check collision with SFML and call collisionHandler of both entities
            if(rectD.intersects(rectD2, intersection)){
                (*iterD)->entity->collisionHandler((*iterD2)->entity, intersection);
                (*iterD2)->entity->collisionHandler((*iterD)->entity, intersection);
            }
        }
        
        // Check between dynamic (entities which have a collider and a rigidBody) and static colliders (entities which have a collider but no rigidBody)
        for (auto iterS = staticColliders.begin(); iterS != staticColliders.end(); ++iterS) {
            
            // If the entity containing this collider is pending of destruction, skip it
            if ((*iterS)->entity->isDestroyPending() || (*iterD)->entity->isDestroyPending())
                continue;
            
            // Get the global bounds rect of the other entity
            sf::FloatRect rectS = (*iterS)->entity->getWorldTransform().transformRect((*iterS)->getRectBounds());
            
            // Check collision with SFML and call collisionHandler of both entities
            if(rectD.intersects(rectS, intersection)){
                (*iterD)->entity->collisionHandler((*iterS)->entity, intersection);
                (*iterS)->entity->collisionHandler((*iterD)->entity, intersection);
            }
        }
    }
}

void PhysicsEngine::addStaticCollider(xgsd::ComponentCollider *collider)
{
    auto inserted = staticColliders.insert(collider);
    assert(inserted.second);
}

void PhysicsEngine::addDynamicCollider(xgsd::ComponentCollider *collider)
{
    auto inserted = dynamicColliders.insert(collider);
    assert(inserted.second);
}

void PhysicsEngine::deleteStaticCollider(xgsd::ComponentCollider *collider)
{
    auto found = staticColliders.find(collider);
    assert(found != staticColliders.end());
    staticColliders.erase(collider);
}

void PhysicsEngine::deleteDynamicCollider(xgsd::ComponentCollider *collider)
{
    auto found = dynamicColliders.find(collider);
    assert(found != dynamicColliders.end());
    dynamicColliders.erase(collider);
}

// Simple integrator. Cheap, but accumulates a lot of error as time advances.
void PhysicsEngine::integrateEuler(PhysicState& physics,
                                   PhysicState& lastPhysics,
                                   sf::Transformable& transf,
                                   sf::Transformable& lastTransf,
                                   const HiResDuration& dt)
{
    float dtValue = ((float)dt.count()/ONE_SECOND.count());
    
    // Save last physics state and transformable
    lastPhysics = physics;
    lastTransf = transf;
    transf.setPosition(transf.getPosition() + physics.getVelocity() * dtValue);
    physics.setVelocity(physics.getVelocity() + (physics.getForce() / physics.getMass()) * dtValue);
}

// More accurate than Euler, but a bit more resource-consuming.
void PhysicsEngine::integrateRK4(PhysicState& physics,
                                 PhysicState& lastPhysics,
                                 sf::Transformable& transf,
                                 sf::Transformable& lastTransf,
                                 const HiResDuration& dt)
{
    float dtValue = ((float)dt.count()/ONE_SECOND.count());
    
    // Save last physics state and transformable
    lastPhysics = physics;
    lastTransf = transf;
    
    Derivative a,b,c,d;
    Derivative initial;
    
    a = evaluateRK4(physics, transf, HiResDuration::zero(), initial);
    b = evaluateRK4(physics, transf, dt / 2, a);
    c = evaluateRK4(physics, transf, dt / 2, b);
    d = evaluateRK4(physics, transf, dt, c);
    
    sf::Vector2f dpdt = 1.0f / 6.0f *
    ( a.dp + 2.0f * (b.dp + c.dp) + d.dp );
    
    sf::Vector2f dvdt = 1.0f / 6.0f *
    ( a.dv + 2.0f * (b.dv + c.dv) + d.dv );
    
    transf.setPosition(transf.getPosition() + dpdt * dtValue);
    physics.setVelocity(physics.getVelocity() + dvdt * dtValue);
}

// Auxiliar function for RK4 integrator
Derivative PhysicsEngine::evaluateRK4(const PhysicState& initialPhysics,
                                      const sf::Transformable& initialTransf,
                                      const HiResDuration& dt,
                                      const Derivative& d)
{
    float dtValue = ((float)dt.count()/ONE_SECOND.count());
    
    PhysicState physics;
    sf::Transformable transf;
    transf.setPosition(initialTransf.getPosition() + d.dp * dtValue); // Add the velocity to position
    physics.setVelocity(initialPhysics.getVelocity() + d.dv * dtValue); // Add the acceleration to velocity
    physics.setForce(initialPhysics.getForce());
    physics.setMass(initialPhysics.getMass());
    
    Derivative output;
    output.dp = physics.getVelocity();
    output.dv = accelerationRK4(physics);
    return output;
}

// Auxiliar function for RK4 integrator
sf::Vector2f PhysicsEngine::accelerationRK4(const PhysicState& physics)
{
    return physics.getForce() / physics.getMass();
}