﻿#pragma once
#include "common_include.h"
#include "Core/Entity/Component/Scene/Shape/JShape.h"

class AActor;
class JCameraComponent;

#define MAX_DEPTH 3

namespace Oc
{

	struct FNode
	{
		uint32_t Depth          = 0;
		uint32_t CornerIndex[4] = {0, 0, 0, 0};
		bool     bIsLeaf        = false;

		FBoxShape BoundArea;

		JArray<AActor*> Actors;

		FNode*      Root        = nullptr;
		FNode*      Parent      = nullptr;
		UPtr<FNode> Children[8] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

		void Update();
		void Render(JCameraComponent* InCamera);

		void SetBoundArea(const FBoxShape& InBoundArea) { BoundArea = InBoundArea; }
		void Subdivide(FNode* InRoot);
		void Insert(AActor* InActor);
		void InsertIntoChildren(AActor* InActor);
		bool Remove(AActor* InActor);
		void Clear();
	};

	class JTree
	{
	public:
		JTree() = default;

	public:
		void Initialize(const FBoxShape& InRootBoundArea, uint32_t InDepth);
		void Update();
		void Insert(AActor* InActor);
		void Render(JCameraComponent* InCamera);
		void Remove(AActor* InActor);
		void Sort(AActor* InActor);
		void ReClassify(AActor* InActor);

	private:
		void Subdivide(FNode* InNode, uint32_t InDepth, FNode* InRoot);

	public:
		JArray<AActor*> IgnoreFrustumActors;

	private:
		UPtr<FNode> mRootNode = nullptr;

	};
}
