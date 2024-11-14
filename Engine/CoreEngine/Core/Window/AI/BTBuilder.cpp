#include "BTBuilder.h"

// BTBuilder::BTBuilder()
// {
//     SetupTree();
// }

void BTBuilder::createRoot(const JText& rootName)
{
    rootNode = std::make_shared<Selector>(rootName);
}

void BTBuilder::tick()
{
    rootNode->tick();
}

void BTBuilder::SetupTree()
{
    createRoot("Root");

    /*Ptr<ActionNode> findTargetNode = std::make_shared<ActionNode>(&BTBuilder::findTarget);
    Ptr<ActionNode> moveToTargetNode = std::make_shared<ActionNode>(&BTBuilder::moveToTarget);
    Ptr<ActionNode> attackTargetNode = std::make_shared<ActionNode>(&BTBuilder::attackTarget);
    
    seq->addChild(findTargetNode);
    seq->addChild(moveToTargetNode);
    seq->addChild(attackTargetNode);*/

    Ptr<Sequence> stopSequence = std::make_shared<Sequence>();
    Ptr<ActionNode> isCloseToPlayer = std::make_shared<ActionNode>(
        [this]() { return IsPlayerClose(); });  // Player와의 거리 확인
    Ptr<ActionNode> stopAction = std::make_shared<ActionNode>(
        [this]() { return StopChase(); });  // 멈춤 행동
    stopSequence->addChild(isCloseToPlayer);
    stopSequence->addChild(stopAction);

    // 2. 추적 행동 시퀀스
    Ptr<Sequence> chaseSequence = std::make_shared<Sequence>();
    Ptr<ActionNode> isFarFromPlayer = std::make_shared<ActionNode>(
        [this]() { return Not(IsPlayerClose()); });  // Player와 멀리 있는지 확인
    Ptr<ActionNode> chaseAction = std::make_shared<ActionNode>(
        [this]() { return ChasePlayer(); });  // 추적 행동
    chaseSequence->addChild(isFarFromPlayer);
    chaseSequence->addChild(chaseAction);

    rootNode->addChild(stopSequence);
    rootNode->addChild(chaseSequence);
}

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
    return NodeStatus::Failure;
}

NodeStatus BTBuilder::Not(NodeStatus state)
{
    if (state == NodeStatus::Success)
        return NodeStatus::Failure;
    else if (state == NodeStatus::Failure)
        return NodeStatus::Success;
    else
        return state;
}


