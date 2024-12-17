// #pragma once
// #include "Core/Entity/Component/JActorComponent.h"
// #include "BTBuilder.h"
// #include "Core/Utils/Math/Vector.h"
// #include "Core/Utils/Timer.h"
// #include "Core/Input/XKeyboardMouse.h"
//
// struct BlackBoard
// {
//     float Distance;
//     FVector Direction;
//     Timer Timer;
//     UINT RandIndex;
//     bool Flag = false;
// };
//
// class BT_TEST : public JActorComponent
// {
// public:
//     BT_TEST(JTextView InName) : JActorComponent(InName) { mInputKeyboard.Initialize(); }
//     ~BT_TEST() override;
//
//     void Initialize() override;
//     void BeginPlay() override;
//     void Destroy() override;
//
//     void Tick(float DeltaTime) override;
//     void SetupTree();
//     void SetupTree2();
//     void BBTick();
//
// public:
//     // Action Function
//     NodeStatus StopChase();
//     NodeStatus StopChase2();
//     NodeStatus ChasePlayer();
//     NodeStatus ChasePlayer2();
//     NodeStatus ChasePlayer3();
//     
//     // Decorator Function
//     NodeStatus IsPlayerClose(UINT N);
//     NodeStatus Not(NodeStatus state);
//     NodeStatus RandN(UINT N);
//     NodeStatus IsRandSame(UINT N);
//
// public:
//     BTBuilder builder;
//     Ptr<Node> BTRoot;
//     BlackBoard BB;
//
// private:
//     FORCEINLINE bool IsKeyPressed(EKeyCode InKey) const { return mInputKeyboard.IsKeyPressed(InKey); }
//     FORCEINLINE bool IsKeyDown(EKeyCode InKey) const { return mInputKeyboard.IsKeyDown(InKey); }
//     XKeyboardMouse mInputKeyboard;
// };
