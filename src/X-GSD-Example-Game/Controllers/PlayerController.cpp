#include "PlayerController.hpp"

PlayerController::PlayerController()
: mVelocity(200)
, mNumShots(0)
{
    // Load resources here (RAII)
}

void PlayerController::onEntityAttach()
{
    // Get references to components that will be used frequently
    rigidBody = entity->getComponent<ComponentRigidBody>();
    assert(rigidBody);
    sprite = entity->getComponent<ComponentSprite>();
    assert(sprite);
    
    // No gravity
    rigidBody->getPhysicsState().setForce(sf::Vector2f());

    // Get the screen bounds
    mBounds = sf::Rect<float>(0, 0, Game::instance().getWindow().getSize().x, Game::instance().getWindow().getSize().y); ;
    
    // Set sounds
    mShootingSound.setBuffer(Game::instance().getLocalSoundManager().get("bulletSound"));
}

void PlayerController::update(const HiResDuration& dt)
{
    // Update logic/physics here
    handleRealTimeInput(dt);
}

void PlayerController::handleEvent(const Event& event)
{
    // Handle system events
    if (event.type == Event::System) {
        
        if (event.systemEvent.key.code  == sf::Keyboard::Key::Space)
            shoot();
    }
    
    // Handle custom events
    if (event.type == Event::Custom) {
        
        
    }
}

void PlayerController::collisionHandler(Entity *theOtherEntity, sf::FloatRect collision)
{
    // Get destroyed
    if (theOtherEntity->getName().find("asteroid") != std::string::npos) {
        
        theOtherEntity->requestDestroy(); // Destroy the asteroid
        
        // Destroy the player
        entity->requestDestroy();
    }
    
}

void PlayerController::handleRealTimeInput(const HiResDuration &dt)
{
    // Basic Keyboard controls
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        rigidBody->getPhysicsState().setVelocity(sf::Vector2f(-mVelocity, rigidBody->getPhysicsState().getVelocity().y));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        rigidBody->getPhysicsState().setVelocity(sf::Vector2f(mVelocity, rigidBody->getPhysicsState().getVelocity().y));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        rigidBody->getPhysicsState().setVelocity(sf::Vector2f(rigidBody->getPhysicsState().getVelocity().x, -mVelocity));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        rigidBody->getPhysicsState().setVelocity(sf::Vector2f(rigidBody->getPhysicsState().getVelocity().x, mVelocity));
    }
    
    // Reduce velocity gradually
    rigidBody->getPhysicsState().setVelocity(sf::Vector2f(rigidBody->getPhysicsState().getVelocity().x * 0.9f, rigidBody->getPhysicsState().getVelocity().y * 0.9));
}

void PlayerController::shoot()
{
    // Play a sound
    mShootingSound.play();
    
    // Create the entity
    Entity::Ptr bulletEntity(new Entity("bullet_" + std::to_string(mNumShots)));
    
    // Set its position to the player's
    bulletEntity->mTransformable.setPosition(entity->mTransformable.getPosition());
    bulletEntity->mTransformable.move(32-8, 0); // Center it so that the bullet exits from the center of the player
    
    // Create the required components
    ComponentSprite::Ptr bulletSprite(new ComponentSprite(Game::instance().getLocalTextureManager().get("bulletTexture")));
    
    ComponentRigidBody::Ptr bulletRigidBody(new ComponentRigidBody(false));
    
    ComponentCollider::Ptr bulletCollider(new ComponentCollider());
    
    // Add the components
    bulletEntity->addComponent(std::move(bulletSprite));
    bulletEntity->addComponent(std::move(bulletRigidBody));
    bulletEntity->addComponent(std::move(bulletCollider));
    
    bulletEntity->getComponent<ComponentRigidBody>()->getPhysicsState().setForce(sf::Vector2f());
    bulletEntity->getComponent<ComponentRigidBody>()->getPhysicsState().setVelocity(sf::Vector2f(0, -300));
    
    // And finally, add it to the scene
    Game::instance().getSceneManager().addNode(std::move(bulletEntity));
    
    mNumShots++;
}

PlayerController::~PlayerController()
{
    // Cleanup
    // Broadcast a "PlayerDestroyed" event
    Event::CustomEvent PlayerDestroyed("PlayerDestroyed");
    Game::instance().broadcastEvent(PlayerDestroyed);
}
