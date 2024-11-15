﻿#pragma once
#include <DirectXMath.h>

#include "Core/Graphics/graphics_common_include.h"
#include "Core/Interface/ICoreInterface.h"
#include "Core/Input/XKeyboardMouse.h"
#include "Core/Manager/IManagedInterface.h"

/**
 * directx11 sdk example - DXUTcamera 참고
 * https://github.com/microsoft/DXUT/blob/main/Optional/DXUTcamera.h
 */
class JCamera : public IManagedInterface, public ICoreInterface
{
public:
	JCamera() noexcept;
	explicit JCamera(const JText& InName);
	explicit JCamera(const JWText& InName);

public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float DeltaTime) override;
	void Release() override;
#pragma endregion

	uint32_t               GetHash() const override;
	Ptr<IManagedInterface> Clone() const override;

	//---------------------------------------------- Camera ---------------------------------------------------------------
	virtual void Reset();
	/**
	 * View Matrix를 새로 구성 
	 * @param InEyeVec 카메라 위치벡터 
	 * @param InLookAtVec 카메라 타겟벡터
	 */
	virtual void SetViewParams(FXMVECTOR InEyeVec, FXMVECTOR InLookAtVec);
	/**
	 * Projection Matrix를 새로 구성
	 * @param InFOV 시야각 (Field of View)
	 * @param InAspect 종횡비 (가로 / 세로)
	 * @param InNearPlane 카메라 근접 평면 (이 값보다 가까이는 렌더링하지 않음)
	 * @param InFarPlane 카메라 원격 평면  (이 값보다 멀리는 렌더링하지 않음)
	 */
	virtual void SetProjParams(float InFOV, float InAspect, float InNearPlane, float InFarPlane);
	//---------------------------------------------- Camera ---------------------------------------------------------------

	void SetCameraConstantBuffer(uint32_t InSlot);


	//--------------------------------------------- Set State -------------------------------------------------------------
	FORCEINLINE void SetInvertPitch(bool bInInvertPitch) { bInvertPitch = bInInvertPitch; }
	FORCEINLINE void SetEnableYAxisMovement(bool bInEnableYAxisMovement) { bEnableYAxisMovement = bInEnableYAxisMovement; }
	FORCEINLINE void SetEnablePositionMovement(bool bInEnablePositionMovement)
	{
		bEnablePositionMovement = bInEnablePositionMovement;
	}

	FORCEINLINE void SetNumberOfFramesToSmoothMouseData(int InFrame)
	{
		if (InFrame > 0)
			mFramesToSmoothMouseData = static_cast<float>(InFrame);
	}

	FORCEINLINE void SetResetCursorAfterMove(bool bInResetCursorAfterMove)
	{
		bResetCursorAfterMove = bInResetCursorAfterMove;
	}

	FORCEINLINE void SetDrag(bool bInMovementDrag, float InTotalDragTimeToZero = .25f)
	{
		bMovementDrag        = bInMovementDrag;
		mTotalDragTimeToZero = InTotalDragTimeToZero;
	}

	FORCEINLINE void SetClipToBoundary(bool              bInClipToBoundary, _In_opt_ FVector* InMinBoundary,
									   _In_opt_ FVector* InMaxBoundary)
	{
		bClipToBoundary = bInClipToBoundary;
		if (InMinBoundary)
			mMinBoundary = *InMinBoundary;
		if (InMaxBoundary)
			mMaxBoundary = *InMaxBoundary;
	}

	FORCEINLINE void SetValues(float InRotationValue = 0.01f, float InTranslationValue = 5.f)
	{
		mRotationValue    = InRotationValue;
		mTranslationValue = InTranslationValue;
	}

	//--------------------------------------------- Set State -------------------------------------------------------------


	//--------------------------------------------- Get State -------------------------------------------------------------
	[[nodiscard]]FORCEINLINE XMMATRIX GetWorldMatrix() const { return XMLoadFloat4x4(&mWorld); }
	[[nodiscard]]FORCEINLINE XMMATRIX GetViewMatrix() const { return XMLoadFloat4x4(&mView); }
	[[nodiscard]]FORCEINLINE XMMATRIX GetProjMatrix() const { return XMLoadFloat4x4(&mProj); }
	[[nodiscard]]FORCEINLINE XMVECTOR GetEyePosition() const { return XMLoadFloat3(&mEye); }
	[[nodiscard]] FORCEINLINE FVector GetEyePositionFVector() const { return mEye; }
	[[nodiscard]]FORCEINLINE XMVECTOR GetLookAtPosition() const { return XMLoadFloat3(&mLookAt); }
	[[nodiscard]]FORCEINLINE float    GetNearClip() const { return mNearPlane; }
	[[nodiscard]]FORCEINLINE float    GetFarClip() const { return mFarPlane; }

	[[nodiscard]] FORCEINLINE bool IsBeingDragged() const
	{
		return IsKeyPressed(EKeyCode::LButton) || IsKeyPressed(EKeyCode::MButton) || IsKeyPressed(EKeyCode::RButton);
	}

	[[nodiscard]] FORCEINLINE bool IsLMBClicked() const { return IsKeyPressed(EKeyCode::LButton); }
	[[nodiscard]] FORCEINLINE bool IsMMBClicked() const { return IsKeyPressed(EKeyCode::MButton); }
	[[nodiscard]] FORCEINLINE bool IsRMBClicked() const { return IsKeyPressed(EKeyCode::RButton); }
	//--------------------------------------------- Get State -------------------------------------------------------------

protected:
	virtual void UpdateVelocity(float DeltaTime);
	virtual void UpdateRotation(float DeltaTime);
	virtual void UpdateInput();

private:
	FORCEINLINE bool IsKeyPressed(EKeyCode InKey) const { return mInputKeyboard.IsKeyPressed(InKey); }
	FORCEINLINE bool IsKeyDown(EKeyCode InKey) const { return mInputKeyboard.IsKeyDown(InKey); }

protected:
	JWText mName;

	FMatrix mWorld;
	FMatrix mView;
	FMatrix mProj;

	FVector mDefaultEye;	 // 초기 카메라 위치벡터
	FVector mDefaultLookAt; // 초기 카메라 타겟벡터
	FVector mEye;    		 // 현재 카메라 위치벡터
	FVector mLookAt; 		 // 현재 카메라 타겟벡터

	float mYaw;
	float mPitch;

	float mFOV;		  // 카메라 시야
	float mAspect;    // 카메라 종횡비
	float mNearPlane; // 카메라 최소 거리
	float mFarPlane;  // 카메라 최대 거리

	float mFramesToSmoothMouseData;
	float mDragTimer;
	float mTotalDragTimeToZero;

	float mRotationValue;
	float mTranslationValue;

	float mCamSpeed = 1.f;

	FVector mMinBoundary;
	FVector mMaxBoundary;

	FVector  mInputDirection;
	FVector  mVelocity;
	FVector  mDragVelocity;
	FVector2 mMouseDelta;
	FVector2 mRotVelocity;

	XKeyboardMouse mInputKeyboard;

	bool bInvertPitch;
	bool bMovementDrag;
	bool bEnablePositionMovement;
	bool bEnableYAxisMovement;
	bool bClipToBoundary;
	bool bResetCursorAfterMove;

	ComPtr<ID3D11Buffer> mCameraConstantBuffer;

	static uint32_t s_CameraNum;
};
