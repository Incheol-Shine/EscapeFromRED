#include "BT_TEST.h"

#include "Core/Interface/MManagerInterface.h"
#include "GUI/GUI_Inspector.h"


BT_TEST::BT_TEST()
{
}

BT_TEST::BT_TEST(JTextView InName)
    : JSceneComponent(InName)
{
}

BT_TEST::~BT_TEST()
{
}

void BT_TEST::Initialize()
{
    JSceneComponent::Initialize();
}

void BT_TEST::BeginPlay()
{
    JSceneComponent::BeginPlay();
}

void BT_TEST::Tick(float DeltaTime)
{
    JSceneComponent::Tick(DeltaTime);
    Ptr BT = mParentSceneComponent.lock();
    assert(BT);
    FVector location = BT->GetLocalLocation();
    location.z += -0.005f;
    BT->SetLocalLocation(location);
}

void BT_TEST::Destroy()
{
    JSceneComponent::Destroy();
}

void BT_TEST::Draw()
{
    JSceneComponent::Draw();
}

bool BT_TEST::Serialize_Implement(std::ofstream& FileStream)
{
    return JSceneComponent::Serialize_Implement(FileStream);
}

bool BT_TEST::DeSerialize_Implement(std::ifstream& InFileStream)
{
    return JSceneComponent::DeSerialize_Implement(InFileStream);
}
