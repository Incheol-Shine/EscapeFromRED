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
    builder.tick();
    
    /*FVector location = GetOwnerActor()->GetRootComponent()->GetLocalLocation();
    location.z += -0.005f;
    GetOwnerActor()->GetRootComponent()->SetLocalLocation(location);*/
}

void BT_TEST::Destroy()
{
    JActorComponent::Destroy();
}
