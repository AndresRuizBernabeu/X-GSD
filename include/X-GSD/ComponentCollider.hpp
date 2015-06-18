#pragma once

#include <X-GSD/Component.hpp>
#include <X-GSD/Entity.hpp> // Completes forward declaration in Component
#include <X-GSD/Time.hpp>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace xgsd {
    
    // Forward declarations
    class ComponentRigidBody;
    
    /*
     ComponentCollider class. Add this component to an entity to give it a collider (for now, it is only
     a rectangle). The PhysicsEngine can check if Entities holding ComponentColliders are colliding, and
     call their collisionHandlers. The Collider can be Static or Dynamic depending on the use of the Entity:
     
     - Colliders are marked as Static (mStatic = true) if the Entity has no ComponentRigidBody. The main use
     of these type of colliders is for static elements of the game, such as the floor or walls. Another common
     use is for "trigger areas" (e.g. to detect if the player enters a teleport, or a room in which a cinematic has to be played). The PhysicsEngine will not
     check collisions between Static Colliders.
     
     - Colliders are marked as Dynamic (mStatic = false) if the Entity already holds a ComponentRigidBody.
     The main use of Dynamic Colliders is for moving elements of the game, such as the player, the enemies,
     bullets, etc. The PhysicsEngine will check collisions between Dynamic Colliders (e.g. a bullet hit the
     player), and between these and Static Colliders (e.g. the player touches a wall).
     */
    class ComponentCollider : public Component
    {
        // Methods
    public:
        ComponentCollider(sf::FloatRect rectBounds = sf::FloatRect());
        ~ComponentCollider();
        
        void                onEntityAttach() override;
        void                update(const HiResDuration& dt) override;
        void                draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void                handleEvent(const Event& event) override;
        
        void                collisionHandler(Entity* theOtherEntity, sf::FloatRect collision) override;
        void                setStatic(bool option);
        bool                isStatic();
        void                setRectBounds(sf::FloatRect rect);
        sf::FloatRect       getRectBounds();
        
        // Variables (member / properties)
    private:
        bool                mStatic;
        sf::FloatRect       mRectBounds;
        
#ifdef DEBUG
        Entity*             mLastCollidedEntity;
        bool                mDebugVisible;
        sf::RectangleShape  mDebugRectangle;
        bool                mDebugTriggeredRecently;
        HiResDuration       mDebugTriggeredTime;
        HiResDuration       mDebugTriggeredDuration;
#endif
    };
    
    
} // namespace xgsd
