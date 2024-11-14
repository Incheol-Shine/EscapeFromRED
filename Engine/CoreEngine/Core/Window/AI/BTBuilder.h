#pragma once
#include <stack>
#include "Node.h"

class JActor;
class BT_TEST;

class BTBuilder
{
public:
    BTBuilder();
    ~BTBuilder();

    Ptr<Selector> rootNode;
    std::stack<Ptr<Node>> nodeStack;

public:
    BTBuilder& createRoot(const JText& Name);
    BTBuilder& addSequence(const JText& Name);
    BTBuilder& addSelector(const JText& Name);
    BTBuilder& addActionNode(std::function<NodeStatus()> action);
    BTBuilder& endBranch();
    Ptr<Node> build();
};
