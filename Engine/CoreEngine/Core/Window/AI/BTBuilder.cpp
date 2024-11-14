#include "BTBuilder.h"

NodeStatus BTBuilder::findTarget()
{
    std::cout << "Find target" << std::endl;
    return NodeStatus::Success;
}

NodeStatus BTBuilder::moveToTarget()
{
    std::cout << "Move to target" << std::endl;
    return NodeStatus::Success;
}

NodeStatus BTBuilder::attackTarget()
{
    std::cout << "Attack Target" << std::endl;
    return NodeStatus::Success;
}

NodeStatus BTBuilder::StopChase()
{
    std::cout << "Stop Chase" << std::endl;
    return NodeStatus::Success;
}

NodeStatus BTBuilder::ChasePlayer()
{
    std::cout << "Chase Player" << std::endl;
    return NodeStatus::Success;
}

// NodeStatus BTBuilder::IsPlayerClose(Ptr<JActor> A, Ptr<JActor> B)
NodeStatus BTBuilder::IsPlayerClose()
{
    /*TVector4 direction;
    direction = A - B;
    if (direction.Length() <= boundary)
    {
        std::cout << "Player Found" << std::endl;
        return NodeStatus::Success;
    }
    else
    {
        std::cout << "Search Player" << std::endl;
        return NodeStatus::Failure;
    }*/

    std::cout << "Player Found" << std::endl;
    return NodeStatus::Success;
}

NodeStatus BTBuilder::IsSuccess()
{
    
}

