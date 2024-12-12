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
    
    Ptr<Node> rootNode;
    std::stack<Ptr<Node>> nodeStack;

public:
    template <typename T>
    BTBuilder& createRoot();
    BTBuilder& addSequence(const JText& Name);
    BTBuilder& addSelector(const JText& Name);
    BTBuilder& addDecorator(std::function<NodeStatus()> condition);
    BTBuilder& addActionNode(std::function<NodeStatus()> action);
    BTBuilder& endBranch();
    Ptr<Node> build();
};
