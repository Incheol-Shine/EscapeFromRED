﻿#include "GUI_Viewport_Scene.h"

#include "Core/Entity/Actor/JStaticMeshActor.h"
#include "Core/Entity/Actor/Character/ACharacter.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Entity/Component/Mesh/JStaticMeshComponent.h"
#include "Core/Entity/Level/MLevelManager.h"
#include "Core/Graphics/Mesh/JSkeletalMeshObject.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Interface/JWorld.h"

GUI_Viewport_Scene::GUI_Viewport_Scene(const JText& InTitle)
	: GUI_Viewport(InTitle),
	  mPauseIcon(nullptr),
	  mPlayIcon(nullptr) {}


void GUI_Viewport_Scene::Initialize()
{
	// mWindowFlags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize;

	mEditorCameraRef = GetWorld.CameraManager->FetchResource<JCamera_Debug>(L"EditorCamera");
	assert(mEditorCameraRef);

	mPauseIcon = GetWorld.TextureManager->CreateOrLoad(L"rsc/Icons/PauseButton On@2x.png");
	mPlayIcon  = GetWorld.TextureManager->CreateOrLoad(L"rsc/Icons/PlayButton On@2x.png");

}

void GUI_Viewport_Scene::Update_Implementation(float DeltaTime)
{
	ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S);
	// ImGui::Shortcut
	ShowTopMenu();

	using Base = GUI_Viewport;
	Base::Update_Implementation(DeltaTime);

	if (ImGui::IsMouseReleased(0) && ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::GetDragDropPayload();;
		{
			JLevel*     activeLevel = GetWorld.LevelManager->GetActiveLevel();
			const char* assetPath   = static_cast<const char*>(payload->Data);

			auto      metaData = Utils::Serialization::GetType(assetPath);
			JTextView name     = std::format("{}_{}", ParseFile(assetPath), activeLevel->GetActorCount());

			switch (metaData.AssetType)
			{
			case HASH_ASSET_TYPE_STATIC_MESH:
				activeLevel->CreateActor<JStaticMeshActor>(name, assetPath);
				break;
			case HASH_ASSET_TYPE_SKELETAL_MESH:

				break;

			default:
				LOG_CORE_WARN("지원하지 않는 메시 타입.");
				break;
			}


		}
		ImGui::EndDragDropTarget();
	}
}

void GUI_Viewport_Scene::ShowTopMenu()
{
	ImVec2 availSize = ImGui::GetContentRegionAvail();

	// 창의 가로 크기 구하기
	float windowWidth = availSize.x;

	// ImGui::SetNextWindowPos(ImVec2(0, 0));
	// ImGui::SetNextWindowSize(ImVec2(windowWidth, 50));

	// 버튼 크기와 여백 정의
	float buttonWidth  = 16.0f;  // 각 버튼의 가로 길이
	float buttonHeight = 16.0f; // 각 버튼의 높이
	float spacing      = 4.0f;      // 버튼 사이의 간격

	// 버튼 3개 (Pause, Play, Stop)의 총 가로 길이 계산
	float totalButtonWidth = (buttonWidth * 2) + (spacing * 1);

	// 버튼들을 중앙에 배치하기 위한 시작 위치 계산
	float offsetX = (windowWidth - totalButtonWidth) * 0.5f;

	// 버튼을 왼쪽 여백으로 이동 (중앙 정렬)
	ImGui::SetCursorPosX(offsetX);

	// Pause 버튼
	if (ImGui::ImageButton("GameBtn", mPauseIcon->GetSRV(), ImVec2(buttonWidth, buttonHeight)))
	{
		// Pause 동작 처리
		std::cout << "Game Paused" << std::endl;
	}

	ImGui::SameLine(0, spacing); // 버튼 간격 띄우기

	// Play 버튼
	if (ImGui::ImageButton("PlayBtn", mPlayIcon->GetSRV(), ImVec2(buttonWidth, buttonHeight)))
	{
		// Play 동작 처리
		std::cout << "Game Playing" << std::endl;
		JTextView         d("Game/Mesh/Tree.jasset");
		ASampleCharacter* character = GetWorld.SpawnActor<ASampleCharacter>("Girl",
																			FVector::ZeroVector,
																			FVector::ZeroVector,
																			nullptr,
																			"Game/Mesh/SK_MechanicGirl_01.jasset");

		character->Initialize();

		bIsGameMode = true;
	}
}
