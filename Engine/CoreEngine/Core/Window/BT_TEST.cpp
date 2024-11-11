#include "BT_TEST.h"

#include "Core/Interface/MManagerInterface.h"
#include "GUI/GUI_Inspector.h"


BT_TEST::BT_TEST(){}

BT_TEST::BT_TEST(JTextView InName)
    : JActor(InName){}

BT_TEST::~BT_TEST() {}

void BT_TEST::Initialize()
{
    JActor::Initialize();
}

void BT_TEST::BeginPlay()
{
    JActor::BeginPlay();
}

void BT_TEST::Tick(float DeltaTime)
{
    JActor::Tick(DeltaTime);
    if (auto sphere = mParentActor.lock())
    {
        FVector location = sphere->GetLocalLocation();
    }
}

void BT_TEST::Destroy()
{
    JActor::Destroy();
}

void BT_TEST::Draw()
{
    JActor::Draw();
}

bool BT_TEST::Serialize_Implement(std::ofstream& FileStream)
{
    return JActor::Serialize_Implement(FileStream);
}

bool BT_TEST::DeSerialize_Implement(std::ifstream& InFileStream)
{
    return JActor::DeSerialize_Implement(InFileStream);
}
