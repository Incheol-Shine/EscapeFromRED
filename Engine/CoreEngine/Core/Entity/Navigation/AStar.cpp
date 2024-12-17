#include "AStar.h"

#include <fbxsdk/core/fbxsystemunit.h>

#include "NavTest.h"
#include "Core/Utils/Math/MathUtility.h"
#include "Core/Entity/Actor/AActor.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Interface/JWorld.h"

#include "Core/Utils/Math/Vector2.h"

AStar::AStar()
{
    
}

AStar::~AStar()
{
}

void AStar::FindPath(Ptr<Node> Start, Ptr<Node> Target, float Weight)
{
    Nav::Node::weight = Weight;
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
    std::vector<Ptr<Nav::Node>> TempPath;
    Ptr<Node> current = Target;
    while (current != Start)
    {
        TempPath.push_back(current);
        current = current->Parent.lock();
    }
    std::reverse(TempPath.begin(), TempPath.end());
    mPath = MakePtr<Path>(simplifyPath(TempPath), Start->WorldPos, TurnDst);
    // mPath = MakePtr<Path>(TempPath, Start->WorldPos, TurnDst);
    mPathIdx = 1;
}

std::vector<Ptr<Node>> AStar::simplifyPath(const std::vector<Ptr<Node>>& path)
{
    if (path.empty())
        return path;
    std::vector<Ptr<Node>> simplifiedPath;
    simplifiedPath.push_back(path[0]); // 시작점

    for (size_t i = 1; i < path.size() - 1; ++i)
    {
        FVector2 prev = simplifiedPath.back()->GridPos;
        FVector2 next = path[i + 1]->GridPos;
        if (!IsLineBlocked(prev, next)) { // prev와 next 사이에 장애물이 없으면
            continue; // 중간 점 스킵
        }
        simplifiedPath.push_back(path[i]); // 필요하면 추가
    }
    simplifiedPath.push_back(path.back()); // 끝점
    return simplifiedPath;
}

bool AStar::IsLineBlocked(FVector2 prevGrid, FVector2 nextGrid) // grid.x = col, grid.y = row
{
    int x0 = prevGrid.x, y0 = prevGrid.y;
    int x1 = nextGrid.x, y1 = nextGrid.y;
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        // 현재 격자 셀이 장애물이 있는지 검사
        if (NAV_MAP.mGridGraph.at(y0).at(x0)->Walkable == false)
        {
            obstacle = FVector2(x0, y0);
            return true; // 장애물 발견
        }
        if (x0 == x1 && y0 == y1) break; // 목표 지점 도달

        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
    return false; // 직선 경로에 장애물 없음
}
