#include "BT_TEST.h"

#include "Core/Interface/MManagerInterface.h"
#include "GUI/GUI_Inspector.h"
#include "Core/Entity/Actor/JActor.h"
#include "Core/Utils/Math/Vector4.h"

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

void BT_TEST::Destroy()
{
    JActorComponent::Destroy();
}

void BT_TEST::Tick(float DeltaTime)
{
    JActorComponent::Tick(DeltaTime);
    BTRoot->tick();
}

// Action Function
NodeStatus BT_TEST::StopChase()
{
    // FVector location = GetOwnerActor()->GetRootComponent()->GetLocalLocation();
    // location.z += -0.005f;
    GetOwnerActor()->GetRootComponent()->SetLocalLocation(JMath::TVector::ZeroVector);
    
    std::cout << "Stop Chase" << std::endl;
    return NodeStatus::Success;
}

NodeStatus BT_TEST::ChasePlayer()
{
    JMath::TVector location = GetOwnerActor()->GetRootComponent()->GetLocalLocation();
    location.z += -0.005f;
    GetOwnerActor()->GetRootComponent()->SetLocalLocation(location);
    
    std::cout << "Chase Player" << std::endl;
    return NodeStatus::Success;
}

// NodeStatus BT_TEST::IsPlayerClose(Ptr<JActor> A, Ptr<JActor> B)
NodeStatus BT_TEST::IsPlayerClose()
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

NodeStatus BT_TEST::Not(NodeStatus state)
{
    if (state == NodeStatus::Success)
        return NodeStatus::Failure;
    else if (state == NodeStatus::Failure)
        return NodeStatus::Success;
    else
        return state;
}


void BT_TEST::SetupTree()
{
    BTRoot = builder
             .createRoot("RootSelector")
                .addSequence("StopSequence")
                    .addActionNode([this]()-> NodeStatus { return IsPlayerClose(); })
                    .addActionNode([this]()-> NodeStatus { return StopChase(); })
                .endBranch()
                .addSequence("ChaseSequence")
                    .addActionNode([this]()-> NodeStatus { return Not(IsPlayerClose()); })
                    .addActionNode([this]()-> NodeStatus { return ChasePlayer(); })
                .endBranch()
             .build();
}
