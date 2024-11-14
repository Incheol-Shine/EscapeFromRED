#include "BT_TEST.h"

#include "Core/Interface/MManagerInterface.h"
#include "GUI/GUI_Inspector.h"
#include "Core/Entity/Actor/JActor.h"
// #include "Core/Window/AI/ActionNode.h"

BT_TEST::BT_TEST()
{
}

BT_TEST::BT_TEST(JTextView InName)
    : JActorComponent(InName)
{
}

BT_TEST::~BT_TEST()
{
}

void BT_TEST::Initialize()
{
    JActorComponent::Initialize();
}

void BT_TEST::BeginPlay()
{
    JActorComponent::BeginPlay();
}

void BT_TEST::Tick(float DeltaTime)
{
    JActorComponent::Tick(DeltaTime);
    // root.tick();
    
    /*FVector location = GetOwnerActor()->GetRootComponent()->GetLocalLocation();
    location.z += -0.005f;
    GetOwnerActor()->GetRootComponent()->SetLocalLocation(location);*/
    
}

void BT_TEST::Destroy()
{
    JActorComponent::Destroy();
}

void BT_TEST::SetupTree()
{
    Ptr<Sequence> seq = std::make_shared<Sequence>(sequence);
    root.addChild(seq);
    
    /*Ptr<ActionNode> findTargetNode = std::make_shared<ActionNode>(&BTBuilder::findTarget);
    Ptr<ActionNode> moveToTargetNode = std::make_shared<ActionNode>(&BTBuilder::moveToTarget);
    Ptr<ActionNode> attackTargetNode = std::make_shared<ActionNode>(&BTBuilder::attackTarget);
    
    seq->addChild(findTargetNode);
    seq->addChild(moveToTargetNode);
    seq->addChild(attackTargetNode);*/

    Ptr<Sequence> stopSequence = std::make_shared<Sequence>();
    Ptr<ActionNode> isCloseToPlayer = std::make_shared<ActionNode>(
        [this]() { return BTBuilder::IsPlayerClose(); });  // Player와의 거리 확인
    Ptr<ActionNode> stopAction = std::make_shared<ActionNode>(
        [this]() { return BTBuilder::StopChase(); });  // 멈춤 행동
    stopSequence->addChild(isCloseToPlayer);
    stopSequence->addChild(stopAction);

    // 2. 추적 행동 시퀀스
    Ptr<Sequence> chaseSequence = std::make_shared<Sequence>();
    Ptr<ActionNode> isFarFromPlayer = std::make_shared<ActionNode>(
        [this]() { return BTBuilder::IsPlayerClose() == NodeStatus::Failure; });  // Player와 멀리 있는지 확인
    Ptr<ActionNode> chaseAction = std::make_shared<ActionNode>(
        [this]() { return ChasePlayer(); });  // 추적 행동
    chaseSequence->addChild(isFarFromPlayer);
    chaseSequence->addChild(chaseAction);

    

    while (root.tick() == NodeStatus::Running)
    {
        std::cout << "Behavior tree running" << std::endl;
    }
    
    std::cout << "Behavior tree stopped" << std::endl;
}
