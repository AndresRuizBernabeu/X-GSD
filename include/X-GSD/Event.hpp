#pragma once

#include <SFML/Window/Event.hpp>

namespace xgsd {
    
    /*
     Event class. Wrapper for sf::Event which adds a CustomEvent type. CustomEvent has two basic std::string
     fields: name and data. Only name is compulsory. These CustomEvents can be used to broadcast custom
     events or messages to the Entities in the scene graph (e.g. an enemy can broadcast a "enemyDestroyed"
     event and carry any data, e.g. its name, on its destructor so that any interested Entity can take
     actions or just ignore it).
     */
    class Event
    {
    public:
        ~Event() {}
        
        struct CustomEvent
        {
            CustomEvent(std::string name, std::string data = "") : name(name), data(data) { }
            
            std::string name;
            std::string data;
        };
        
        enum EventType
        {
            System,
            Custom
        };
        
        // Constructors
        Event(sf::Event systemEvent) : type(System), systemEvent(systemEvent) { }
        Event(CustomEvent customEvent) : type(Custom), customEvent(customEvent) { }
        
        // Type of the event
        EventType type;
        
        union {
            sf::Event   systemEvent;
            CustomEvent customEvent;
        };
        
    };
    
} // namespace xgsd
