#include <X-GSD/Component.hpp>

using namespace xgsd;

Component::Component()
: entity()
{
    // Load resources here (RAII)
}


void Component::attachedToEntity()
{
    // Perform any task needed when the component is attached to a specific entity. Override on derived classes if needed. Does nothing by default
}

void Component::detachedFromEntity()
{
    // Perform any task needed when the component is detached from a specific entity. Override on derived classes if needed. Does nothing by default
}


void Component::onEntityAttach()
{
    // Perform any task when the entity holding this component is attached to another entity. Initialization of this component may be here (at this point the scene graph is already built). Override this method on derived classes if needed. Does nothing by default
}

void Component::onEntityDetach()
{
    // Perform any task when the entity holding this component is detached to another entity. Override this method on derived classes if needed. Does nothing by default
}

void Component::update(const HiResDuration& dt)
{
    // Update logic/physics here. Override this method on derived classes if needed. Does nothing by default
}


void Component::onPause(const HiResDuration &dt)
{
    // Perform tasks here while the game is paused. Override this method on derived classes if needed. Does nothing by default
}


void Component::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Render here. Override this method on derived classes if needed. Does nothing by default
}


void Component::handleEvent(const Event& event)
{
    // Handle events here. Override this method on derived classes if needed. Does nothing by default
}


void Component::collisionHandler(xgsd::Entity *theOtherEntity, sf::FloatRect collision)
{
    // Handle collisions here. Override this method on derived classes if needed. Does nothing by default
}


Component::~Component()
{
    // Cleanup. Implement a custom destructor on derived classes if needed.
}
