#include "Node.h"

Node::Node()
{}

Node::Node(JText InNodeName)
    : NodeName(std::move(InNodeName))
{}

Node::Node(Text InNodeName, const std::vector<NodeSPtr>& InChildNodes)
    : NodeName(std::move(InNodeName))
{
    for (NodeSPtr node : InChildNodes)
    {
        Attach(node);
    }
}

Node::~Node()
{}

ENodeState Node::Evaluate()
{
    return ENodeState::Failure;
}

void Node::SetNodes(const std::vector<NodeSPtr>& InChildNodes)
{
    for (NodeSPtr node : InChildNodes)
    {
        Attach(node);
    }
}

void Node::Attach(NodeSPtr InNode)
{
    InNode->ParentNode = weak_from_this();
    ChildNodes.emplace_back(InNode);
}

void Node::SetData(const Text& InName, UObject* Value)
{
    if (!DataContext.contains(InName))
    {
        DataContext[InName] = Value;
    }
    else
    {
        LOG_CORE_WARN("�̹� �����ϴ� Ű���� ���� �õ�");
    }
}

UObject* Node::GetData(const Text& InKey)
{
    if (DataContext.contains(InKey))
    {
        return DataContext[InKey];
    }

    // NodeSPtr node = ParentNode;

    // while (node != nullptr)
    // {
    // 	if (UObject* value = node->GetData(InKey); value != nullptr)
    // 		return value;
    // 	node = node->ParentNode;
    // }

    return nullptr;
}

bool Node::ClearData(const Text& InKey)
{
    if (DataContext.contains(InKey))
    {
        DataContext.erase(InKey);
        return true;
    }

    // NodeSPtr node = ParentNode;
    // while (node != nullptr)
    // {
    // 	if (bool cleared = node->ClearData(InKey))
    // 		return true;
    // 	node = node->ParentNode;
    // }

    return false;
}

NodeSPtr Node::GetRoot()
{
    NodeSPtr node = shared_from_this();
    while (!node->ParentNode.expired())
    {
        node = node->ParentNode.lock();
    }
    return node;
}
