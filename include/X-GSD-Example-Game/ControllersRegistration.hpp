#pragma once

#include  <X-GSD/ControllersManager.hpp>

// Include all of your controllers
#include "TitleController.hpp"
#include "PlayerController.hpp"
#include "EnemyController.hpp"
#include "GameController.hpp"

class ControllersRegistration {
	
public:
	void registerControllers(xgsd::ControllersManager& controllersManager) {
		
		// Register your controllers this way
		controllersManager.registerController<TitleMenuController>("TitleController");
		controllersManager.registerController<PlayerController>("PlayerController");
		controllersManager.registerController<EnemyController>("EnemyController");
		controllersManager.registerController<GameController>("GameController");
	}
	
};
