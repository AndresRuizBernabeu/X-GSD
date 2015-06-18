// Modified version of ResourceHolder.hpp from SFML Game Development Book

#pragma once

/*
 ResourcePath.hpp is not provided with X-GSD because its
 implementation is OS-dependent.
 You can easily create your own as an envelop of the OS specific
 function to reach the resource folder.
 You can also use the implementation provided with SFML templates
 for specific platforms-OSs (for example, they provide one for
 Xcode in their downloads section of SFML webpage).
 */
#include "ResourcePath.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>

namespace xgsd {
    
    /* Class template to store and manage resources: Textures, sounds, fonts, etc.
     
     You can create instances of this resource manager, each of
     which will store their own resources and have no relation or
     communication.
     
     Local version instances at scenes ensures that each scene will load / free
     from memory their required resources on creation / destruction
     of the scene (RAII), while the general (Game's instances) resources will last
     until the game ends or manual unload is invoked.
     */
    template <typename Resource, typename Identifier>
    class ResourceManager
    {
    public:
        
        typedef std::unique_ptr<ResourceManager<Resource, Identifier>> Ptr;
        
        void            load(Identifier id, const std::string& filename);
        
        template <typename Parameter>
        void            load(Identifier id, const std::string& filename, const Parameter& secondParam);
        
        void            unload(Identifier id);
        
        Resource&       get(Identifier id);
        const Resource& get(Identifier id) const;
        
    private:
        void            insertResource(Identifier id, std::unique_ptr<Resource> resource);
        
    private:
        std::map<Identifier, std::unique_ptr<Resource>>         mResourceMap;
    };
    
    // Specific resource managers (textures, fonts,  audio...)
    typedef ResourceManager<sf::Texture, std::string>           TextureManager;
    typedef ResourceManager<sf::Font, std::string>              FontManager;
    typedef ResourceManager<sf::SoundBuffer, std::string>       SoundManager;
    
    
    
    /////////////////////////////
    // Template implementation //
    /////////////////////////////
    
    
    ////// LOAD //////
    
    template <typename Resource, typename Identifier>
    void ResourceManager<Resource, Identifier>::load(Identifier id, const std::string& filename)
    {
        // Create and load resource
        std::unique_ptr<Resource> resource(new Resource());
        if (!resource->loadFromFile(resourcePath() + filename))
            throw std::runtime_error("ResourceManager::load - Failed to load " + resourcePath() + filename);
        
        // If loading successful, insert resource to map
        insertResource(id, std::move(resource));
    }
    
    template <typename Resource, typename Identifier>
    template <typename Parameter>
    void ResourceManager<Resource, Identifier>::load(Identifier id, const std::string& filename, const Parameter& secondParam)
    {
        // Create and load resource
        std::unique_ptr<Resource> resource(new Resource());
        if (!resource->loadFromFile(resourcePath() + filename, secondParam))
            throw std::runtime_error("ResourceManager::load - Failed to load " + resourcePath() + filename);
        
        // If loading successful, insert resource to map
        insertResource(id, std::move(resource));
    }
    
    
    ////// UNLOAD //////
    
    /* Use unload for special resource management. For general purpose, see description at the beggining of this file. */
    template <typename Resource, typename Identifier>
    void ResourceManager<Resource, Identifier>::unload(Identifier id)
    {
        auto found = mResourceMap.find(id);
        assert(found != (mResourceMap.end()));
        mResourceMap.erase(found);
    }
    
    
    ////// GET //////
    
    template <typename Resource, typename Identifier>
    Resource& ResourceManager<Resource, Identifier>::get(Identifier id)
    {
        auto found = mResourceMap.find(id);
        assert(found != mResourceMap.end());
        
        return *found->second;
    }
    
    template <typename Resource, typename Identifier>
    const Resource& ResourceManager<Resource, Identifier>::get(Identifier id) const
    {
        auto found = mResourceMap.find(id);
        assert(found != mResourceMap.end());
        
        return *found->second;
    }
    
    ////// INSERT //////
    
    template <typename Resource, typename Identifier>
    void ResourceManager<Resource, Identifier>::insertResource(Identifier id, std::unique_ptr<Resource> resource)
    {
        // Insert and check success
        auto inserted = mResourceMap.insert(std::make_pair(id, std::move(resource)));
        assert(inserted.second);
    }
    
} // namespace xgsd
