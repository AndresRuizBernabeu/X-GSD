#pragma once

#include <X-GSD/ControllerHeaders.hpp>

using namespace xgsd;

/*
 Controller for the logic of the player.
 */
class PlayerController : public Component
{
	// Methods
public:
	PlayerController();
	~PlayerController();
	
	void				onEntityAttach() override;
	void				update(const HiResDuration& dt) override;
	void				handleEvent(const Event& event) override;
	void				collisionHandler(Entity* theOtherEntity, sf::FloatRect collision) override;
		
private:
	void				handleRealTimeInput(const HiResDuration& dt);
	void				shoot();
	
	
	
	// Variables (member / properties)
private:
    float               mVelocity;
	sf::Sound			mShootingSound;
	sf::Sound			mDieSound;
    int                 mNumShots;
    
	ComponentRigidBody* rigidBody;
	ComponentSprite*	sprite;
    
    sf::Rect<float>        mBounds;
};
