#include "EnemyController.hpp"
#include <string>

EnemyController::EnemyController()
{
	// Load resources here (RAII)
}

void EnemyController::onEntityAttach()
{
	// Broadcast a "AsteroidCreated" event
	Event::CustomEvent AsteroidCreated("AsteroidCreated");
	Game::instance().broadcastEvent(AsteroidCreated);
	
	// Get a reference to the component sprite
	mSprite = entity->getComponent<ComponentSprite>();
}

void EnemyController::update(const HiResDuration &dt)
{
    entity->mTransformable.rotate(50*dt.count()/(float)ONE_SECOND.count());
}


void EnemyController::collisionHandler(Entity *theOtherEntity, sf::FloatRect collision)
{
	
	// Get destroyed
    if (theOtherEntity->getName().find("bullet") != std::string::npos) {
		
		theOtherEntity->requestDestroy(); // Destroy the bullet
		
		// Destroy the Asteroid
		entity->requestDestroy();
	}
	
}

EnemyController::~EnemyController()
{
	// Cleanup
	
	// Broadcast a "AsteroidDestroyed" event
	Event::CustomEvent AsteroidDestroyed("AsteroidDestroyed");
	Game::instance().broadcastEvent(AsteroidDestroyed);
}