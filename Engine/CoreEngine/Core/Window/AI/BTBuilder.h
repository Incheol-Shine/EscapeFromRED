#pragma once
#include "Node.h"

class JActor;

class BTBuilder
{
public:
    // BTBuilder();
    // ~BTBuilder();
    
    Ptr<Selector> rootNode;
    
public:
    // Root Node
    void createRoot(const JText& rootName);
    void tick();
    void SetupTree();
    
    // Action Function
    NodeStatus findTarget();
    NodeStatus moveToTarget();
    NodeStatus attackTarget();

    NodeStatus StopChase();
    NodeStatus ChasePlayer();

    // Decorator Function
    // NodeStatus IsPlayerClose(Ptr<JActor> A, Ptr<JActor> B);
    NodeStatus IsPlayerClose();
    NodeStatus Not(NodeStatus state);
    
};
