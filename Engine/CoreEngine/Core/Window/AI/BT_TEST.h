#pragma once
// #include "Core/Entity/Actor/JActor.h"
#include "Core/Entity/Component/JActorComponent.h"
#include "BTBuilder.h"

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

public:
    // Action Function
    NodeStatus StopChase();
    NodeStatus ChasePlayer();

    // Decorator Function
    // NodeStatus IsPlayerClose(Ptr<JActor> A, Ptr<JActor> B);
    NodeStatus IsPlayerClose();
    NodeStatus Not(NodeStatus state);

public:
    BTBuilder builder;
    Ptr<Node> BTRoot;
};
