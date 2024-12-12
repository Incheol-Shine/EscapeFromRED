#include "AStar.h"

#include <fbxsdk/core/fbxsystemunit.h>

#include "NavTest.h"
#include "Core/Utils/Math/MathUtility.h"
#include "Core/Entity/Actor/AActor.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Interface/JWorld.h"

#include "Core/Utils/Math/Vector2.h"

void AStar::Initialize()
{
}

void AStar::BeginPlay()
{
}

void AStar::Tick(float DeltaTime)
{
    mInputKeyboard.Update(DeltaTime);
    PushHold = false;
    if (IsKeyPressed(EKeyCode::Space))
    {
        PushHold = !PushHold;
    }
    if (PushHold)
    {
        FVector2 playerGrid = NAV_MAP.GridFromWorldPoint(NAV_MAP.PlayerPos);
        
        if (abs(playerGrid.x - NAV_MAP.GridFromWorldPoint(NewPlayerPos).x) >= 1 ||
            abs(playerGrid.y - NAV_MAP.GridFromWorldPoint(NewPlayerPos).y) >= 1)
        {
            NewPlayerPos = NAV_MAP.PlayerPos;
            FVector2 npcGrid = NAV_MAP.GridFromWorldPoint(GetOwnerActor()->GetWorldLocation());
            if (NAV_MAP.mGridGraph[playerGrid.y][playerGrid.x]->Walkable)
            {
                FindPath(NAV_MAP.mGridGraph.at(npcGrid.y).at(npcGrid.x),
                NAV_MAP.mGridGraph.at(playerGrid.y).at(playerGrid.x));
                mSpeed = FMath::GenerateRandomFloat(300, 800);
            }
        }
        if (mPathIdx < mPath->lookPoints.size())
            FollowPath(DeltaTime);
        else
        {
            FVector rotation = GetOwnerActor()->GetLocalRotation();
            rotation.y += DeltaTime * mRotateSpeed * 50;
            GetOwnerActor()->SetLocalRotation(rotation);
        }
    }

    auto* cam = GetWorld.CameraManager->GetCurrentMainCam();
    G_DebugBatch.PreRender(cam->GetViewMatrix(), cam->GetProjMatrix());
    if (mPath)
    {int flag = 0;
        for (auto grid : mPath->lookPoints)
        {
            if (flag == 0)
            {
                NAV_MAP.DrawNode(FVector2(grid->GridX, grid->GridY), Colors::Yellow);
                flag = 1;
            }
            else
                NAV_MAP.DrawNode(FVector2(grid->GridX, grid->GridY), Colors::Cyan);
        }
        for (auto line : mPath->turnBoundaries)
        {
            G_DebugBatch.DrawRay_Implement(FVector(line->pointOnLine_1.x, 0.5, line->pointOnLine_1.y),
                100 * FVector(line->pointOnLine_2.x - line->pointOnLine_1.x, 0.005, line->pointOnLine_2.y - line->pointOnLine_1.y),
                false,
                Colors::HotPink);   
        }
    }
    G_DebugBatch.PostRender();
}

void AStar::Destroy()
{
}

void AStar::FindPath(Ptr<Node> Start, Ptr<Node> Target)
{
    // PriorityQueue openSet;
    std::vector<Ptr<Node>> openSet;
    UnOrdSet closedSet;
    openSet.push_back(Start);
    std::push_heap(openSet.begin(), openSet.end(), CompareNode());

    while (!openSet.empty())
    {
        Ptr<Node> current = openSet.front();
        std::pop_heap(openSet.begin(), openSet.end(), CompareNode());
        openSet.pop_back();
        closedSet.insert(current);
        if (current == Target)
        {
            RetracePath(Start, Target);
            return;
        }
        for (auto child : current->Children)
        {
            if (closedSet.find(child.lock()) != closedSet.end())
                continue;
            int newMoveCostToChild = current->GCost + GetDistance(current, child.lock());
            if (newMoveCostToChild < child.lock()->GCost)
            {
                child.lock()->GCost = newMoveCostToChild;
                child.lock()->HCost = GetDistance(current, Target);
                child.lock()->Parent = current;
            }
            if (std::find(openSet.begin(), openSet.end(), child.lock()) == openSet.end())
            {
                child.lock()->GCost = newMoveCostToChild;
                child.lock()->HCost = GetDistance(current, Target);
                child.lock()->Parent = current;
                openSet.push_back(child.lock());
                std::push_heap(openSet.begin(), openSet.end(), CompareNode());
            }
        }
    }
}

int AStar::GetDistance(Ptr<Node> A, Ptr<Node> B)
{
    int dstX = abs(A->GridX - B->GridX);
    int dstY = abs(A->GridY - B->GridY);
    if (dstX > dstY)
        return 14 * dstY + 10 * (dstX - dstY);
    return 14 * dstX + 10 * (dstY - dstX);
}

void AStar::RetracePath(Ptr<Node> Start, Ptr<Node> Target)
{
    std::vector<Ptr<NAV::Node>> TempPath;
    Ptr<Node> current = Target;
    while (current != Start)
    {
        TempPath.push_back(current);
        current = current->Parent.lock();
    }
    std::reverse(TempPath.begin(), TempPath.end());
    NAV_MAP.tempPath = TempPath;
    mPath = MakePtr<Path>(TempPath, Start->WorldPos, turnDst);
    mPathIdx = 0;
}

void AStar::FollowPath(float DeltaTime)
{
    FVector NextPos = mPath->lookPoints.at(mPathIdx)->WorldPos;
    FVector currentPos = GetOwnerActor()->GetWorldLocation();
    FVector direction = FVector(NextPos.x - currentPos.x, NextPos.y - currentPos.y, NextPos.z - currentPos.z);
    if (mPath->turnBoundaries.at(mPathIdx)->HasCrossedLine(Path::V3ToV2(currentPos)))
    {
        mPathIdx++;
        IsPosUpdated = true;
    }
    else
    {
        IsPosUpdated = false;
        FVector location = GetOwnerActor()->GetLocalLocation();
        FVector NormalDirection;
        direction.Normalize(NormalDirection);
        
        float theta = atan2(NormalDirection.x, NormalDirection.z);
        float degree = theta * (180.0f / M_PI);
        
        FVector rotation = GetOwnerActor()->GetLocalRotation();
        float targetAngle = degree + 180.0f;
        float angleDifference = targetAngle - rotation.y;

        // 각도 차이를 -180도에서 180도 사이로 정규화
        if (angleDifference > 180.0f)
            angleDifference -= 360.0f;
        else if (angleDifference < -180.0f)
            angleDifference += 360.0f;

        if (angleDifference > 0)
            rotation.y += DeltaTime * mRotateSpeed * abs(angleDifference);
        else
            rotation.y -= DeltaTime * mRotateSpeed * abs(angleDifference); // 반시계 방향 회전

        // 각도를 0도에서 360도 사이로 정규화
        if (rotation.y >= 360.0f)
            rotation.y -= 360.0f;
        else if (rotation.y < 0.0f)
            rotation.y += 360.0f;
        
        GetOwnerActor()->SetLocalRotation(rotation);

        float rotationRadians = rotation.y * M_PI / 180.0f;
        float target_x = -sin(rotationRadians) * mSpeed * DeltaTime;
        float target_z = -cos(rotationRadians) * mSpeed * DeltaTime;


        auto* cam = GetWorld.CameraManager->GetCurrentMainCam();
        G_DebugBatch.PreRender(cam->GetViewMatrix(), cam->GetProjMatrix());
        G_DebugBatch.DrawRay_Implement(location, 100 * FVector(target_x, 0, target_z)
            , false, Colors::BlueViolet);
        G_DebugBatch.PostRender();
        
        location = FVector(location.x + target_x, 50, location.z + target_z);
        GetOwnerActor()->SetLocalLocation(location);
    }
}
