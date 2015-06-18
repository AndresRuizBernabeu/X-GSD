#pragma once

#include <X-GSD/Component.hpp>
#include <X-GSD/Entity.hpp> // Completes forward declaration in Component
#include <X-GSD/PhysicState.hpp>

#include <SFML/Graphics/Transformable.hpp>

namespace xgsd {
    
    /*
     ComponentRigidBody class. Add this component to an entity to give it physics behavior (the entity will
     get its position updated according to physics). If the Entity also holds a ComponentCollider, it will
     trigger collisions. If the ComponentRigidBody is set as Kinematic, the Entity will not get its position
     automatically updated, but it will still trigger collisions.
     
     Note: This component does not offer automatic physics-based collision reactions (i.e. preventing the
     player from falling through the floor, or making a ball to bounce). The desired reactions must be
     implemented in a custom Component (a controller), or extend the functionality of this class to provide
     automatic physics-based collision reactions.
     */
    class ComponentRigidBody : public Component
    {
        // Methods
    public:
        ComponentRigidBody(bool isKinematic);
        ~ComponentRigidBody();
            
        void                onEntityAttach() override;
        void                update(const HiResDuration& dt) override;
        
        void                returnToLastPhysicsState();
        // TODO: override collisionHandler method for a physics automatic collision response (modify the physics state accordingly to that collision)
        
        // TODO: Add setters and getters regarding the mPhysics and use them at JSON scene loading
        PhysicState&        getPhysicsState();
        
        void                pausePhysics();
        void                resumePhysics();
        
        // Variables (member / properties)
    private:
        
        bool                mKinematic;
        bool                mPausedPhysics;
        
        PhysicState         mPhysics;
        PhysicState         mLastPhysicsState;
        sf::Transformable   mLastTransformable;
        
        // TODO: More member variables such as "affectedByGravity", "friction", "airFriction" or "bounceRatio"
    };
} // namespace xgsd
