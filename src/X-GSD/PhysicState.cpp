#include <X-GSD/PhysicState.hpp>

using namespace xgsd;

// Constructor
PhysicState::PhysicState()
: mVelocity()
, mForce()
, mMass(1.f)
{
    
}

void PhysicState::setVelocity(sf::Vector2f velocity)
{
    mVelocity = velocity;
}

sf::Vector2f PhysicState::getVelocity() const
{
    return mVelocity;
}

sf::Vector2f& PhysicState::getVelocityRef()
{
    return mVelocity;
}

void PhysicState::setForce(sf::Vector2f force)
{
    mForce = force;
}

sf::Vector2f PhysicState::getForce() const
{
    return mForce;
}

void PhysicState::setMass(float mass)
{
    assert(mass > 0);
    mMass = mass;
}

float PhysicState::getMass() const
{
    return mMass;
}
