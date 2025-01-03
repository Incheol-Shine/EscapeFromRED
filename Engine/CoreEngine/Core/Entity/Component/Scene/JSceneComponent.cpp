﻿#include "JSceneComponent.h"

#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/Material/MMaterialInstanceManager.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Interface/JWorld.h"
#include "imgui/imgui_stdlib.h"
#include "Core/Entity/Component/Scene/Shape/JShape.h"
#include "Core/Entity/Navigation/NavTest.h"
#include "imgui/imgui.h"

JSceneComponent_2D::JSceneComponent_2D()
{
}

JSceneComponent_2D::JSceneComponent_2D(JTextView InName, AActor* InOwnerActor)
{
}

JSceneComponent_2D::~JSceneComponent_2D()
{
}

bool JSceneComponent_2D::Serialize_Implement(std::ofstream& FileStream)
{
    return JActorComponent::Serialize_Implement(FileStream);
}

bool JSceneComponent_2D::DeSerialize_Implement(std::ifstream& InFileStream)
{
    return JActorComponent::DeSerialize_Implement(InFileStream);
}

void JSceneComponent_2D::Initialize()
{
    m2DMaterialInstance = GetWorld.MaterialInstanceManager->Load(NAME_MAT_2D);
}

void JSceneComponent_2D::Tick(float DeltaTime)
{
    JActorComponent::Tick(DeltaTime);
}

JSceneComponent::JSceneComponent()
    : mParentSceneComponent(nullptr)
{
    mObjectType = NAME_OBJECT_SCENE_COMPONENT;
}

JSceneComponent::JSceneComponent(JTextView InName, AActor* InOwnerActor,
                                 JSceneComponent* InParentSceneComponent)
    : JActorComponent(InName)
{
    mObjectType = NAME_OBJECT_SCENE_COMPONENT;

    mOwnerActor = InOwnerActor;
    mParentSceneComponent = InParentSceneComponent;
}

JSceneComponent::JSceneComponent(const JSceneComponent& Copy)
    : JActorComponent(Copy),
      mParentSceneComponent(nullptr),
      mWorldLocation(Copy.mWorldLocation),
      mWorldRotation(Copy.mWorldRotation),
      mWorldScale(Copy.mWorldScale),
      mLocalLocation(Copy.mLocalLocation),
      mLocalRotation(Copy.mLocalRotation),
      mLocalScale(Copy.mLocalScale)
{
    for (int32_t i = 0; i < Copy.mChildSceneComponents.size(); ++i)
    {
        UPtr<JSceneComponent> newObj = MakeUPtr<JSceneComponent>(*Copy.mChildSceneComponents[i].get());
        newObj->mParentSceneComponent = this;
        mChildSceneComponentIndices[newObj->GetName()] = i;
        mChildSceneComponents.push_back(std::move(newObj));
    }
}


JSceneComponent::~JSceneComponent()
{
}

bool JSceneComponent::Serialize_Implement(std::ofstream& FileStream)
{
    if (!JActorComponent::Serialize_Implement(FileStream))
    {
        return false;
    }

    int32_t childCount = mChildSceneComponents.size();
    Utils::Serialization::Serialize_Primitive(&childCount, sizeof(int32_t), FileStream);


    for (int32_t i = 0; i < childCount; ++i)
    {
        JText childType = mChildSceneComponents[i]->GetObjectType();

        Utils::Serialization::Serialize_Text(childType, FileStream);

        mChildSceneComponents[i]->Serialize_Implement(FileStream);
    }


    // World Transform Data
    Utils::Serialization::Serialize_Primitive(&mWorldLocation, sizeof(FVector), FileStream);
    Utils::Serialization::Serialize_Primitive(&mWorldRotation, sizeof(FVector), FileStream);
    Utils::Serialization::Serialize_Primitive(&mWorldScale, sizeof(FVector), FileStream);

    // Local Transform Data
    Utils::Serialization::Serialize_Primitive(&mLocalLocation, sizeof(FVector), FileStream);
    Utils::Serialization::Serialize_Primitive(&mLocalRotation, sizeof(FVector), FileStream);
    Utils::Serialization::Serialize_Primitive(&mLocalScale, sizeof(FVector), FileStream);

    // Bounding Box
    Utils::Serialization::Serialize_Primitive(&mBoundingBox, sizeof(mBoundingBox), FileStream);

    return true;
}

bool JSceneComponent::DeSerialize_Implement(std::ifstream& InFileStream)
{
    if (!JActorComponent::DeSerialize_Implement(InFileStream))
    {
        return false;
    }

    int32_t childCount;
    Utils::Serialization::DeSerialize_Primitive(&childCount, sizeof(int32_t), InFileStream);


    mChildSceneComponents.reserve(childCount);

    for (int32_t i = 0; i < childCount; ++i)
    {
        // 자식 씬 컴포넌트의 이름
        JText childType;
        Utils::Serialization::DeSerialize_Text(childType, InFileStream);

        UPtr<JSceneComponent> newObj = UPtrCast<JSceneComponent>(MClassFactory::Get().Create(childType));
        newObj->mParentSceneComponent = this;
        newObj->SetOwnerActor(mOwnerActor == nullptr ? static_cast<AActor*>(this) : mOwnerActor);

        newObj->DeSerialize_Implement(InFileStream);
        mChildSceneComponentIndices[newObj->GetName()] = i;
        mChildSceneComponents.push_back(std::move(newObj));
    }


    // World Transform Data
    Utils::Serialization::DeSerialize_Primitive(&mWorldLocation, sizeof(FVector), InFileStream);
    Utils::Serialization::DeSerialize_Primitive(&mWorldRotation, sizeof(FVector), InFileStream);
    Utils::Serialization::DeSerialize_Primitive(&mWorldScale, sizeof(FVector), InFileStream);

    // Local Transform Data
    Utils::Serialization::DeSerialize_Primitive(&mLocalLocation, sizeof(FVector), InFileStream);
    Utils::Serialization::DeSerialize_Primitive(&mLocalRotation, sizeof(FVector), InFileStream);
    Utils::Serialization::DeSerialize_Primitive(&mLocalScale, sizeof(FVector), InFileStream);

    // Bounding Box
    Utils::Serialization::DeSerialize_Primitive(&mBoundingBox, sizeof(mBoundingBox), InFileStream);

    return true;
}

void JSceneComponent::Tick(float DeltaTime)
{
    UpdateTransform();

    for (int32_t i = 0; i < mChildSceneComponents.size(); ++i)
    {
        mChildSceneComponents[i]->Tick(DeltaTime);
    }
}

void JSceneComponent::Draw()
{
    for (int32_t i = 0; i < mChildSceneComponents.size(); ++i)
    {
        mChildSceneComponents[i]->Draw();
    }
}

void JSceneComponent::DrawID(uint32_t ID)
{
    for (int32_t i = 0; i < mChildSceneComponents.size(); ++i)
    {
        mChildSceneComponents[i]->DrawID(ID);
    }
}

void JSceneComponent::ShowEditor()
{
    ImGui::InputText(u8("이름"),
                     &mName,
                     ImGuiInputTextFlags_CharsNoBlank);

    ImGui::Separator();

    if (ImGui::CollapsingHeader(u8("변환 정보")))
    {
        ImGui::DragFloat3(u8("위치"), &mLocalLocation.x, 1.f);
        ImGui::DragFloat3(u8("회전"), &mLocalRotation.x, 0.1f, -360.0f, 360.0f);
        ImGui::DragFloat3(u8("크기"), &mLocalScale.x, 0.01f, 0.f, 10.0f);
    }

    ImGui::SeparatorText(u8("오브젝트 플래그"));
    for (int32_t i = 0; i < 6; i++)
    {
        uint32_t value = (i == 0) ? 1 : 1 << i;
        bool bFlag = (mObjectFlags & value);
        if (ImGui::Checkbox(NAME_OBJECT_FLAGS[i], &bFlag))
        {
            bFlag ? SetFlag(value) : RemoveFlag(value);
        }
    }
}

void JSceneComponent::SetWorldLocation(const FVector& InTranslation)
{
    mWorldLocation = InTranslation;
    UpdateWorldTransform();

    // 로컬 위치 계산
    if (mParentSceneComponent)
    {
        FMatrix parentWorldInverse = mParentSceneComponent->mWorldMat.Invert();
        mLocalMat = parentWorldInverse * mWorldMat;
        mLocalLocation = FVector(mLocalMat.m[3][0], mLocalMat.m[3][1], mLocalMat.m[3][2]);
    }
    else
    {
        mLocalLocation = InTranslation;
    }
}

void JSceneComponent::SetWorldRotation(const FVector& InRotation)
{
    mWorldRotation = InRotation;
    UpdateWorldTransform();

    if (mParentSceneComponent)
    {
        // 월드 회전을 쿼터니언으로 변환
        DirectX::XMVECTOR worldQuat = DirectX::XMQuaternionRotationRollPitchYaw(
            DirectX::XMConvertToRadians(InRotation.x),
            DirectX::XMConvertToRadians(InRotation.y),
            DirectX::XMConvertToRadians(InRotation.z));

        // 부모 월드 회전을 쿼터니언으로 변환
        DirectX::XMVECTOR parentQuat = XMQuaternionRotationMatrix(mParentSceneComponent->mWorldMat);
        DirectX::XMVECTOR parentQuatInv = DirectX::XMQuaternionInverse(parentQuat);

        // 로컬 회전 = 부모 월드 회전의 역 * 월드 회전
        DirectX::XMVECTOR localQuat = DirectX::XMQuaternionMultiply(parentQuatInv, worldQuat);

        // 로컬 회전을 오일러 각도로 변환
        DirectX::XMVECTOR axis;
        float angle;
        DirectX::XMQuaternionToAxisAngle(&axis, &angle, localQuat);

        mLocalRotation.x = axis.m128_f32[0] * angle;
        mLocalRotation.y = axis.m128_f32[1] * angle;
        mLocalRotation.z = axis.m128_f32[2] * angle;
    }
    else
    {
        mLocalRotation = InRotation;
    }
}

void JSceneComponent::SetWorldScale(const FVector& InScale)
{
    mWorldScale = InScale;
    UpdateWorldTransform();

    if (mParentSceneComponent)
    {
        FMatrix parentWorldInverse = mParentSceneComponent->mWorldMat.Invert();
        mLocalMat = parentWorldInverse * mWorldMat;

        // 각 축 벡터의 길이로 스케일 계산
        FVector scaleX(mLocalMat.m[0][0], mLocalMat.m[0][1], mLocalMat.m[0][2]);
        FVector scaleY(mLocalMat.m[1][0], mLocalMat.m[1][1], mLocalMat.m[1][2]);
        FVector scaleZ(mLocalMat.m[2][0], mLocalMat.m[2][1], mLocalMat.m[2][2]);

        mLocalScale = FVector(scaleX.Length(), scaleY.Length(), scaleZ.Length());
    }
    else
    {
        mLocalScale = InScale;
    }
}

void JSceneComponent::AddChildSceneComponent(JSceneComponent* Ptr)
{
}

void JSceneComponent::SetupAttachment(JSceneComponent* InParentComponent)
{
    if (!InParentComponent || InParentComponent == mParentSceneComponent)
    {
        return;
    }

    const int32_t index = mOwnerActor->mChildSceneComponentIndices[GetName()];

    InParentComponent->mChildSceneComponentIndices[GetName()] = InParentComponent->mChildSceneComponents.size();
    InParentComponent->mChildSceneComponents.push_back(std::move(mOwnerActor->mChildSceneComponents[index]));

    // 원래 존재하던 mParentSceneComponent의 배열 업데이트
    mOwnerActor->mChildSceneComponentIndices.erase(GetName());
    mOwnerActor->mChildSceneComponents.erase(mOwnerActor->mChildSceneComponents.begin() + index);

    mParentSceneComponent = InParentComponent;
    SetOwnerActor(InParentComponent->GetOwnerActor());
}

void JSceneComponent::AttachComponent(JSceneComponent* InChildComponent)
{
    if (!InChildComponent)
    {
        return;
    }
}

void JSceneComponent::AttachToComponent(const Ptr<JSceneComponent>& InParentComponent)
{
}

void JSceneComponent::AttachToActor(AActor* InParentActor, const JText& InComponentAttachTo)
{
}

void JSceneComponent::AttachToActor(AActor* InParentActor, JSceneComponent* InComponentAttachTo)
{
    assert(InParentActor);
}

void JSceneComponent::DetachFromComponent()
{
}

JSceneComponent* JSceneComponent::GetComponentByName(const JText& InName)
{
    auto it = mChildSceneComponentIndices.find(InName);
    if (it != mChildSceneComponentIndices.end())
    {
        return mChildSceneComponents[it->second].get();
    }
    return nullptr;
}

void JSceneComponent::UpdateTransform()
{
    mCachedLocalMat = mWorldMat;

    // Step1. 로컬 좌표 변환
    mLocalLocationMat = DirectX::XMMatrixTranslation(mLocalLocation.x, mLocalLocation.y, mLocalLocation.z);

    mLocalRotationMat = XMMatrixRotationRollPitchYaw(
        XMConvertToRadians(mLocalRotation.x),
        XMConvertToRadians(mLocalRotation.y),
        XMConvertToRadians(mLocalRotation.z));

    mLocalScaleMat = DirectX::XMMatrixScaling(mLocalScale.x, mLocalScale.y, mLocalScale.z);

    mLocalMat = mLocalScaleMat * mLocalRotationMat * mLocalLocationMat;

    // Step2. 부모 씬 컴포넌트가 존재한다면, 로컬 행렬을 계산
    if (mParentSceneComponent)
    {
        mWorldMat = mLocalMat * mParentSceneComponent->mWorldMat;
    }
    else // 부모가 없는 씬 컴포넌트는 그 자체 위치를 월드 위치로 사용
    {
        mWorldMat = mLocalMat;
    }

    DirectX::XMVECTOR scale, rot, loc;
    XMMatrixDecompose(&scale, &rot, &loc, mWorldMat);
    mWorldLocation = loc;
    mWorldRotation = rot;
    mWorldScale = scale;

    if (mCachedLocalMat != mWorldMat)
    {
        MarkAsDirty();
    }
}

void JSceneComponent::UpdateWorldTransform()
{
    mWorldMat = DirectX::XMMatrixScaling(
            mWorldScale.x,
            mWorldScale.y,
            mWorldScale.z) *
        DirectX::XMMatrixRotationRollPitchYaw(
            DirectX::XMConvertToRadians(mWorldRotation.x),
            DirectX::XMConvertToRadians(mWorldRotation.y),
            DirectX::XMConvertToRadians(mWorldRotation.z)) *
        DirectX::XMMatrixTranslation(
            mWorldLocation.x,
            mWorldLocation.y,
            mWorldLocation.z);
}

JBoxComponent::JBoxComponent()
    : JSceneComponent()
{
    mObjectType = NAME_OBJECT_BOX_COMPONENT;
}

JBoxComponent::JBoxComponent(JTextView InName, AActor* InOwnerActor, JSceneComponent* InParentSceneComponent)
    : JSceneComponent(InName, InOwnerActor, InParentSceneComponent)
{
    mObjectType = NAME_OBJECT_BOX_COMPONENT;
}

bool JBoxComponent::Serialize_Implement(std::ofstream& FileStream)
{
    if (!JSceneComponent::Serialize_Implement(FileStream))
        return false;

    // Box Color
    // Utils::Serialization::Serialize_Primitive(&mColor, sizeof(mColor), FileStream);

    return true;
}

bool JBoxComponent::DeSerialize_Implement(std::ifstream& InFileStream)
{
    if (!JSceneComponent::DeSerialize_Implement(InFileStream))
        return false;

    // Box Color;
    // Utils::Serialization::DeSerialize_Primitive(&mColor, sizeof(mColor), InFileStream);

    return true;
}

void JBoxComponent::Initialize()
{
    JSceneComponent::Initialize();
}

void JBoxComponent::Tick(float DeltaTime)
{
    JSceneComponent::Tick(DeltaTime);

    // Step1.1 프러스텀 박스(OBB) 업데이트
    mBoundingBox.Box.LocalAxis[0] = XMVector3TransformNormal(FVector(1, 0, 0), XMLoadFloat4x4(&mWorldMat));
    mBoundingBox.Box.LocalAxis[1] = XMVector3TransformNormal(FVector(0, 1, 0), XMLoadFloat4x4(&mWorldMat));
    mBoundingBox.Box.LocalAxis[2] = XMVector3TransformNormal(FVector(0, 0, 1), XMLoadFloat4x4(&mWorldMat));

    mBoundingBox.Box.Center = mWorldLocation;

    RayOrigin = mOwnerActor->GetWorldLocation() + FVector(0, CRayOffset, 0);

    if (IsNPC)
    {
        for (auto collider : NAV_MAP.ColliderTarget)
        {
            if (collider != this)
            {
                FVector normal = FVector::ZeroVector;
                float depth = std::numeric_limits<float>::infinity();
                if (IsIntersectOBB(*collider, normal, depth))
                {
                    // LOG_CORE_TRACE("<collision> {} < ---- > {}");
                    HandleCollision(*collider, normal, depth, DeltaTime);
                }
                else
                {
                    // LOG_CORE_TRACE("No");
                }
            }
        }
        FRay TempRay(RayOrigin, RayDir);
        float MaxHeight = 100.f;
        for (auto plane : NAV_MAP.GroundColliders)
        {
            FVector tempHeight = FVector::ZeroVector;
            if (plane)
            {
                if (RayIntersectOBB(TempRay, plane->mBoundingBox, tempHeight))
                {
                    MaxHeight = FMath::Max(MaxHeight, tempHeight.y);
                }
            }
        }
        GroundHeight = MaxHeight;
    }
    for (auto ray : NAV_MAP.RayCollider)
    {
        FVector tempHeight = FVector::ZeroVector;
        if (ray)
        {
            // if (RayIntersectAABB(ray->mRay, mBoundingBox, tempHeight))
            if (IsIntersect(ray->mRay, tempHeight))
            {
                LOG_CORE_TRACE("{} {} < ---- > {}", tempHeight.y, ray->GetOwnerActor()->GetName(), mOwnerActor->GetName());
                
            }
            // else
            // {
            //     LOG_CORE_TRACE("<충돌 없음> ");
            // }
        }
    }
}

void JBoxComponent::HandleCollisionAABB(const JBoxComponent& OtherBoxComp, float DeltaTime)
{
    // return mBoundingBox.Intersect(OtherBoxComp.mBoundingBox);
    const auto otherMin = OtherBoxComp.mBoundingBox.Box.Center - OtherBoxComp.mBoundingBox.Box.Extent;
    const auto otherMax = OtherBoxComp.mBoundingBox.Box.Center + OtherBoxComp.mBoundingBox.Box.Extent;

    const auto thisMin = mBoundingBox.Box.Center - mBoundingBox.Box.Extent;
    const auto thisMax = mBoundingBox.Box.Center + mBoundingBox.Box.Extent;

    float overlapX = FMath::Min(otherMax.x, thisMax.x) - FMath::Max(otherMin.x, thisMin.x);
    float overlapY = FMath::Min(otherMax.y, thisMax.y) - FMath::Max(otherMin.y, thisMin.y);
    float overlapZ = FMath::Min(otherMax.z, thisMax.z) - FMath::Max(otherMin.z, thisMin.z);

    // float minOverlap = FMath::Min3(overlapX, overlapY, overlapZ);
    float minOverlap = FMath::Min(FMath::Min(overlapX, overlapY), overlapZ);
    FVector pushDirection;

    if (minOverlap == overlapX)
    {
        if (thisMin.x < otherMin.x)
            pushDirection = FVector(overlapX, 0, 0);
        else
            pushDirection = -FVector(overlapX, 0, 0);
    }
    else if (minOverlap == overlapY)
    {
        if (thisMin.y < otherMin.y)
            pushDirection = FVector(0, overlapY, 0);
        else
            pushDirection = -FVector(0, overlapY, 0);
    }
    else
    {
        if (thisMin.z < otherMin.z)
            pushDirection = FVector(0, 0, overlapZ);
        else
            pushDirection = -FVector(0, 0, overlapZ);
    }

    pushDirection *= DeltaTime * 10;
    FVector ThisPosition = mOwnerActor->GetWorldLocation();
    FVector OtherPosition = OtherBoxComp.mOwnerActor->GetWorldLocation();

    mOwnerActor->SetWorldLocation(ThisPosition - pushDirection);
    OtherBoxComp.mOwnerActor->SetWorldLocation(OtherPosition + pushDirection);
}

void JBoxComponent::HandleCollision(const JBoxComponent& OtherBoxComp, FVector normal, float depth, float DeltaTime)
{
    // float minOverlap = FMath::Min3(overlapX, overlapY, overlapZ);
    FVector pushDirection = normal * depth * 5;
    

    pushDirection *= DeltaTime;
    FVector ThisPosition = mOwnerActor->GetWorldLocation();
    FVector OtherPosition = OtherBoxComp.mOwnerActor->GetWorldLocation();

    mOwnerActor->SetWorldLocation(ThisPosition - pushDirection);
    OtherBoxComp.mOwnerActor->SetWorldLocation(OtherPosition + pushDirection);
}

void JBoxComponent::Draw()
{
    mBoundingBox.DrawDebug(mColor);
}

void JBoxComponent::ShowEditor()
{
    JSceneComponent::ShowEditor();

    ImGui::SeparatorText(u8("박스 컴포넌트"));

    ImGui::DragFloat3(u8("반지름"), &mBoundingBox.Box.Extent.x, 0.1f);

    ImGui::ColorEdit4(u8("색상"), &mColor.x);
}

bool JBoxComponent::IsIntersect(const JBoxComponent& OtherBoxComp)
{
    return mBoundingBox.Intersect(OtherBoxComp.mBoundingBox);
}


bool JBoxComponent::IsIntersectOBB(const JBoxComponent& OtherBoxComp)
{
    return mBoundingBox.IntersectOBB(OtherBoxComp.mBoundingBox);
}

bool JBoxComponent::IsIntersectOBB(const JBoxComponent& OtherBoxComp, FVector& CollisionNormal, float& CollisionDepth)
{
    return mBoundingBox.IntersectOBB(OtherBoxComp.mBoundingBox, CollisionNormal, CollisionDepth);
}

//
bool JBoxComponent::IsIntersect(const FBoxShape& OtherBox)
{
    return mBoundingBox.Intersect(OtherBox);
}

//
bool JBoxComponent::IsIntersect(const FRay& InRay, FVector& OutT)
{
    return RayIntersectOBB(InRay, mBoundingBox, OutT);
}


SimpleRayComponent::SimpleRayComponent(JTextView InName, AActor* InOwnerActor, JSceneComponent* InParentSceneComponent)
    : JSceneComponent(InName, InOwnerActor, InParentSceneComponent)
{
}

void SimpleRayComponent::Initialize()
{
    JSceneComponent::Initialize();
}

void SimpleRayComponent::Tick(float DeltaTime)
{
    JSceneComponent::Tick(DeltaTime);

    mRay.Origin = mWorldLocation;
    FVector TempDirection = FVector(0,-1,0);
    mRay.Direction.x = TempDirection.Dot({mOwnerActor->GetWorldMatrix()._11, mOwnerActor->GetWorldMatrix()._21, mOwnerActor->GetWorldMatrix()._31});
    mRay.Direction.y = TempDirection.Dot({mOwnerActor->GetWorldMatrix()._12, mOwnerActor->GetWorldMatrix()._22, mOwnerActor->GetWorldMatrix()._32});
    mRay.Direction.z = TempDirection.Dot({mOwnerActor->GetWorldMatrix()._13, mOwnerActor->GetWorldMatrix()._23, mOwnerActor->GetWorldMatrix()._33});
}

void SimpleRayComponent::Draw()
{
    JSceneComponent::Draw();

    mRay.DrawDebug();
    // auto* cam = GetWorld.CameraManager->GetCurrentMainCam();
}
