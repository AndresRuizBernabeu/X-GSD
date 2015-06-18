#pragma once

#include <X-GSD/Component.hpp>

#include <SFML/Graphics/Sprite.hpp>

namespace xgsd {
    
    /*
     ComponentSprite class. Add this component to an entity to give it a sprite (basic visual representation
     of a texture). Specify a texture rectangle (with the overloaded constructor or the setter) in order to
     use a portion of the texture instead of the whole.
     
     Attention: This class is in very basic state and functionality, totally subject to change.
     */
    
    // TODO: Transform this component in something more generic (i.e. ComponentRenderer) which hold a sf::Shape, text, a sf::Sprite... any sf::Drawable derived class.
    class ComponentSprite : public Component
    {
        // Methods
    public:
        ComponentSprite(const sf::Texture& texture);
        ComponentSprite(const sf::Texture& texture, sf::IntRect textureRect);
        ~ComponentSprite();
        
        void                    draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        
        sf::FloatRect           getGlobalBounds();
        sf::Color               getColor();
        const sf::Texture&      getTexture();
        sf::IntRect             getTextureRect();
        
        void                    setColor(sf::Color color);
        void                    setTexture(sf::Texture& texture);
        void                    setTextureRect(sf::IntRect textureRect);
        
        
        // Variables (member / properties)
    private:
        sf::Sprite              mSprite;
    };
    
} // namespace xgsd
