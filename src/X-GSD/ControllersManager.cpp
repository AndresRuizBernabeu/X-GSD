#include <X-GSD/ControllersManager.hpp>

using namespace xgsd;

Component::Ptr ControllersManager::getController(std::string controllerName)
{
    auto found = mControllerFactories.find(controllerName);
    if(found == mControllerFactories.end())
        throw std::runtime_error("Attempt to load an unknown controller: " + controllerName);
    
    return std::move(found->second()); // Call the needed functor to constructor
}
