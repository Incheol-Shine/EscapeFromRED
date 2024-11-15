#include "BT_TEST.h"

#include "Core/Interface/MManagerInterface.h"
#include "GUI/GUI_Inspector.h"
#include "Core/Entity/Actor/JActor.h"
#include "Core/Entity/Camera/MCameraManager.h"

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
    BBTick();
    BTRoot->tick();
}

void BT_TEST::BBTick()
{
    Ptr<JCamera> cam = IManager.CameraManager->GetCurrentMainCam();
    XMMATRIX Mat = cam->GetWorldMatrix();
    XMFLOAT4X4 fMat;
    XMStoreFloat4x4(&fMat, Mat);
    FVector camLocation = FVector(fMat._41, fMat._42, fMat._43);

    FVector myLocation = GetOwnerActor()->GetRootComponent()->GetLocalLocation();
    BB.direction = camLocation - myLocation;
    BB.distance = BB.direction.Length();
    // std::cout << "cam: " << camLocation.z << std::endl;
    // std::cout << "my: " << myLocation.z << std::endl;
}

// Action Function
NodeStatus BT_TEST::StopChase()
{
    std::cout << "Stop Chase" << std::endl;
    return NodeStatus::Success;
}

NodeStatus BT_TEST::ChasePlayer()
{
    JMath::TVector location = GetOwnerActor()->GetRootComponent()->GetLocalLocation();
    float mag = 0.1f;
    location.x += BB.direction.x * mag / BB.distance;
    location.y += BB.direction.y * mag / BB.distance;
    location.z += BB.direction.z * mag / BB.distance;
    GetOwnerActor()->GetRootComponent()->SetLocalLocation(location);
    
    std::cout << "Chase Player" << std::endl;
    return NodeStatus::Success;
}

NodeStatus BT_TEST::ChasePlayer2()
{
    JMath::TVector location = GetOwnerActor()->GetRootComponent()->GetLocalLocation();
    float mag = 0.1f;
    location.x += BB.direction.x * mag / BB.distance;
    location.z += BB.direction.z * mag / BB.distance;
    GetOwnerActor()->GetRootComponent()->SetLocalLocation(location);
    
    std::cout << "Chase Player2" << std::endl;
    return NodeStatus::Success;
}

NodeStatus BT_TEST::ChasePlayer3()
{
    JMath::TVector location = GetOwnerActor()->GetRootComponent()->GetLocalLocation();
    float mag = 0.1f;
    location.x += BB.direction.x * mag / BB.distance;
    location.y = abs(sin(BB.timer.Elapsed() * 10)) * 3;
    location.z += BB.direction.z * mag / BB.distance;
    GetOwnerActor()->GetRootComponent()->SetLocalLocation(location);
    
    std::cout << "Chase Player3" << std::endl;
    return NodeStatus::Success;
}

NodeStatus BT_TEST::IsPlayerClose()
{
    if (BB.distance <= 5)
        return NodeStatus::Success;
    else
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
                    .addActionNode([this]()-> NodeStatus { return ChasePlayer3(); })
                .endBranch()
             .build();
}

void BT_TEST::SetupTree2()
{
    BTRoot = builder
             .createRoot("RootSelector")
                .addSequence("StopSequence")
                    .addActionNode([this]()-> NodeStatus { return IsPlayerClose(); })
                    .addActionNode([this]()-> NodeStatus { return StopChase(); })
                .endBranch()
                .addSequence("ChaseSequence")
                    .addActionNode([this]()-> NodeStatus { return Not(IsPlayerClose()); })
                    .addActionNode([this]()-> NodeStatus { return ChasePlayer3(); })
                .endBranch()
             .build();
}

