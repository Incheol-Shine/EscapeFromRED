﻿#pragma once
#include <DirectXColors.h>

#include "Core/Utils/Math/Vector.h"


class JMeshObject;

struct FRay
{
    FVector Origin = FVector::ZeroVector; // 광선의 시작점
    FVector Direction{0, 0, 1}; // 광선의 방향
    float Length = 1000.f;

    void DrawDebug(DirectX::GXMVECTOR InColor = DirectX::Colors::BlueViolet);
};

struct FPlane
{
    FVector Normal; // 평면의 법선 벡터
    float Distance; // 원점으로부터 평면까지의 거리

    float A, B, C, D; // Ax + By + Cz + D = 0 (평면의 방정식)

    void CreatePlane(FVector InNormal, FVector InPoint);
    void CreatePlane(FVector InPoint0, FVector InPoint1, FVector InPoint2);
    void Normalize();
};

struct FBox
{
    FVector Center = FVector::ZeroVector; // 박스 중심좌표
    FVector Extent = FVector(50.f, 50.f, 50.f); // 박스 반지름

    FVector LocalAxis[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; // 박스 로컬 축
    FVector Position[8]; // 박스의 8개의 꼭지점

    FBox() = default;

    FBox(const FVector& InCenter, const FVector& InExtent)
        : Center(InCenter),
          Extent(InExtent)
    {
    }

    bool Contains(const FVector& InPoint) const;
};

struct FBoxShape
{
    // ------ Common ------
    FBox Box;

    // ------- OBB --------
    FVector Min = FVector(FLT_MAX, FLT_MAX, FLT_MAX);
    FVector Max = FVector(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    void DrawDebug() const;
    void DrawDebug(DirectX::GXMVECTOR InVector);

    bool Intersect(const FBoxShape& InBox) const;
    bool IntersectOBB(const FBoxShape& InBox) const;
    bool IntersectOBB(const FBoxShape& InBox,  FVector& CollisionNormal, float& CollistionDepth) const;
    bool Contains(const FVector& InPoint) const;

    FBoxShape() = default;

    FBoxShape(const FVector& InCenter, const FVector& InExtent)
    {
        Box.Center = InCenter;
        Box.Extent = InExtent;

        Min = Box.Center - Box.Extent;
        Max = Box.Center + Box.Extent;
    }
};

struct FSphere
{
    FVector Center; // 구의 중심
    float Radius; // 구의 반지름

    FSphere() = default;

    FSphere(const FVector& InCenter, float InRadius)
        : Center(InCenter),
          Radius(InRadius)
    {
    }

    // 구와 구의 충돌 검사
    bool Intersect(const FSphere& InSphere) const;

    // 구와 점의 충돌 검사
    bool ContainsPoint(const FVector& Point) const;
};

struct FCapsule
{
    FVector Center = FVector::ZeroVector; // 캡슐의 중심
    float Radius = 50.f; // 캡슐의 반지름 (둘레)
    float Height = 100.f; // 캡슐의 높이 (끝점 사이의 거리, 반구 부분 제외)

    FCapsule() = default;

    FCapsule(const FVector& InCenter, float InRadius, float InHeight)
        : Center(InCenter),
          Radius(InRadius),
          Height(InHeight)
    {
    }

    // 두 캡슐의 충돌 여부 검사
    bool Intersect(const FCapsule& Other) const;

    // 캡슐과 점 간 거리 계산
    float DistanceToPoint(const FVector& Point) const;

    // 디버그용 캡슐 그리기
    void DrawDebug() const;
};

static bool RayIntersectAABB(const FRay& InRay, const FBoxShape& InBox, float& OutT);
static bool RayIntersectOBB(const FRay& InRay, const FBoxShape& InBox, FVector& OutT);
static bool RayIntersectOBB(const FVector& RayOrigin, const FVector& RayDir, // Ray 정보
                            const FVector& BoxCenter, const FVector BoxAxis[3], const FVector& BoxExtent, // OBB 정보
                            FVector& OutHitPoint // 충돌 지점
);

// class JShape : public JSceneComponent
// {
// public:
// 	void CreateOBBBox(const FVector& InCenter, const FVector& InExtent, const FVector InAxisX, const FVector InAxisY,
// 					  const FVector  InAxisZ);
// 	void CreateAABBBox(const FVector& InMin, const FVector& InMax);
//
// 	virtual void GenCollisionData() = 0;
// };
//
// class JBoxComponent : public JShape
// {
// public:
// 	void Initialize() override;
// 	void Tick(float DeltaTime) override;
//
// public:
// 	void SetExtent(const FVector& InExtent) { mBoxShape.Box.Extent = InExtent; }
// 	void SetCenter(const FVector& InCenter) { mBoxShape.Box.Center = InCenter; }
//
// 	[[nodiscard]] const FBoxShape& GetBoxShape() const { return mBoxShape; }
//
// private:
// 	FBoxShape mBoxShape;
// };
