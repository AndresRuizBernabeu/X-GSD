#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include <cassert>

namespace xgsd {
    
    // Needed struct for entities updated with the Integrator class.
    class PhysicState
    {
        // Methods
    public:
        PhysicState();// Constructor
        
        void                    setVelocity(sf::Vector2f velocity);
        void                    setForce(sf::Vector2f force);
        void                    setMass(float mass);
        
        sf::Vector2f            getVelocity() const;
        sf::Vector2f&           getVelocityRef();
        sf::Vector2f            getForce() const;
        float                   getMass() const;
        
        // Variables (member / properties)
    private:
        sf::Vector2f            mVelocity;
        sf::Vector2f            mForce;
        float                   mMass;
    };
    
} // namespace xgsd
