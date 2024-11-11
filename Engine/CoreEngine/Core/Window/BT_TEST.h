#pragma once
#include "Core/Entity/Actor/JActor.h"

class BT_TEST : public JActor
{
public:
    BT_TEST();
    BT_TEST(JTextView InName);
    ~BT_TEST() override;
    
    void Initialize() override;
    void BeginPlay() override;
    void Tick(float DeltaTime) override;
    void Destroy() override;

    void Draw() override;

    bool Serialize_Implement(std::ofstream& FileStream) override;
    bool DeSerialize_Implement(std::ifstream& InFileStream) override;
};
