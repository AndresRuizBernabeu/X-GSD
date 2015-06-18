#include <X-GSD/ComponentSprite.hpp>

using namespace xgsd;

ComponentSprite::ComponentSprite(const sf::Texture& texture)
: mSprite(texture)
{
    // Load resources here (RAII)
    
    // If this constructor is used and no texture rect is specified it will be as big as the texture    
}

ComponentSprite::ComponentSprite(const sf::Texture& texture, sf::IntRect textureRect)
: mSprite(texture)
{
    // Load resources here (RAII)
    
    mSprite.setTextureRect(textureRect);
}

void ComponentSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Just draw the sprite on the render target
    target.draw(mSprite, states);
}

void ComponentSprite::setColor(sf::Color color)
{
    mSprite.setColor(color);
}

void ComponentSprite::setTexture(sf::Texture &texture)
{
    mSprite.setTexture(texture);
}

void ComponentSprite::setTextureRect(sf::IntRect textureRect)
{
    mSprite.setTextureRect(textureRect);
}

sf::FloatRect ComponentSprite::getGlobalBounds()
{
    return mSprite.getGlobalBounds();
}

sf::Color ComponentSprite::getColor()
{
    return mSprite.getColor();
}

const sf::Texture& ComponentSprite::getTexture()
{
    return *mSprite.getTexture();
}

sf::IntRect ComponentSprite::getTextureRect()
{
    return mSprite.getTextureRect();
}

ComponentSprite::~ComponentSprite()
{
    // Cleanup
}
