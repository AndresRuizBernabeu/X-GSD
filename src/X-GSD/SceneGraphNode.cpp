// Modified version of SceneNode.cpp from SFML Game Development Book

#include <X-GSD/SceneGraphNode.hpp>

using namespace xgsd;

SceneGraphNode::SceneGraphNode()
: mChildren()
, mParent(nullptr)
, mTransformable()
, mPendingDetachments()
, mPendingDestruction(false)
{
    // Load resources here (RAII)
}

void SceneGraphNode::requestAttach(Ptr child)
{
    mPendingAttachments.push_back(std::move(child));
}

void SceneGraphNode::requestDetach(SceneGraphNode* child)
{
    mPendingDetachments.push_back(child);
}

void SceneGraphNode::requestDestroy()
{
    mPendingDestruction = true;
}

void SceneGraphNode::performPendingSceneGraphOperations()
{
    // Perform delayed node attachment/detachment/destruction, when it is safe
    
    for (auto childIter = mChildren.begin(); childIter != mChildren.end(); ++childIter) {
        (*childIter)->performPendingSceneGraphOperations();
    }
    
    for (auto nodeIter = mPendingAttachments.begin(); nodeIter != mPendingAttachments.end(); ++nodeIter) {
        attachChild(std::move(*nodeIter));
    }
    mPendingAttachments.clear();
    
    for (auto nodeIter = mPendingDetachments.begin(); nodeIter != mPendingDetachments.end(); ++nodeIter) {
        detachChild(**nodeIter);
    }
    mPendingDetachments.clear();
    
    if (mPendingDestruction) {
        destroy();
    }
}

void SceneGraphNode::attachChild(Ptr child)
{
    child->mParent = this;
    child->onAttach();
    mChildren.push_back(std::move(child));
}

SceneGraphNode::Ptr SceneGraphNode::detachChild(SceneGraphNode& child)
{
    auto found = std::find_if(mChildren.begin(), mChildren.end(), [&] (Ptr& p) { return p.get() == &child; });
    assert(found != mChildren.end());
    
    child.onDetach();
    
    Ptr result = std::move(*found);
    result->mParent = nullptr;
    mChildren.erase(found);
    return result;
}

void SceneGraphNode::destroy()
{
    assert(mParent && "Attempted to destroy the root scene graph node, or a node which has not been attached as a child of another yet!");
    this->mChildren.clear();
    mParent->requestDetach(this);
    this->mParent = nullptr;
}

void SceneGraphNode::onAttach()
{
    onAttachThis();
    onAttachChildren();
}

void SceneGraphNode::onAttachThis()
{
    // Do nothing by default. Override this on a derived class or a custom Component controller
}

void SceneGraphNode::onAttachChildren()
{
    for(Ptr& child : mChildren)
        child->onAttach();
}

void SceneGraphNode::onDetach()
{
    onDetachThis();
    onDetachChildren();
}

void SceneGraphNode::onDetachThis()
{
    // Do nothing by default. Override this on a derived class or a custom Component controller
}

void SceneGraphNode::onDetachChildren()
{
    for(Ptr& child : mChildren)
        child->onAttach();
}


void SceneGraphNode::onPause(const HiResDuration &dt)
{
    onPauseThis(dt);
    onPauseChildren(dt);
}

void SceneGraphNode::onPauseThis(const HiResDuration &dt)
{
    // Do nothing by default. Override this on a derived class or a custom Component controller
}

void SceneGraphNode::onPauseChildren(const HiResDuration &dt)
{
    for(const Ptr& child : mChildren)
        child->onPauseThis(dt);
}

void SceneGraphNode::update(const HiResDuration& dt)
{
    updateThis(dt);
    updateChildren(dt);
}

void SceneGraphNode::updateThis(const HiResDuration& dt)
{
    // Do nothing by default. Override this on a derived class or a custom Component controller
}

void SceneGraphNode::updateChildren(const HiResDuration& dt)
{
    for(Ptr& child : mChildren)
        child->update(dt);
}

void SceneGraphNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Apply transform of current node
    states.transform *= mTransformable.getTransform();
    
    // Draw node and children with changed transform
    drawThis(target, states);
    drawChildren(target, states);
}

void SceneGraphNode::drawThis(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Do nothing by default. Override this on a derived class or a custom Component controller
}

void SceneGraphNode::drawChildren(sf::RenderTarget& target, sf::RenderStates states) const
{
    for(const Ptr& child : mChildren)
        child->draw(target, states);
}

void SceneGraphNode::handleEvent(const Event &event)
{
    handleEventThis(event);
    handleEventChildren(event);
}

void SceneGraphNode::handleEventThis(const Event &event)
{
    // Do nothing by default. Override this on a derived class or a custom Component controller
}

void SceneGraphNode::handleEventChildren(const Event &event)
{
    for(const Ptr& child : mChildren)
        child->handleEvent(event);
}

sf::Vector2f SceneGraphNode::getWorldPosition() const
{
    return getWorldTransform() * sf::Vector2f();
}

sf::Transform SceneGraphNode::getWorldTransform() const
{
    sf::Transform worldTransform = sf::Transform::Identity;

    for (const SceneGraphNode* node = this; node != nullptr; node = node->mParent)
        worldTransform = node->mTransformable.getTransform() * worldTransform;
    
    return worldTransform;
}

SceneGraphNode& SceneGraphNode::getParent()
{
    return *mParent;
}

bool SceneGraphNode::isDestroyPending()
{
    return mPendingDestruction;
}

SceneGraphNode::~SceneGraphNode()
{
    // Cleanup
}
