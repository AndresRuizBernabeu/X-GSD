#include "GameController.hpp"

GameController::GameController()
: mGameOverTime(0)
, mDurationBetweenasteroidSpawns(ONE_SECOND * 2)
, mTimeBetweenasteroidSpawns(ONE_SECOND * 1)
, mCentralTextRectangle()
, mScreenFrame()
, mPoints(0)
, mPointsMultiplier(1)
, mDestroyedAsteroids(0)
, mCreatedAsteroids(0)
, mPointsText()
, mGameOver(false)
, mPaused(false)
, mStopped(false)
{
	// Load resources here (RAII)
	
	// Initialize random system
	std::random_device randomSeed;
	randomEngine = std::default_random_engine(randomSeed());
	randomasteroidPositionValues = std::uniform_int_distribution<int>(60, 600);
	randomasteroidTextureValues = std::uniform_int_distribution<int>(1, 3);
}

void GameController::onEntityAttach()
{
	sf::Vector2f viewSize = Game::instance().getWindow().getView().getSize();
	
	// Points text configuration
	mPointsText.setFont(Game::instance().getLocalFontManager().get("mainFont"));
	mPointsText.setCharacterSize(20);
	mPointsText.setString("0");
	mPointsText.setPosition(viewSize.x - 100, 40);
	mPointsText.setColor(sf::Color::White);

	// Pause text
	mCentralText.setFont(Game::instance().getLocalFontManager().get("mainFont"));
	mCentralText.setCharacterSize(32);
	mCentralText.setString("PAUSE");
	mCentralText.setPosition(viewSize.x / 2.f - mCentralText.getLocalBounds().width/ 2.f, viewSize.y / 2.f - mCentralText.getLocalBounds().height / 2.f);
	mCentralText.setColor(sf::Color::White);
	
	// Set the central text rectangle (background with alpha, so that the text is more readable)
	mCentralTextRectangle.setFillColor(sf::Color(0, 0, 0, 200));
	mCentralTextRectangle.setPosition(0, viewSize.y / 2.5f);
	mCentralTextRectangle.setSize(sf::Vector2f(viewSize.x, 80));
	
	// Set sounds
	mPauseSound.setBuffer(Game::instance().getLocalSoundManager().get("pauseSound"));
	mExplosionSound.setBuffer(Game::instance().getLocalSoundManager().get("explosion"));
	mMelody.setBuffer(Game::instance().getLocalSoundManager().get("melody"));
	mMelody.play();
	mMelody.setLoop(true);
	
	// Set the background
	mBackground.setTexture(&Game::instance().getLocalTextureManager().get("BGTexture"));
	mBackground.setTextureRect(sf::IntRect(15, 15, Game::instance().getWindow().getView().getSize().x-15, Game::instance().getWindow().getView().getSize().y-15));
	mBackground.setSize(sf::Vector2f(Game::instance().getWindow().getView().getSize().x, Game::instance().getWindow().getView().getSize().y));
}

void GameController::update(const xgsd::HiResDuration& dt)
{
	// If game has ended and timer is over, perform transition
	if ((mGameOver) && mGameOverTime < ONE_SECOND * 3)
		mGameOverTime += dt;
	else if ((mGameOver) && mGameOverTime > ONE_SECOND * 3) {
		if (mGameOver)
			Game::instance().getSceneManager().loadSceneFromFile("TitleScene.json");
	}
	
	// Spawn asteroids
	if (!mGameOver && !mStopped && mTimeBetweenasteroidSpawns < mDurationBetweenasteroidSpawns)
		mTimeBetweenasteroidSpawns += dt;
	if (!mGameOver && !mStopped && mTimeBetweenasteroidSpawns > mDurationBetweenasteroidSpawns) {
		spawnAsteroid();
		mTimeBetweenasteroidSpawns = HiResDuration(0);
	}
}

void GameController::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	// Draw the background
	target.draw(mBackground, states);
	
	// Draw the screen's frame
	target.draw(mScreenFrame, states);
	
	// Draw central text with background
	if (mPaused || mGameOver) {
		target.draw(mCentralTextRectangle, states);
		target.draw(mCentralText, states);
	}
	
	// Draw player information
	target.draw(mPointsText, states);
}

void GameController::handleEvent(const Event& event)
{
	// Handle system events
	if (event.type == Event::System) {
		
		// If the game continues
		if (!mGameOver) {
			
			// Check if Return has been pressed on keyboard, or Start on the first joystick controller
			if ((event.systemEvent.type == sf::Event::KeyPressed && event.systemEvent.key.code == sf::Keyboard::Return) || (event.systemEvent.type == sf::Event::JoystickButtonPressed && event.systemEvent.joystickButton.button == 3 && event.systemEvent.joystickConnect.joystickId == 1)) {
				pause();
			}
		}
	}
	
	// Handle custom events
	if (event.type == Event::Custom) {
		
		if (event.customEvent.name == "PlayerDestroyed") {

            gameOver();
        }
		
		else if (event.customEvent.name == "AsteroidDestroyed") {
			
			mDestroyedAsteroids++;
			
			mExplosionSound.play();
			
			mPointsMultiplier++;
			
            mPoints += 100 * mPointsMultiplier;
            mPointsText.setString(std::to_string(mPoints));
		}
	}
}

void GameController::gameOver()
{
	DBGMSGC("GAME OVER :(");
	mGameOver = true;
	mCentralText.setString("GAME OVER");
	sf::Vector2f viewSize = Game::instance().getWindow().getView().getSize();
	mCentralText.setPosition(viewSize.x / 2.f - mCentralText.getLocalBounds().width/ 2.f, viewSize.y / 2.f - mCentralText.getLocalBounds().height / 2.f);
}

void GameController::pause()
{
	mPauseSound.play();
	
	mPaused = !mPaused;
	mCentralText.setString(mPaused ? "PAUSE" : "GAME OVER");
	
	if (mPaused)
		Game::instance().getSceneManager().pauseGame();
	else
		Game::instance().getSceneManager().resumeGame();
}

void GameController::spawnAsteroid()
{
	// Create the two new asteroids entities
	std::string asteroidName = "asteroid_" + std::to_string(++mCreatedAsteroids);
	Entity::Ptr asteroidEntity(new Entity(asteroidName));
	
	// Set its position
	asteroidEntity->mTransformable.setPosition(randomasteroidPositionValues(randomEngine), -50);
	
	// Create the required components
	int randomTexture = randomasteroidTextureValues(randomEngine);
	std::string textureName;
	
	switch (randomTexture) {
		case 1:
			textureName = "asteroid1";
			break;
		case 2:
			textureName = "asteroid2";
			break;
		case 3:
			textureName = "asteroid3";
			break;
	}
	ComponentSprite::Ptr asteroidSprite(new ComponentSprite(Game::instance().getLocalTextureManager().get(textureName)));
	
	// Create the rigidbody and set initial velocity
	ComponentRigidBody *rb = new ComponentRigidBody(false);
    rb->getPhysicsState().getVelocityRef().y = 25;// + random()%100;
	ComponentRigidBody::Ptr asteroidRigidbody(rb);
	
	// Create the collider
	ComponentCollider::Ptr asteroidCollider(new ComponentCollider());
	
	// Add the components
	asteroidEntity->addComponent(std::move(asteroidSprite));
	asteroidEntity->addComponent(std::move(asteroidRigidbody));
	asteroidEntity->addComponent(std::move(asteroidCollider));
	asteroidEntity->addComponent(Game::instance().getControllersManager().getController("EnemyController"));
	
	// And finally, add it to the scene
	Game::instance().getSceneManager().addNode(std::move(asteroidEntity));
}

GameController::~GameController()
{
	// Cleanup
}
