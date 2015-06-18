#pragma once

#include <X-GSD/ControllerHeaders.hpp>

using namespace xgsd;

/*
 Controller for the logic of the TitleMenuScene.
 */
class TitleMenuController : public Component
{
	// Typedefs and enumerations
private:

	
	// Methods
public:
	TitleMenuController();
	~TitleMenuController();
	
	void					onEntityAttach() override;
	void					update(const HiResDuration& dt) override;
	void					draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void					handleEvent(const Event& event) override;
	
	// Variables (member / properties)
private:
	sf::Text				mTextPressAnyKey;
	sf::RectangleShape		mBackground;
	
	sf::Sound				mMenuSound;
	
	HiResDuration			mPressAnyKeyTime;
};
