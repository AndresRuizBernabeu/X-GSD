#pragma once

#include <X-GSD/Time.hpp>
#include <X-GSD/Entity.hpp>
#include <X-GSD/ControllersManager.hpp>
#include <X-GSD/PhysicsEngine.hpp>
#include <X-GSD/SceneGraphNode.hpp>
#include <X-GSD/ResourceManager.hpp>
#include <X-GSD/Event.hpp>

#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <unordered_map>


namespace xgsd {
    
    class Game;
    
    /*
     Scene class. Abstraction to organize and group different elements of the game.
     This class also manages loading, unloading, update/render/events calls, etc.
     Contains the main SceneGraphNode (the root element of the scene) and some resource managers to store
     and access fonts, textures, sounds, etc. loaded to this specific scene (or global resources).
     */
    
    class Scene
    {
        // Typedefs and enumerations
    public:
        typedef std::unique_ptr<Scene>    Ptr;
        
    private:
        enum TransitionState {
            none,
            in,
            inFinished,
            out,
            outFinished
        };
        
        // Methods
    public:
        Scene(sf::RenderWindow& window);
        ~Scene();
        
        void                    update(const HiResDuration &dt);
        void                    render();
        void                    handleEvent(const Event &event);
        
        std::string             getName();
        FontManager&            getLocalFontManager()           { return *mFontManager; }
        TextureManager&         getLocalTextureManager()        { return *mTextureManager; }
        SoundManager&           getLocalSoundManager()          { return *mSoundManager; }
        ControllersManager&     getControllersManager()         { return mControllersManager; }
        
        void                    addNode(SceneGraphNode::Ptr node);
        
        void                    loadSceneFromFile(std::string jsonPath);
        
        bool                    isTransitionEnabled();
        void                    setTransitionEnabled(bool option);
        void                    pauseGame();
        void                    resumeGame();
        
    private:
        void                    readJsonSceneFile(std::string jsonPath);
        void                    performSceneChange();
        void                    unloadScene();
        
        void                    updateTransition(const HiResDuration &dt);
        void                    renderTransition();
        
        
        // Variables (member / properties)
    private:
        std::string             mName;
        SceneGraphNode::Ptr     mSceneGraph;
        sf::RenderWindow&       mWindow;
        sf::View                mSceneView; // Camera
        std::string             mNextScenePath;
        bool                    mSceneChangeRequest;
        bool                    mPaused;
        
        // Resources managers
        FontManager::Ptr        mFontManager;
        TextureManager::Ptr     mTextureManager;
        SoundManager::Ptr       mSoundManager;
        
        ControllersManager      mControllersManager;
        
        PhysicsEngine&          mPhysicsEngine;
        
        bool                    mTransitionEnabled;
        TransitionState         mTransitionState;
        HiResDuration           mTransitionTime;
        HiResDuration           mTransitionDuration;
        sf::RectangleShape      mTransitionFadingRectangle;
    };
    
} // namespace xgsd
