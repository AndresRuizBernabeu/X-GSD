#include <X-GSD/Game.hpp>

#include <stdexcept>
#include <iostream>

int main()
{
	// General try-catch for unhandled exceptions in game
	try
	{
		/*
		 Get the instance of the Game and run it.
		 Choose from these run methods according to your requirements.
		 Most of them can be tunned passing some arguments, but you can
		 leave them unspecified to use default values.
		 
		 xgsd::Game::instance().runFixedDeltaTime();
		 xgsd::Game::instance().runVariableDeltaTime();
		 xgsd::Game::instance().runSemiFixedDeltaTime();
		 xgsd::Game::instance().runFixedSimulationVariableFramerate();
		 */
        
        //xgsd::Game::instance().runFixedDeltaTime(30);
        xgsd::Game::instance().runFixedSimulationVariableFramerate(60);
	}
	catch (std::exception& e)
	{
		std::cout << "\nEXCEPTION: " << e.what() << std::endl;
	}
}
