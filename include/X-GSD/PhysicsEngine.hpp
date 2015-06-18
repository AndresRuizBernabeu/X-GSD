#pragma once

#include <X-GSD/Time.hpp>
#include <X-GSD/PhysicState.hpp>
#include <X-GSD/ComponentCollider.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include <set>

namespace xgsd {
    
    // Needed struct for RK4 method.
    struct Derivative
    {
        sf::Vector2f dp; // dp/dt = velocity
        sf::Vector2f dv; // dv/dt = acceleration
    };
    
    class PhysicsEngine
    {
        // Typedefs and enumerations
    public:
        typedef std::unique_ptr<PhysicsEngine>    Ptr;
        
        // Methods
    public:
        void    checkCollisions();
        
        void    addStaticCollider(ComponentCollider* collider);
        void    addDynamicCollider(ComponentCollider* collider);
        void    deleteStaticCollider(ComponentCollider* collider);
        void    deleteDynamicCollider(ComponentCollider* collider);
        
    private:
        Derivative static       evaluateRK4(const PhysicState& initialPhysics,
                                            const sf::Transformable& initialTransf,
                                            const HiResDuration& dt,
                                            const Derivative& d);
        
        sf::Vector2f static     accelerationRK4(const PhysicState&
                                                physics);
        
        // Class methods
    public:
        void static             integrateEuler(PhysicState& physics,
                                               PhysicState& lastPhysics,
                                               sf::Transformable& transf,
                                               sf::Transformable& lastTransf,
                                               const HiResDuration& dt);
        
        void static             integrateRK4(PhysicState& physics,
                                             PhysicState& lastPhysics,
                                             sf::Transformable& transf,
                                             sf::Transformable& lastTransf,
                                             const HiResDuration& dt);
        
        // Variables (member / properties)
    private:
        std::set<ComponentCollider*>    staticColliders; // Colliders whose Entity does not have a RigidBody
        std::set<ComponentCollider*>    dynamicColliders; // Colliders whose Entity has a RigidBody
    };
    
} // namespace xgsd