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
    void Tick(float DeltaTime) override;
    void Destroy() override;

public:
    // Selector root;
    // Sequence sequence;
    BTBuilder builder;
    
};
