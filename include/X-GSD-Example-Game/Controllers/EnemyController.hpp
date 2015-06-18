#pragma once

#include <X-GSD/ControllerHeaders.hpp>

using namespace xgsd;

/*
 Controller for the logic of the enemy asteroids.
 */
class EnemyController : public Component
{
	// Methods
public:
	EnemyController();
	~EnemyController();
	
	void				onEntityAttach() override;
    void                update(const HiResDuration &dt) override;
	void				collisionHandler(Entity *theOtherEntity, sf::FloatRect collision) override;
		
	// Variables (member / properties)
private:
	ComponentSprite*	mSprite;
};
