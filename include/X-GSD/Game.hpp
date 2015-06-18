#pragma once

#include <X-GSD/Scene.hpp>
#include <X-GSD/ResourceManager.hpp>
#include <X-GSD/PhysicsEngine.hpp>

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

#include <unordered_map>

namespace xgsd {
  
  /*
   Game class. The main class of X-GSD, resposible to load the game's initial configuration, keep the game
   running in a specific timestep loop, handle resources and subsystems such as physics engine or events,
   and serve as a global point of access to these resources and subsystems.
   */
  class Game
  {
    // Typedefs and enumerations
  private:
    typedef std::unordered_map<std::string, std::string> StringDataStore;
    
    
    // Methods
  public:
    
    static Game&            instance() { return globalInstance; }
    
    void                    runFixedDeltaTime(int simulationFrequency = 60);
    void                    runVariableDeltaTime();
    void                    runSemiFixedDeltaTime(int simulationFrequency = 60, int stepLimit = 3);
    void                    runFixedSimulationVariableFramerate(int simulationFrequency = 60);
    
    Scene&                  getSceneManager()               { return *mScene; }
    ControllersManager&     getControllersManager()         { return mScene->getControllersManager(); }
    PhysicsEngine&          getPhysicsEngine()              { return mPhysicsEngine; }
    FontManager&            getGlobalFontManager()          { return mFontManager; }
    TextureManager&         getGlobalTextureManager()       { return mTextureManager; }
    SoundManager&           getGlobalSoundManager()         { return mSoundManager; }
    FontManager&            getLocalFontManager()           { return mScene->getLocalFontManager(); }
    TextureManager&         getLocalTextureManager()        { return mScene->getLocalTextureManager(); }
    SoundManager&           getLocalSoundManager()          { return mScene->getLocalSoundManager(); }
    sf::RenderWindow&       getWindow()                     { return *mWindow; }
    HiResDuration           getRunningTime()                { return mTimeSinceStart; }
    
    void                    broadcastEvent(const Event& event);
    
#ifdef DEBUG
    bool                    isDebugRenderingEnabled() { return mDebugRendering; }
    void                    updateStatistics(const HiResDuration& elapsedTime);
#endif
    
  private:
    // Private constructor to ensure the static globalInstance is the only one
    Game();
    void                    loadConfigurationFromFile();
    void                    update(const HiResDuration& dt);
    void                    render();
    void                    handleEvents();
    
    // Variables (member / properties)
  private:
    static Game             globalInstance;
    
    HiResDuration           mTimeSinceStart;
    sf::RenderWindow*       mWindow;
    bool                    mVSync;
    Scene*                  mScene;
    sf::Event               mEvent;
    PhysicsEngine           mPhysicsEngine;
    
    // Resources managers
    FontManager             mFontManager;
    TextureManager          mTextureManager;
    SoundManager            mSoundManager;
    
#ifdef DEBUG
    // Statistics
    bool                    mDebugRendering;
    bool                    mEnableStatistics;
    sf::RectangleShape      mStatisticsBackground;
    sf::Text                mStatisticsText;
    HiResDuration           mStatisticsUpdateTime;
    std::size_t             mStatisticsNumFrames;
    std::size_t             mStatisticsNumSimulationSteps;
#endif
    
  public:
    /*
     This unordered map can be used to pass information between objects of the game (example:
     level1 could store points and remaining lives before loading level2, and level2 would load
     the points and remaining lives)
     */
    StringDataStore         DataStore;
  };
  
} // Namespace xgsd
