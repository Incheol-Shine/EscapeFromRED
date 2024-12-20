﻿#pragma once
#include "JShader.h"

class JSceneComponent;
class JCamera;

class JShader_Basic : public JShader
{
public:
	JShader_Basic(const JText& InName);
	~JShader_Basic() override = default;

public:
	void BindShaderPipeline(ID3D11DeviceContext* InDeviceContext) override;
	void UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext) override;
};
