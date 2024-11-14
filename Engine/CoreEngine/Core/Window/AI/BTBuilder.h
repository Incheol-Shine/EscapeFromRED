#pragma once
#include <stack>
#include "Node.h"

class JActor;
class BT_TEST;

class BTBuilder
{
public:
    // BTBuilder();
    // ~BTBuilder();
    
    Ptr<Selector> rootNode;
    std::stack<Ptr<Node>> nodeStack;
    
public:
    // void tick();
    // void SetupTree();
    // Root Node
    BTBuilder& createRoot(const JText& Name);
    BTBuilder& addSequence(const JText& Name);
    BTBuilder& addSelector(const JText& Name);
    // BTBuilder& addActionNode(std::function<NodeStatus()> action);
    BTBuilder& addActionNode(std::function<NodeStatus()> action);
    BTBuilder& endBranch();
    Ptr<Node> build();
    
    // // Action Function
    // NodeStatus findTarget();
    // NodeStatus moveToTarget();
    // NodeStatus attackTarget();
    //
    // NodeStatus StopChase();
    // NodeStatus ChasePlayer();
    //
    // // Decorator Function
    // // NodeStatus IsPlayerClose(Ptr<JActor> A, Ptr<JActor> B);
    // NodeStatus IsPlayerClose();
    // NodeStatus Not(NodeStatus state);
    
};
