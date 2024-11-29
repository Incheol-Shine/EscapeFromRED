﻿#pragma once
#include "GUI_Editor_Base.h"
#include "Core/Graphics/Material/JMaterial.h"
#include "Core/Graphics/Viewport/MViewportManager.h"

// Forward Declaration
class JCameraComponent;
class JMeshObject;
class JMaterial;
class JShader;
class GUI_Viewport;

/**
 * 머티리얼 에디터 
 */
class GUI_Editor_Material : public GUI_Editor_Base
{
public:
	GUI_Editor_Material(const JText& InTitle);
	~GUI_Editor_Material() override = default;

	void Render() override;

private:
	void Update_Implementation(float DeltaTime) override;

private:
	void SetMeshObject(JTextView InMeshPath);

	void HandleIntegerType(FMaterialParam& MaterialParam);
	void HandleFloatType(FMaterialParam& Param);
	void HandleFloat2Type(FMaterialParam& Param);
	void HandleFloat4Type(FMaterialParam& Param, uint32_t Index) const;

	void ShowMaterialEditor();

	void ShowTextureSlot(FMaterialParam& Param, uint32_t Index) const;

	void Button_AddParam();

private:
	bool bOpenFileBrowser;
	bool bOpenFolder;
	bool bOpenNewParamPopup = false;

	char mFilePath[256];

	UPtr<JMeshObject>  mPreviewMeshObject;	// 머티리얼 적용 표면
	JMaterialInstance* mMaterialToEdit;		// 머티리얼
};
