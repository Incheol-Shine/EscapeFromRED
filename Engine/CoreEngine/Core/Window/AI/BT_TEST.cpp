#include "BT_TEST.h"

#include "Core/Interface/MManagerInterface.h"
#include "GUI/GUI_Inspector.h"
#include "Core/Entity/Actor/JActor.h"
#include "Core/Entity/Camera/MCameraManager.h"
#define MAG 0.01f
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

NodeStatus BT_TEST::StopChase2()
{
    BB.flag = false;
    std::cout << "Stop Chase" << std::endl;
    return NodeStatus::Success;
}


NodeStatus BT_TEST::ChasePlayer()
{
    JMath::TVector location = GetOwnerActor()->GetRootComponent()->GetLocalLocation();
    location.x += BB.direction.x * MAG / BB.distance;
    location.y += BB.direction.y * MAG / BB.distance;
    location.z += BB.direction.z * MAG / BB.distance;
    GetOwnerActor()->GetRootComponent()->SetLocalLocation(location);
    
    std::cout << "Chase Player" << std::endl;
    return NodeStatus::Success;
}

NodeStatus BT_TEST::ChasePlayer2()
{
    JMath::TVector location = GetOwnerActor()->GetRootComponent()->GetLocalLocation();
    location.x += BB.direction.x * MAG / BB.distance;
    location.z += BB.direction.z * MAG / BB.distance;
    GetOwnerActor()->GetRootComponent()->SetLocalLocation(location);
    
    std::cout << "Chase Player2" << std::endl;
    return NodeStatus::Success;
}

NodeStatus BT_TEST::ChasePlayer3()
{
    JMath::TVector location = GetOwnerActor()->GetRootComponent()->GetLocalLocation();
    location.x += BB.direction.x * MAG / BB.distance;
    location.y = abs(sin(BB.timer.Elapsed() * 7)) * 3;
    location.z += BB.direction.z * MAG / BB.distance;
    GetOwnerActor()->GetRootComponent()->SetLocalLocation(location);
    
    std::cout << "Chase Player3" << std::endl;
    return NodeStatus::Success;
}

NodeStatus BT_TEST::IsPlayerClose(UINT N)
{
    if (BB.distance <= N)
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

NodeStatus BT_TEST::RandN(UINT N)
{
    if (!BB.flag)
    {
        BB.randIndex = FMath::GenerateNum(0, N - 1);
        BB.flag = true;
    }
    return NodeStatus::Success;
}

NodeStatus BT_TEST::IsRandSame(UINT N)
{
    if (BB.randIndex == N)
        return NodeStatus::Success;
    else
        return NodeStatus::Failure;
}


void BT_TEST::SetupTree()
{
    BTRoot = builder
            .createRoot("RootSelector")
                .addSequence("StopSequence")
                    .addActionNode([this]()-> NodeStatus { return IsPlayerClose(5); })
                    .addActionNode([this]()-> NodeStatus { return StopChase(); })
                .endBranch()
                .addActionNode([this]()-> NodeStatus { return ChasePlayer3(); })
            .build();
}

void BT_TEST::SetupTree2()
{
    BTRoot = builder
            .createRoot("RootSelector")
                .addSequence("StopSequence")
                    .addActionNode([this]()-> NodeStatus { return IsPlayerClose(5); })
                    .addActionNode([this]()-> NodeStatus { return StopChase2(); })
                .endBranch()
                .addSequence("RandSequence")
                    .addActionNode([this]()-> NodeStatus { return RandN(2); })
                    .addSelector("ActSelector")
                        .addSequence("Act1")
                            .addActionNode([this]()-> NodeStatus { return IsRandSame(0); })
                            .addActionNode([this]()-> NodeStatus { return ChasePlayer2(); })
                        .endBranch()
                        .addSequence("Act2")
                            .addActionNode([this]()-> NodeStatus { return IsRandSame(1); })
                            .addActionNode([this]()-> NodeStatus { return ChasePlayer3(); })
                        .endBranch()
                    .endBranch()
                .endBranch()
            .build();
}

