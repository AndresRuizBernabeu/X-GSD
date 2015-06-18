#pragma once

#include <X-GSD/Time.hpp>
#include <X-GSD/Debug.hpp>
#include <X-GSD/Event.hpp>

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace xgsd {
    
    // Forward declaration
    class Entity;
    
    /*
     Component class. Used as base class for specific components. Components can be added to Entities to
     add functionality. It also serves as base class to be inherited from user-defined controllers (the name
     "controller" refers to Components created by a user of X-GSD and added to any Entity to give it custom
     behaviour. It is the "scripting" part of X-GSD).
     */
    class Component : sf::NonCopyable
    {
        // Typedefs and enumerations
    public:
        typedef std::unique_ptr<Component> Ptr;
        
        // Methods
    public:
        Component();
        virtual ~Component();
        
        virtual void            attachedToEntity();
        virtual void            detachedFromEntity();
        
        virtual void            onEntityAttach();
        virtual void            onEntityDetach();
        virtual void            onPause(const HiResDuration& dt);
        
        virtual void            update(const HiResDuration& dt);
        virtual void            draw(sf::RenderTarget& target, sf::RenderStates states) const;
        virtual void            handleEvent(const Event& event);
        
        // Callbacks for other common components
        virtual void            collisionHandler(Entity* theOtherEntity, sf::FloatRect collision);
        
        // Variables (member / properties)
    public:
        Entity*                 entity;
        
    };
    
} // namespace xgsd
