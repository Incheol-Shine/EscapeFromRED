﻿#pragma once
#include "Core/Entity/JObject.h"
#include "Core/Utils/SpaceDivision/SpaceDivision.h"

namespace Oc
{
	class JTree;
}
class AActor;

/**
 * 레벨에서는 액터 관리(환경, 배치)를 담당한다.
 */
class JLevel : public JAsset
{
public:
	JLevel(const JText& InPath);
	~JLevel() override;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	JText   GetName() const { return mName; }
	int32_t GetActorCount() const { return mActors.size(); }

public:
	void InitializeLevel();
	void UpdateLevel(float DeltaTime);
	void RenderLevel();

public:
	// void AddActor(const Ptr<AActor>& InActor);

	AActor* LoadActorFromPath(const JText& InPath);

	template <typename T, typename... Args>
	T* CreateActor(JTextView InName, Args&&... InArgs);

private:
	JText mName;	// 레벨 이름

public:
	UPtr<Oc::JTree> mOcTree;

	JArray<UPtr<AActor>>    mActors;	// 레벨에 속한 액터들
	JHash<int32_t, int32_t> mActorIndexMap;	// 액터 인덱스 맵 (Node ID, Node Actor Index)

	// TODO : 레벨과 관련된 변수들 추가
	// Time, Mission, States ... etc


	friend class GUI_Inspector;
	friend class GUI_Viewport;
};

template <typename T = AActor, typename... Args>
T* JLevel::CreateActor(JTextView InName, Args&&... InArgs)
{
	static_assert(std::is_base_of<AActor, T>::value, "T must be derived from AActor");

	UPtr<T> newActor = MakeUPtr<T>(InName, std::forward<Args>(InArgs)...);

	T* ptr = newActor.get();

	mOcTree->Insert(ptr);
	mActors.push_back(std::move(newActor));

	return ptr;
}
