﻿#include "JSkeletalMeshComponent.h"

#include "Core/Entity/Animation/JAnimator.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/Mesh/JSkeletalMeshObject.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Interface/JWorld.h"

JSkeletalMeshComponent::JSkeletalMeshComponent()
{
	mObjectType = NAME_OBJECT_SKELETAL_MESH_COMPONENT;
}

JSkeletalMeshComponent::JSkeletalMeshComponent(JTextView InName, AActor* InOwnerActor, JSceneComponent* InParentComponent)
	: JSceneComponent(InName, InOwnerActor, InParentComponent)
{
	mObjectType = NAME_OBJECT_SKELETAL_MESH_COMPONENT;
}

uint32_t JSkeletalMeshComponent::GetType() const
{
	return JSceneComponent::GetType();
}

bool JSkeletalMeshComponent::Serialize_Implement(std::ofstream& FileStream)
{
	return JSceneComponent::Serialize_Implement(FileStream);
}

bool JSkeletalMeshComponent::DeSerialize_Implement(std::ifstream& InFileStream)
{
	return JSceneComponent::DeSerialize_Implement(InFileStream);
}

void JSkeletalMeshComponent::Tick(float DeltaTime)
{
	// TransformComponent에서 위치 업데이트
	JSceneComponent::Tick(DeltaTime);

	// MeshObject의 버퍼 업데이트
	if (mSkeletalMeshObject)
	{
		mSkeletalMeshObject->Tick(DeltaTime);
		mSkeletalMeshObject->UpdateInstance_Transform(mWorldMat);
	}
}

void JSkeletalMeshComponent::Draw()
{
	// MeshObject의 Draw 호출
	if (mSkeletalMeshObject)
	{
		FVector distance = mWorldLocation - GetWorld.CameraManager->GetCurrentMainCam()->GetWorldLocation();
		float   dist     = distance.Length();

		mSkeletalMeshObject->AddInstance(dist);
	}

	JSceneComponent::Draw();
}

void JSkeletalMeshComponent::DrawID(uint32_t ID)
{
	if (mSkeletalMeshObject)
	{
		mSkeletalMeshObject->DrawID(ID);
	}

	JSceneComponent::DrawID(ID);
}

void JSkeletalMeshComponent::SetSkeletalMesh(JTextView InSkeletalMeshPath)
{
	mSkeletalMeshObject = GetWorld.MeshManager->Clone<JSkeletalMeshObject>(InSkeletalMeshPath.data());
	// mAnimator           = MakeUPtr<JAnimator>(mSkeletalMeshObject->mSkeletalMesh->mSkeleton);
}

void JSkeletalMeshComponent::ShowEditor()
{
	JSceneComponent::ShowEditor();


}
