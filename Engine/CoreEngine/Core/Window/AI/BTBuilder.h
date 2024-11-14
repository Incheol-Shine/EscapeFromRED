#pragma once
#include "Node.h"

class JActor;

class BTBuilder
{
public:
    // Action Node
    static NodeStatus findTarget();
    static NodeStatus moveToTarget();
    static NodeStatus attackTarget();

    static NodeStatus StopChase();
    static NodeStatus ChasePlayer();

    // Decorator
    // static NodeStatus IsPlayerClose(Ptr<JActor> A, Ptr<JActor> B);
    static NodeStatus IsPlayerClose();
    static NodeStatus IsSuccess();
};
