﻿#pragma once

#include "GUI_Base.h"

class JCamera;

class GUI_Viewport : public GUI_Base
{
public:
	explicit GUI_Viewport(const std::string& InTitle);
	~GUI_Viewport();

public:
	void Initialize() override;
	void Update(float DeltaTime) override;

public:
	[[nodiscard]] FORCEINLINE bool IsBIsFocused() const { return bIsFocused; }
	[[nodiscard]] FORCEINLINE bool IsBIsHovered() const { return bIsHovered; }

protected:
	bool bIsFocused;
	bool bIsHovered;

	JCamera* mEditorCameraRef;
};
