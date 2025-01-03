﻿#include "SpaceDivision.h"
#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Camera/JCameraComponent.h"

void Oc::FNode::Update()
{
	JArray<AActor*> actorsToSort;

	std::erase_if(
				  Actors,
				  [&](AActor* Actor){

					  if (Actor->IsMarkedAsDirty())
					  {
						  if (!BoundArea.Intersect(Actor->GetBoundingVolume()))
						  {
							  actorsToSort.emplace_back(Actor);
							  return true;
						  }
						  Actor->RemoveFlag(MarkAsDirty);
					  }
					  return false;
				  });

	for (const auto& actor : actorsToSort)
	{
		Root->Insert(actor);
		actor->RemoveFlag(EObjectFlags::MarkAsDirty);
	}

	for (int i = 0; i < 8; ++i)
	{
		if (Children[i])
		{
			Children[i]->Update();
		}
	}
}

void Oc::FNode::Render(JCameraComponent* InCamera)
{
	if (InCamera->IsBoxInFrustum(BoundArea))
	{
		// BoundArea.DrawDebug();
		for (const auto& actor : Actors)
		{
			if (actor)
			{
				actor->Draw();
			}
		}

		for (int i = 0; i < 8; ++i)
		{
			if (Children[i])
			{
				Children[i]->Render(InCamera);
			}
		}
	}

}

void Oc::FNode::Subdivide(FNode* InRoot)
{
	Root = InRoot;

	const FVector center = BoundArea.Box.Center;
	const FVector extent = BoundArea.Box.Extent * 0.5f;	// 부모 노드의 크기의 절반
	const FVector min    = center - extent;
	const FVector max    = center + extent;

	// 8개 자식 노드의 중심점을 계산
	const FVector childCenters[8] = {
		FVector(min.x, min.y, min.z), // 앞, 왼쪽, 아래
		FVector(max.x, min.y, min.z), // 앞, 오른쪽, 아래
		FVector(min.x, max.y, min.z), // 뒤, 왼쪽, 아래
		FVector(max.x, max.y, min.z), // 뒤, 오른쪽, 아래
		FVector(min.x, min.y, max.z), // 앞, 왼쪽, 위
		FVector(max.x, min.y, max.z), // 앞, 오른쪽, 위
		FVector(min.x, max.y, max.z), // 뒤, 왼쪽, 위
		FVector(max.x, max.x, max.z)  // 뒤, 오른쪽, 위
	};

	// 각 자식 노드 생성
	for (int i = 0; i < 8; ++i)
	{
		Children[i]         = MakeUPtr<FNode>();
		Children[i]->Parent = this;
		Children[i]->Depth  = Depth + 1;

		// 자식 노드의 BoundArea 설정
		Children[i]->BoundArea = FBoxShape(childCenters[i], extent);
	}

	// 현재 노드는 더 이상 리프 노드가 아님
	bIsLeaf = false;
}

void Oc::FNode::Insert(AActor* InActor)
{
	// 현재 노드가 리프 노드이고 액터를 직접 저장할 수 있는 경우
	if (bIsLeaf)
	{
		// 최대 깊이 또는 자식 노드로 더 이상 분리할 수 없는 경우, 직접 저장
		if (Depth >= MAX_DEPTH || Children[0] != nullptr)
		{
			Actors.emplace_back(InActor);

			return;
		}

		// // 자식 노드로 분리
		// Subdivide();

		// // 기존에 저장된 액터들을 재분배
		// for (const auto& existingActor : Actors)
		// {
		// 	InsertIntoChildren(existingActor);
		// }
		// Actors.clear();
	}

	// 자식 노드 중 하나에 삽입
	InsertIntoChildren(InActor);
}

void Oc::FNode::InsertIntoChildren(AActor* InActor)
{
	const FBoxShape& actorBounds = InActor->GetBoundingVolume();

	int32_t duplicateCount = 0;

	for (int i = 0; i < 8; ++i)
	{
		if (Children[i]->BoundArea.Contains(InActor->GetWorldLocation()))
		{
			duplicateCount++;
			if (duplicateCount > 1)
			{
				Actors.emplace_back(InActor);
				return;
			}
		}
	
	}

	for (int i = 0; i < 8; ++i)
	{
		if (Children[i]->BoundArea.Intersect(actorBounds))
		{
			Children[i]->Insert(InActor);
			return;
		}
	}

	// 액터가 어떤 자식 노드와도 교차하지 않으면, 현재 노드에 저장
	Actors.emplace_back(InActor);
}

bool Oc::FNode::Remove(AActor* InActor)
{
	// 현재 노드에 액터가 있는 경우 제거
	if (const auto it = std::ranges::find(Actors, InActor); it != Actors.end())
	{
		Actors.erase(it);
		return true;
	}

	// 리프 노드라면 더 이상 탐색 불필요
	if (bIsLeaf)
	{
		return false;
	}

	// 자식 노드에서 제거 시도
	for (int i = 0; i < 8; ++i)
	{
		if (Children[i] && Children[i]->Remove(InActor))
		{
			return true;
		}
	}

	return false; // 액터를 찾지 못함
}

void Oc::FNode::Clear()
{
	Actors.clear();
	for (int i = 0; i < 8; ++i)
	{
		if (Children[i])
		{
			Children[i]->Clear();
		}
	}
}

void Oc::JTree::Initialize(const FBoxShape& InRootBoundArea, uint32_t InDepth)
{
	if (mRootNode)
	{
		return;
	}

	mRootNode            = MakeUPtr<FNode>();
	mRootNode->BoundArea = InRootBoundArea;

	Subdivide(mRootNode.get(), InDepth, mRootNode.get());
}

void Oc::JTree::Update()
{
	mRootNode->Update();
}

void Oc::JTree::Insert(AActor* InActor)
{
	if (InActor->IsIgnoreFrustum())
	{
		IgnoreFrustumActors.emplace_back(InActor);
		return;
	}
	mRootNode->Insert(InActor);
}

void Oc::JTree::Render(JCameraComponent* InCamera)
{
	for (const auto& actor : IgnoreFrustumActors)
	{
		actor->Draw();
	}

	// 모든 노드 순회하면서 프러스텀내에 있는 액터들 렌더링
	mRootNode->Render(InCamera);
}

void Oc::JTree::Remove(AActor* InActor)
{
	mRootNode->Remove(InActor);
}

void Oc::JTree::Sort(AActor* InActor)
{
	if (!mRootNode || !InActor)
	{
		return;
	}

	// 기존 액터 제거
	Remove(InActor);

	// 새 위치에 맞게 재삽입
	Insert(InActor);
}

void Oc::JTree::Subdivide(FNode* InNode, uint32_t InDepth, FNode* InRoot)
{
	if (!InNode)
	{
		return;
	}
	if (InNode->Depth >= InDepth) // 최대 깊이에 도달하거나 잘못된 노드일 경우 종료
	{
		InNode->Root    = InRoot;
		InNode->bIsLeaf = true;
		return;
	}

	// 노드 분할
	InNode->Subdivide(InRoot);

	// 각 자식 노드에 대해 재귀적으로 분할 수행
	for (int i = 0; i < 8; ++i)
	{
		InNode->Children[i]->Depth = InNode->Depth + 1; // 자식 노드의 깊이 설정
		Subdivide(InNode->Children[i].get(), InDepth, InRoot);
	}
}
