#pragma once

#include <X-GSD/ControllerHeaders.hpp>

#include <random>

using namespace xgsd;

/*
 Controller for the logic of the TitleScene.
 */
class GameController : public Component
{
	// Methods
public:
	GameController();
	~GameController();
	
	void					onEntityAttach() override;
	void					update(const xgsd::HiResDuration& dt) override;
	void					draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void					handleEvent(const Event& event) override;
	
private:
	void					pause();
	void					gameOver();
	void					spawnAsteroid();
	
	// Variables (member / properties)
private:
	HiResDuration			mGameOverTime;
	HiResDuration			mDurationBetweenasteroidSpawns;
	HiResDuration			mTimeBetweenasteroidSpawns;
	
	sf::RectangleShape		mScreenFrame;
	sf::RectangleShape		mBackground;
	sf::RectangleShape		mCentralTextRectangle;
	
	sf::Text				mPointsText;
	
	sf::Sound				mPauseSound;
	sf::Sound				mExplosionSound;
	sf::Sound				mMelody;
	
	int						mPoints;
	float					mPointsMultiplier;
	int						mCreatedAsteroids;
	int						mDestroyedAsteroids;
	bool					mGameOver;
	sf::Text				mCentralText;
	bool					mPaused;
	bool					mStopped;
		
	std::default_random_engine			randomEngine;
	std::uniform_int_distribution<int>	randomasteroidScaleValues;
	std::uniform_int_distribution<int>	randomasteroidPositionValues;
	std::uniform_int_distribution<int>	randomasteroidTextureValues;
};
