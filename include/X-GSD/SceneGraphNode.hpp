// Modified version of SceneNode.hpp from SFML Game Development Book

#pragma once

#include <X-GSD/Time.hpp>
#include <X-GSD/Debug.hpp>
#include <X-GSD/Event.hpp>

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>

#include <vector>
#include <cassert>

namespace xgsd {
    
    /* SceneGraphNode class. It is the core element of the Scene and a key element of the game: All entities
     inherit from SceneGraphNode, so they are in fact SceneGraphNodes with added functionality.
     This class manages scene graph operations (update, draw, handleEvent...). It has a collection of
     children SceneGraphNodes and a reference to the parent. Actions like update, draw and handleEvent are
     performed on this node first, and then invoked on all of its children. As the node can have children nodes,
     attachChild, detachChild operations can be used to make or destroy these relationships.
     */
    class SceneGraphNode : public sf::Drawable, private sf::NonCopyable
    {
        // Typedefs and enumerations
    public:
        typedef std::unique_ptr<SceneGraphNode> Ptr;
        
        // Methods
    public:
        SceneGraphNode();
        virtual             ~SceneGraphNode();
        
        void                requestDetach(SceneGraphNode* child);
        void                requestAttach(Ptr child);
        void                requestDestroy();
        void                performPendingSceneGraphOperations(); // Attach, detach, destroy
        
        void                onAttach();
        void                onDetach();
        void                onPause(const HiResDuration &dt);

        void                update(const HiResDuration& dt);
        void                draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void                handleEvent(const Event& event);
        
        sf::Vector2f        getWorldPosition() const;
        sf::Transform       getWorldTransform() const;
        SceneGraphNode&     getParent();
        bool                isDestroyPending();
        
    private:
        virtual void        onAttachThis();
        void                onAttachChildren();
        
        virtual void        onDetachThis();
        void                onDetachChildren();
        
        virtual void        onPauseThis(const HiResDuration& dt);
        void                onPauseChildren(const HiResDuration& dt);
        
        virtual void        updateThis(const HiResDuration& dt);
        void                updateChildren(const HiResDuration& dt);

        virtual void        handleEventThis(const Event& event);
        void                handleEventChildren(const Event& event);
        
        virtual void        drawThis(sf::RenderTarget& target, sf::RenderStates states) const;
        void                drawChildren(sf::RenderTarget& target, sf::RenderStates states) const;
        
        void                attachChild(Ptr child);
        Ptr                 detachChild(SceneGraphNode& child);
        void                destroy();
        
        // Variables (member / properties)
    public:
        sf::Transformable               mTransformable;
        
    private:
        std::vector<Ptr>                mChildren;
        SceneGraphNode*                 mParent;
        
        std::vector<SceneGraphNode*>    mPendingDetachments;
        std::vector<Ptr>                mPendingAttachments;
        bool                            mPendingDestruction;
    };
    
} // namespace xgsd
