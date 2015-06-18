#include "Game.hpp"

#include <json/json.h>

#include <fstream>

using namespace xgsd;

Game Game::globalInstance; // Static initialization of the Game globalInstance

// Constructor
Game::Game()
: mVSync(false)
, mTimeSinceStart(0)
{
    // Load configuration (window properties, first scene to load...)
    loadConfigurationFromFile();
    
#ifdef DEBUG
    mDebugRendering = false;
    mEnableStatistics = false;
    mStatisticsNumFrames = 0;
    mStatisticsNumSimulationSteps = 0;
    mStatisticsUpdateTime = HiResDuration(0);
    
    // Statistics
    mStatisticsText.setPosition(18.f, 18.f);
    mStatisticsText.setCharacterSize(8);
    mStatisticsText.setColor(sf::Color::Green);
    mStatisticsBackground.setPosition(mStatisticsText.getPosition());
    mStatisticsBackground.setFillColor(sf::Color(0, 0, 0, 200));
#endif
}

void Game::loadConfigurationFromFile()
{
    Json::Value root;   // will contains the root value after parsing
    Json::Reader reader;
    
    std::string jsonPath = "gameconfig.json";
    
    DBGMSGC("Configuration JSON file to parse: " << resourcePath() << jsonPath);
    
    std::ifstream jsonFile(resourcePath() + jsonPath, std::ifstream::binary);
    
    bool parsingSuccessful = reader.parse(jsonFile, root);
    if ( !parsingSuccessful )
    {
        // report to the user the failure and their locations in the document.
        throw std::runtime_error("Failed to parse file\n" + reader.getFormattedErrorMessages());
    }
    
    // Get window name
    std::string windowName = root.get("windowName", "").asString();
    
    // Get window size
    auto windowSizeJson = root["windowSize"];
    sf::Vector2u windowSize;
    
    if (!windowSizeJson) {
        windowSize = sf::Vector2u(800, 600);
        DBGMSGC("No windowSize defined on gameconfig.json - Applying default 800x600 windowSize.");
    }
    else {
        auto width = windowSizeJson["width"];
        auto height = windowSizeJson["height"];
        
        if (!width || !height || !width.isInt() || !height.isInt()) {
            windowSize = sf::Vector2u(800, 600);
            DBGMSGC("Width and/or height of windowSize wrongly defined on gameconfig.json - Applying default 800x600 windowSize.");
        } else {
            windowSize = sf::Vector2u(width.asInt(), height.asInt());
        }
    }
    
    // Get fullscreen or windowed mode
    auto fullscreenJson = root["fullscreen"];
    bool fullscreen;
    
    if (!fullscreenJson || !fullscreenJson.isBool()) {
        fullscreen = false;
        DBGMSGC("No fullscreen properly defined on gameconfig.json - Applying default windowed mode.");
    }
    else {
        fullscreen = fullscreenJson.asBool();
    }
    
    // Get vsync
    auto vsyncJson = root["vsync"];
    bool vsync;
    
    if (!fullscreenJson || !fullscreenJson.isBool()) {
        vsync = true;
        DBGMSGC("No vsync properly defined on gameconfig.json - Applying default vsync on.");
    }
    else {
        vsync = vsyncJson.asBool();
    }
    
    // Get keyRepetition
    auto keyRepetitionJson = root["keyRepetition"];
    bool keyRepetition;
    
    if (!keyRepetitionJson || !keyRepetitionJson.isBool()) {
        keyRepetition = false;
        DBGMSGC("No vsync keyRepetition defined on gameconfig.json - Applying default keyRepetition off.");
    }
    else {
        keyRepetition = keyRepetitionJson.asBool();
    }
    
    // Get mouseCursorVisible
    auto mouseCursorVisibleJson = root["mouseCursorVisible"];
    bool mouseCursorVisible;
    
    if (!mouseCursorVisibleJson || !mouseCursorVisibleJson.isBool()) {
        mouseCursorVisible = true;
        DBGMSGC("No mouseCursorVisible properly defined on gameconfig.json - Applying default mouseCursorVisible on.");
    }
    else {
        mouseCursorVisible = keyRepetitionJson.asBool();
    }
    
#ifdef DEBUG
    // Get debug font
    std::string debugFontPath = root.get("debugFont", "").asString();
    
    if (debugFontPath != "") {
        mFontManager.load("debugFont", debugFontPath);
        mStatisticsText.setFont(mFontManager.get("debugFont"));
    }
#endif
    
    // Get initialScene
    std::string initialScene = root.get("initialScene", "").asString();
    
    if (initialScene == "")
        throw std::runtime_error("Game::loadConfigurationFromFile - Failed to load " + resourcePath() + jsonPath + "  - No 'initialScene' found");
    
    // Create the window with the loaded preferences
    mWindow = new sf::RenderWindow(sf::VideoMode(windowSize.x, windowSize.y), windowName, fullscreen ? sf::Style::Fullscreen : sf::Style::Close);
    
    // Get app icon, if any
    std::string iconPath = root.get("icon", "").asString();
    
    if (iconPath != "") {
        mTextureManager.load("icon", iconPath);
        auto texture = mTextureManager.get("icon");
        mWindow->setIcon(texture.getSize().x, texture.getSize().y, texture.copyToImage().getPixelsPtr());
    }
    
    // Set some mWindow's properties
    mWindow->setVerticalSyncEnabled(vsync);
    mWindow->setKeyRepeatEnabled(keyRepetition);
    mWindow->setMouseCursorVisible(mouseCursorVisible);
    
    sf::View view = mWindow->getView();
    view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    mWindow->setView(view);
    
    // Create the scene with that window
    mScene = new Scene(*mWindow);
    
    // And finally, load the initial scene
    mScene->loadSceneFromFile(initialScene);
}

void Game::update(const xgsd::HiResDuration& dt)
{
    
    // Update calls here
    
    // Example:
    /*
     for (auto &entity : entitiesContainer) {
     
     // Call each entity's own update method
     entity.update(dt);
     }
     
     NOTE:
     The recommended approach is to ask the Scene to update
     itself, which will invoke the current scene's update method.
     This is the default implementation:
     */
    
    mScene->update(dt);
    
#ifdef DEBUG
    mStatisticsNumSimulationSteps++;
#endif
    
}

void Game::render()
{
    // Draw calls here
    
    // Example:
    /*
     mWindow->clear();
     
     for (auto &entity : entitiesContainer) {
     // Call each entity's own draw method
     entity.draw();
     }
     
     NOTE:
     The recommended approach is to ask the Scene to render
     itself, which will invoke the current scene's render method.
     This is the default implementation:
     */
    
    mWindow->clear(); // Clear the window before drawing the new frame
    mScene->render();
    
#ifdef DEBUG
    // Statistics will only be rendered if the flag is true and DEBUG mode
    if(mEnableStatistics) {
        mWindow->draw(mStatisticsBackground);
        mWindow->draw(mStatisticsText);
    }
    mStatisticsNumFrames++;
#endif
    
    mWindow->display();
}


// TODO: Get controls/buttons bindings dynamically
void Game::handleEvents()
{
    // while there are pending events...
    while (mWindow->pollEvent(mEvent))
    {
        // Create an event wrapper and use it after
        Event eventWrapper(mEvent);
        
        // check the type of the event...
        switch (mEvent.type)
        {
                // window closed
            case sf::Event::Closed:
                mWindow->close();
                break;
                
                // key pressed
            case sf::Event::KeyPressed:
                
#ifdef DEBUG
                // Show statistics
                if (mEvent.key.code == sf::Keyboard::V) {
                    mEnableStatistics = !mEnableStatistics;
                    mDebugRendering = !mDebugRendering;
                }
                // Toggle vSync
                if (mEvent.key.code == sf::Keyboard::B) {
                    mVSync = !mVSync;
                    mWindow->setVerticalSyncEnabled(mVSync);
                }
                
                // Print total running time on console
                if (mEvent.key.code == sf::Keyboard::T) {
                    double totalMilliseconds = (double)mTimeSinceStart.count()/1000000;
                    DBGMSGC("Total running time: " <<  totalMilliseconds << " ms (" <<
                            (long)totalMilliseconds/1000/60/60 << " hours, " << (long)totalMilliseconds/1000/60%60 << " min, " << (long)totalMilliseconds/1000%60 << " sec)");
                }
#endif
                
                // Propagate KeyPressed event to the scene
                mScene->handleEvent(eventWrapper);
                break;
                
            case sf::Event::JoystickButtonPressed:
                
                // Propagate JoystickButtonPressed event to the scene
                mScene->handleEvent(eventWrapper);
                break;
                
                // TOOD: Add more event types here if needed, or just propagate any event
                
            default:
                break;
        }
    }
}

void Game::broadcastEvent(const Event& event)
{
    mScene->handleEvent(event);
}

#ifdef DEBUG
void Game::updateStatistics(const xgsd::HiResDuration& elapsedTime)
{
    // Statistics will only be updated if the flag is true
    if (!mEnableStatistics)
        return;
    
    mStatisticsUpdateTime += elapsedTime;
    //mStatisticsNumFrames += 1;
    
    if (mStatisticsUpdateTime >= ONE_SECOND)
    {
        mStatisticsText.setString(
                                  "Frames / Second       = " + std::to_string(mStatisticsNumFrames) + " (" + std::to_string((float)mStatisticsUpdateTime.count()/mStatisticsNumFrames/1000000) + " ms per frame)\n" +
                                  "Simulations / Second  = " + std::to_string(mStatisticsNumSimulationSteps) + " (" + std::to_string((float)mStatisticsUpdateTime.count()/mStatisticsNumSimulationSteps/1000000) + " ms per simulation)\n" +
                                  "Simulations / Frame   = " + std::to_string((float)mStatisticsNumSimulationSteps / mStatisticsNumFrames) + "\n" +
                                  "Vertical Sync enabled = " + (mVSync ? "Yes" : "No"));
        
        mStatisticsUpdateTime -= ONE_SECOND;
        mStatisticsNumFrames = 0;
        mStatisticsNumSimulationSteps = 0;
    }
    
    mStatisticsBackground.setSize(sf::Vector2f(mStatisticsText.getLocalBounds().width, mStatisticsText.getLocalBounds().height));
    
}
#endif



/*****************************************/
/***             TIMESTEPS             ***/
/*****************************************/

/* --------------------------
 * Basic Fixed Delta Timestep
 * --------------------------
 *
 * Advances the game's simulation time at a fixed delta time. For correct behaviour, the simulation
 * frequency must match the FPS. If not:
 * - If simulation frequency is lower than FPS (or simulation time is higher than frame time) -> Faster
 *   simulation. The game will look faster.
 * - If simulation frequency is higher than FPS (or simulation time is lower than frame time) -> Slower
 *   simulation. The game will look slower.
 *
 * Uses: With V-Sync activated, good for mobile platforms, where system events (notifications,
 * updates, incomming call...) may break the game experience. As with this kind of timestep
 * the game would slowdown instead of render at low fps but advance the game world at a normal
 * time, the player can react better to these system events interruptions.
 */

void Game::runFixedDeltaTime(int simulationFrequency)
{
    HiResDuration simulationFixedDuration(ONE_SECOND/simulationFrequency); // Simulation step
    
    HiResDuration lastRenderDuration; // Frame time (update + handleEvents + render times = 1 loop iteration time)
    
    HiResTime lastTimeMeasure = HiResClock::now();
    HiResTime newTimeMeasure;
    
    while (mWindow->isOpen())
    {
        newTimeMeasure = HiResClock::now();
        lastRenderDuration = newTimeMeasure - lastTimeMeasure;
        lastTimeMeasure = newTimeMeasure;
        
        handleEvents();
        
        update(simulationFixedDuration);
        mTimeSinceStart += simulationFixedDuration;
        
#ifdef DEBUG
        updateStatistics(lastRenderDuration);
#endif
        
        render();
    }
}



/* -----------------------------
 * Basic Variable Delta Timestep
 * -----------------------------
 *
 * Updates as fast as it can with no specified frame time.
 * If it is faster than the display frequency and V-sync is disabled -> No problems. Normal simulation
 * If it the computer is slow and fps are low -> Normal simulation, but big steps can break the physics
 *
 * Uses: General
 * Needs high resolution time compatibility (hardware/OS)
 */

void Game::runVariableDeltaTime()
{
    HiResDuration lastRenderDuration(0); // FrameTime (aka dt)
    
    HiResTime lastTimeMeasure = HiResClock::now();
    HiResTime newTimeMeasure;
    
    while (mWindow->isOpen())
    {
        newTimeMeasure = HiResClock::now();
        lastRenderDuration = newTimeMeasure - lastTimeMeasure;
        lastTimeMeasure = newTimeMeasure;
        
        handleEvents();
        
        update(lastRenderDuration);
        mTimeSinceStart += lastRenderDuration;
        
#ifdef DEBUG
        updateStatistics(lastRenderDuration);
#endif
        
        render();
    }
}




/* -------------------------
 * Semi Fixed Delta Timestep
 * -------------------------
 *
 * Updates the simulation with an upper bound timestep. This ensures the simulation
 * won't be oversampled if vSync is disabled and the simulation completes
 * faster than the display rate, and simulation step will never be greater than
 * a specified value (this guarantees physics to be at least as stable as specified)
 * If X seconds of simulation takes Y seconds of real time, with Y > X, the
 * simulation will slowdown. That can be an acceptable behavior on some heavy
 * load spikes, but this should be only temporary and not taken as normal.
 *
 * Uses: General. With low stepLimit, it is suitable for similar recommended scenarios
 * to use Basic Fixed Timestep: good for mobile platforms, where system events (notifications,
 * updates, incomming call...) may break the game experience. The game would slowdown instead
 * of render at low fps but advance the game world at a normal time, so that the player can
 * react better to these system events interruptions.
 *
 * Needs high resolution time compatibility (hardware/OS)
 */

void Game::runSemiFixedDeltaTime(int minSimulationFrequency, int stepLimit)
{
    // stepLimit must be at least 1, or it would not update the simulation
    assert(stepLimit > 0);
    
    HiResDuration simulationFixedDuration(ONE_SECOND/minSimulationFrequency); // Simulation step time (aka dt)
    
    const short multipleStepLimit = stepLimit; // Limit of steps to consume the frameTime. Needed to avoid the spiral of death effect. Tuning is recommended
    short multipleStepCounter = 0;
    
    HiResDuration lastRenderDuration;
    HiResDuration simulationDuration;
    
    HiResTime lastTimeMeasure = HiResClock::now();
    HiResTime newTimeMeasure;
    
    while (mWindow->isOpen())
    {
        newTimeMeasure = HiResClock::now();
        lastRenderDuration = newTimeMeasure - lastTimeMeasure;
        lastTimeMeasure = newTimeMeasure;
        
#ifdef DEBUG
        updateStatistics(lastRenderDuration); // Update statistics before the variable gets modified
#endif
        
        while (lastRenderDuration > HiResDuration::zero() && multipleStepCounter < multipleStepLimit)
        {
            simulationDuration = std::min(lastRenderDuration, simulationFixedDuration);
            
            handleEvents();
            
            update(simulationDuration);
            
            lastRenderDuration -= simulationDuration;
            mTimeSinceStart += simulationDuration;
            ++multipleStepCounter;
        }
        multipleStepCounter = 0;
        
        
        render();
    }
}



/* ------------------------------------------------------
 * Fixed Delta Timestep with variable rendering framerate
 * ------------------------------------------------------
 *
 * Simulation and Rendering are not coupled with this loop. This can be seen as a producer-consumer model,
 * where the Renderer produces (the time it takes to render) and the Simulation consumes that time
 * in fixed dt chunks.
 * If the Simulation frequency is not divisible by Rendering FPS, a tiny temporal aliasing can appear.
 * To solve it, enable VSync and set simulationFixedDuration to the display Hz, or implement interpolation.
 *
 * Examples:
 * - 60 fps and 60 Hz of simulation would require 1 simulation per frame (no temporal aliasing).
 * - 60 fps and 120 Hz of simulation would require 2 simulations per frame (no temporal aliasing).
 * - 200 fps and 100 Hz of simulation would require 0.5 simulations per frame, which is not allowed
 * as render time must be consumed in fixed dt chunks. Temporal aliasing will appear.
 * - 50 fps and 60 Hz of simulation would require 1.2 simulations per frame. Same as before.
 *
 *
 * Uses: General, deterministic simulations (different runs with the same result), network-related games
 * Needs high resolution time compatibility (hardware/OS)
 */

void Game::runFixedSimulationVariableFramerate(int simulationFrequency)
{
    HiResDuration accumulatedRenderTime(0); // Accumulator of render time to be consumed by simulations
    HiResDuration simulationFixedDuration(ONE_SECOND/simulationFrequency); // This determines the Simulation frequency (in Hz)
    
    HiResDuration lastRenderDuration; // FrameTime
    
    HiResTime lastTimeMeasure = HiResClock::now();
    HiResTime newTimeMeasure;
    
    while (mWindow->isOpen())
    {
        newTimeMeasure = HiResClock::now();
        lastRenderDuration = newTimeMeasure - lastTimeMeasure;
        lastTimeMeasure = newTimeMeasure;
        
        accumulatedRenderTime += lastRenderDuration;
        
        while (accumulatedRenderTime >= simulationFixedDuration)
        {
            accumulatedRenderTime -= simulationFixedDuration;
            
            handleEvents();
            
            update(simulationFixedDuration);
            
            mTimeSinceStart += simulationFixedDuration;
        }
        
#ifdef DEBUG
        updateStatistics(lastRenderDuration);
#endif
        
        render();
    }
}
