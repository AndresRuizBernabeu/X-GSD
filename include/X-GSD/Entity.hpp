#pragma once

#include <X-GSD/SceneGraphNode.hpp>
#include <X-GSD/Event.hpp>
#include <X-GSD/Component.hpp>

// Include all built-in components
#include <X-GSD/ComponentSprite.hpp>
#include <X-GSD/ComponentRigidBody.hpp>
#include <X-GSD/ComponentCollider.hpp>

#include <SFML/Graphics/RenderTarget.hpp> // Completes the RenderTarget forward declaration in Drawable, inherited from SceneGrahpNode

#include <map>
#include <unordered_map>
#include <typeindex>
#include <cassert>

namespace xgsd {
    
    /*
     Entity class. Entities are the basic and fundamental units of X-GSD. An Entity is a SceneGraphNode
     (an element of a Scene, with all the required scene graph behaviour and properties such as a Transform,
     a reference to a parent node and a collection of children nodes) with added functionality and a unique
     name which identifies it. Entities can hold Components, which add more specific functionality and
     properties.
     */
    class Entity : public SceneGraphNode
    {
        // Typedefs and enumerations
    public:
        typedef std::unique_ptr<Entity>    Ptr;
        
        // Methods
    public:
        Entity(std::string name);
        ~Entity();
        
        void                    onAttachThis() override;
        void                    onDetachThis() override;
        void                    onPauseThis(const HiResDuration &dt) override;

        void                    updateThis(const HiResDuration& dt) override;
        void                    drawThis(sf::RenderTarget& target, sf::RenderStates states) const override;
        void                    handleEventThis(const Event& event) override;
        
        void                    addComponent(Component::Ptr component);
        template <typename T>
        std::unique_ptr<T>      removeComponent();
        template <typename T>
        T*                      getComponent();
        
        void                    collisionHandler(Entity* theOtherEntity, sf::FloatRect collision);
        
        std::string             getName();
        static Entity*          getEntityNamed(std::string name);
        
        // Variables (member / properties)
    private:
        std::map<std::type_index, Component::Ptr>       mComponents;
        std::string                                     mName;
        
        static std::unordered_map<std::string, Entity*> entities; // TODO: Make it non-static. Maybe inside Scene class, so that each scene has a collection of pointers to entities
    };
    
    
    
    /////////////////////////////
    // Template implementation //
    /////////////////////////////
    
    template <typename T>
    std::unique_ptr<T> Entity::removeComponent()
    {
        // Retrieve the type_index of T
        std::type_index index(typeid(T));
        
        // Ensure that a component with type T exists in the collection
        assert(mComponents.count(std::type_index(typeid(T))) != 0);
        
        // Retrieve it from the collection and cast it back to the T polymorphic type
        T *tmp = static_cast<T*>(mComponents[index].get());
        
        // Remove it from the collection and return it
        std::unique_ptr<T> removedComponent(tmp);
        mComponents[index].release();
        mComponents.erase(index);
        removedComponent->detachedFromEntity();
        
        return removedComponent;
    }
    
    template <typename T>
    T* Entity::getComponent()
    {
        // Retrieve the type_index of T
        std::type_index index(typeid(T));
        
        // If it exists in the collection, retrieve it, cast it back to the T polymorphic type and return it
        if (mComponents.count(std::type_index(typeid(T))) != 0) {
            return static_cast<T*>(mComponents[index].get());
        }
        // Otherwise, a nullptr is returned
        else {
            return nullptr;
        }
    }
} // namespace xgsd
