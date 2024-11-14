#include "BTBuilder.h"

BTBuilder::BTBuilder()
{
}

BTBuilder::~BTBuilder()
{
}

BTBuilder& BTBuilder::createRoot(const JText& Name)
{
    rootNode = std::make_shared<Selector>(Name);
    nodeStack.push(rootNode);
    return *this;
}

BTBuilder& BTBuilder::addSequence(const JText& Name)
{
    auto sequence = std::make_shared<Sequence>(Name);
    nodeStack.top()->addChild(sequence);
    nodeStack.push(sequence);
    return *this;
}

BTBuilder& BTBuilder::addSelector(const JText& Name)
{
    auto selector = std::make_shared<Selector>(Name);
    nodeStack.top()->addChild(selector);
    nodeStack.push(selector);
    return *this;
}

BTBuilder& BTBuilder::addActionNode(std::function<NodeStatus()> action)
{
    auto actionNode = std::make_shared<ActionNode>(action);
    nodeStack.top()->addChild(actionNode);
    return *this;
}

BTBuilder& BTBuilder::endBranch()
{
    if (nodeStack.size() > 1)
    {
        nodeStack.pop(); // 현재 노드를 완료하고 부모 노드로 복귀
    }
    return *this;
}

Ptr<Node> BTBuilder::build()
{
    while (nodeStack.size() > 1)
    {
        nodeStack.pop(); // 모든 노드가 닫힐 때까지 스택을 비움
    }
    return rootNode;
}
