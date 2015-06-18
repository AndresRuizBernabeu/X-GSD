#include "TitleController.hpp"


TitleMenuController::TitleMenuController()
: mBackground()
{
    // Load resources here (RAII)
}

void TitleMenuController::onEntityAttach()
{
    // Sound
    mMenuSound.setBuffer(Game::instance().getLocalSoundManager().get("selectionSound"));
    
    // Font
    sf::Font& mainFont = Game::instance().getLocalFontManager().get("mainFont");
    mTextPressAnyKey.setFont(mainFont);
    
    // String
    mTextPressAnyKey.setString("Press any key to start");
    
    // Size
    mTextPressAnyKey.setCharacterSize(14);
    
    // Positioning
    sf::Vector2f viewSize = Game::instance().getWindow().getView().getSize();
    sf::FloatRect textRect = mTextPressAnyKey.getLocalBounds();
    mTextPressAnyKey.setOrigin(textRect.left + textRect.width/2.0f,
                               textRect.top  + textRect.height/2.0f);
    mTextPressAnyKey.setPosition(viewSize.x / 2.0f, viewSize.y / 1.2f);
    
    // Color
    mTextPressAnyKey.setColor(sf::Color::Transparent);
    
    // Background
    mBackground.setSize(viewSize);
    mBackground.setFillColor(sf::Color(50, 100, 200));
}

void TitleMenuController::update(const HiResDuration& dt)
{
    // Update "Press any key" blinking animation
    mPressAnyKeyTime += dt;
    if (mPressAnyKeyTime > ONE_SECOND / 2) {
        if (mTextPressAnyKey.getColor() == sf::Color::White)
            mTextPressAnyKey.setColor(sf::Color::Transparent);
        else
            mTextPressAnyKey.setColor(sf::Color::White);
        mPressAnyKeyTime = xgsd::HiResDuration(0);
    }
}

void TitleMenuController::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // Draw background
    target.draw(mBackground, states);
    
    // Draw text on top of everything
    target.draw(mTextPressAnyKey, states);
}

void TitleMenuController::handleEvent(const Event& event)
{
    
    if (event.type == Event::System) {
        
        auto systemEvent = event.systemEvent;
        
        // Perform appropiate actions
        if (systemEvent.type == sf::Event::KeyPressed ||
            systemEvent.type == sf::Event::JoystickButtonPressed)
        {
            
            // Pressing Esc will close the game
            if (systemEvent.key.code == sf::Keyboard::Escape) {
                Game::instance().getWindow().close();
                return;
            }
            
            // Any other key or button:
            mMenuSound.play();
            
            // Load Game scene
            Game::instance().getSceneManager().loadSceneFromFile("GameScene.json");
            
        }
    }
}

TitleMenuController::~TitleMenuController()
{
    // Cleanup
}
