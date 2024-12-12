#pragma once
// #include "Core/Entity/Actor/JActor.h"
#include "Core/Entity/Component/JActorComponent.h"
#include "BTBuilder.h"
#include "Core/Utils/Math/Vector.h"

#include "Core/Utils/Timer.h"

struct BlackBoard
{
    float distance;
    FVector direction;
    Timer timer;
    UINT randIndex;
    bool flag = false;
};

class BT_TEST : public JActorComponent
{
public:
    BT_TEST();
    BT_TEST(JTextView InName);
    ~BT_TEST() override;

    void Initialize() override;
    void BeginPlay() override;
    void Destroy() override;

    void Tick(float DeltaTime) override;
    void SetupTree();
    void SetupTree2();
    void BBTick();

public:
    // Action Function
    NodeStatus StopChase();
    NodeStatus StopChase2();
    NodeStatus ChasePlayer();
    NodeStatus ChasePlayer2();
    NodeStatus ChasePlayer3();
    
    // Decorator Function
    NodeStatus IsPlayerClose(UINT N);
    NodeStatus Not(NodeStatus state);
    NodeStatus RandN(UINT N);
    NodeStatus IsRandSame(UINT N);

public:
    BTBuilder builder;
    Ptr<Node> BTRoot;
    BlackBoard BB;
};
