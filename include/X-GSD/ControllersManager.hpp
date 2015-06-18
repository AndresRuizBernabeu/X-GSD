#pragma once

#include <X-GSD/Component.hpp>

#include <unordered_map>
#include <stdexcept>

namespace xgsd {
    
    /*
     ControllersManager class. This utility class contains methods to register and create
     ControllerComponents in a dynamic manner. Use registerController method to store a pointer to the
     constructor of that type of controller associated with a name (std::string). Then, use
     getController(std::string controllerName) to create controllers of the desired type.
     */
    class ControllersManager
    {
        // Methods
    public:
        template <typename T>
        void            registerController(std::string controllerName);
        
        Component::Ptr  getController(std::string controllerName);
        
        // Variables (member / properties)
    private:
        std::unordered_map<std::string, std::function<Component::Ptr()>> mControllerFactories;
    };
    
    
    // Template implementation
    template <typename T>
    void ControllersManager::registerController(std::string controllerName)
    {
        mControllerFactories[controllerName] = [] ()
        {
            return std::move(Component::Ptr(new T()));
        };
    }
    
} // namespace xgsd
