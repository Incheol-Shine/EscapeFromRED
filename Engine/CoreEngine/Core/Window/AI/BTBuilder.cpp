#include "BTBuilder.h"

// BTBuilder::BTBuilder()
// {
//     SetupTree();
// }

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

// BTBuilder& BTBuilder::addActionNode([]()->NodeStatus{ return action; })
// BT_TEST 주소를 가져야 함
BTBuilder& BTBuilder::addActionNode(std::function<NodeStatus()> action)
{
    auto actionNode = std::make_shared<ActionNode>(action);
    nodeStack.top()->addChild(actionNode);
    return *this;
}

BTBuilder& BTBuilder::endBranch()
{
    if (nodeStack.size() > 1) {
        nodeStack.pop();  // 현재 노드를 완료하고 부모 노드로 복귀
    }
    return *this;
}

Ptr<Node> BTBuilder::build()
{
    while (nodeStack.size() > 1)
    {
        nodeStack.pop();  // 모든 노드가 닫힐 때까지 스택을 비움
    }
    return rootNode;
}


/*
void BTBuilder::SetupTree()
{
    createRoot("Root");

    /*Ptr<ActionNode> findTargetNode = std::make_shared<ActionNode>(&BTBuilder::findTarget);
    Ptr<ActionNode> moveToTargetNode = std::make_shared<ActionNode>(&BTBuilder::moveToTarget);
    Ptr<ActionNode> attackTargetNode = std::make_shared<ActionNode>(&BTBuilder::attackTarget);
    
    seq->addChild(findTargetNode);
    seq->addChild(moveToTargetNode);
    seq->addChild(attackTargetNode);#1#

    Ptr<Sequence> stopSequence = std::make_shared<Sequence>();
    Ptr<ActionNode> isCloseToPlayer = std::make_shared<ActionNode>(
        [this]()->NodeStatus { return IsPlayerClose(); });  // Player와의 거리 확인
    Ptr<ActionNode> stopAction = std::make_shared<ActionNode>(
        [this]()->NodeStatus { return StopChase(); });  // 멈춤 행동
    stopSequence->addChild(isCloseToPlayer);
    stopSequence->addChild(stopAction);

    // 2. 추적 행동 시퀀스
    Ptr<Sequence> chaseSequence = std::make_shared<Sequence>();
    Ptr<ActionNode> isFarFromPlayer = std::make_shared<ActionNode>(
        [this]()->NodeStatus { return Not(IsPlayerClose()); });  // Player와 멀리 있는지 확인
    Ptr<ActionNode> chaseAction = std::make_shared<ActionNode>(
        [this]()->NodeStatus { return ChasePlayer(); });  // 추적 행동
    chaseSequence->addChild(isFarFromPlayer);
    chaseSequence->addChild(chaseAction);

    rootNode->addChild(stopSequence);
    rootNode->addChild(chaseSequence);
}
*/



