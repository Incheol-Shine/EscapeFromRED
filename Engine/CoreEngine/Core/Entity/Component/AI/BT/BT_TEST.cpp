// #include "BT_TEST.h"
//
// #include "GUI/GUI_Inspector.h"
// #include "Core/Entity/Actor/AActor.h"
// #include "Core/Entity/Camera/MCameraManager.h"
// #include "Core/Interface/JWorld.h"
// #define MAG 0.01f
// #define LAMBDA(func, ...) [this]() -> NodeStatus { return func(__VA_ARGS__); }
//
// BT_TEST::BT_TEST()
// {
// }
//
// BT_TEST::BT_TEST(JTextView InName)
//     : JActorComponent(InName)
// {
// }
//
// BT_TEST::~BT_TEST()
// {
// }
//
// void BT_TEST::Initialize()
// { 
//     JActorComponent::Initialize();
// }
//
// void BT_TEST::BeginPlay()
// {
//     JActorComponent::BeginPlay();
// }
//
// void BT_TEST::Destroy()
// {
//     JActorComponent::Destroy();
// }
//
// void BT_TEST::Tick(float DeltaTime)
// {
//     JActorComponent::Tick(DeltaTime);
//     BBTick();
//     BTRoot->tick();
// }
//
// void BT_TEST::BBTick()
// {
//     JCameraComponent* cam = GetWorld.CameraManager->GetCurrentMainCam();
//     XMMATRIX Mat = cam->GetWorldMatrix();
//     XMFLOAT4X4 fMat;
//     XMStoreFloat4x4(&fMat, Mat);
//     FVector camLocation = FVector(fMat._41, fMat._42, fMat._43);
//
//     FVector myLocation = GetOwnerActor()->GetLocalLocation();
//     BB.Direction = camLocation - myLocation;
//     BB.Distance = BB.Direction.Length();
//     // std::cout << "cam: " << camLocation.z << std::endl;
//     // std::cout << "my: " << myLocation.z << std::endl;
// }
//
// // Action Function
// NodeStatus BT_TEST::StopChase()
// {
//     std::cout << "Stop Chase" << std::endl;
//     return NodeStatus::Success;
// }
//
// NodeStatus BT_TEST::StopChase2()
// {
//     BB.Flag = false;
//     std::cout << "Stop Chase" << std::endl;
//     return NodeStatus::Success;
// }
//
//
// NodeStatus BT_TEST::ChasePlayer()
// {
//     JMath::TVector location = GetOwnerActor()->GetLocalLocation();
//     location.x += BB.Direction.x * MAG / BB.Distance;
//     location.y += BB.Direction.y * MAG / BB.Distance;
//     location.z += BB.Direction.z * MAG / BB.Distance;
//     GetOwnerActor()->SetLocalLocation(location);
//     
//     std::cout << "Chase Player" << std::endl;
//     return NodeStatus::Success;
// }
//
// NodeStatus BT_TEST::ChasePlayer2()
// {
//     JMath::TVector location = GetOwnerActor()->GetLocalLocation();
//     location.x += BB.Direction.x * MAG / BB.Distance;
//     location.z += BB.Direction.z * MAG / BB.Distance;
//     GetOwnerActor()->SetLocalLocation(location);
//     
//     std::cout << "Chase Player2" << std::endl;
//     return NodeStatus::Success;
// }
//
// NodeStatus BT_TEST::ChasePlayer3()
// {
//     JMath::TVector location = GetOwnerActor()->GetLocalLocation();
//     location.x += BB.Direction.x * MAG / BB.Distance;
//     location.y = abs(sin(BB.Timer.Elapsed() * 7)) * 3;
//     location.z += BB.Direction.z * MAG / BB.Distance;
//     GetOwnerActor()->SetLocalLocation(location);
//     
//     std::cout << "Chase Player3" << std::endl;
//     return NodeStatus::Success;
// }
//
// NodeStatus BT_TEST::IsPlayerClose(UINT N)
// {
//     if (BB.Distance <= N)
//         return NodeStatus::Success;
//     else
//         return NodeStatus::Failure;
// }
//
// NodeStatus BT_TEST::Not(NodeStatus state)
// {
//     if (state == NodeStatus::Success)
//         return NodeStatus::Failure;
//     else if (state == NodeStatus::Failure)
//         return NodeStatus::Success;
//     else
//         return state;
// }
//
// NodeStatus BT_TEST::RandN(UINT N)
// {
//     if (!BB.Flag)
//     {
//         BB.RandIndex = FMath::GenerateNum(0, N - 1);
//         BB.Flag = true;
//     }
//     return NodeStatus::Success;
// }
//
// NodeStatus BT_TEST::IsRandSame(UINT N)
// {
//     if (BB.RandIndex == N)
//         return NodeStatus::Success;
//     else
//         return NodeStatus::Failure;
// }
//
//
// void BT_TEST::SetupTree()
// {
//     BTRoot = builder
//             .CreateRoot<Selector>()
//                 .AddDecorator(LAMBDA(IsPlayerClose, 5))
//                     .AddActionNode(LAMBDA(StopChase))
//                 .EndBranch()
//                 .AddActionNode(LAMBDA(ChasePlayer3))
//             .Build();
// }
//
// void BT_TEST::SetupTree2()
// {
//     BTRoot = builder
//             .CreateRoot<Selector>()
//                 .AddDecorator(LAMBDA(IsPlayerClose, 5))
//                     .AddActionNode(LAMBDA(StopChase2))
//                 .EndBranch()
//                 .AddDecorator(LAMBDA(RandN, 2))
//                     .AddSelector("ActSelector")
//                         .AddDecorator(LAMBDA(IsRandSame, 0))
//                             .AddActionNode(LAMBDA(ChasePlayer2))
//                         .EndBranch()
//                         .AddDecorator(LAMBDA(IsRandSame, 1))
//                             .AddActionNode(LAMBDA(ChasePlayer3))
//                         .EndBranch()
//                     .EndBranch()
//                 .EndBranch()
//             .Build();
// }