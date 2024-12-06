#include "AStar.h"
#include "NavTest.h"
#include "Core/Entity/Actor/AActor.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"

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
            // if (npcGrid.y < NAV_MAP.mGridGraph.size())
            if (NAV_MAP.mGridGraph[playerGrid.y][playerGrid.x]->Walkable)
                FindPath(NAV_MAP.mGridGraph.at(npcGrid.y).at(npcGrid.x),
                NAV_MAP.mGridGraph.at(playerGrid.y).at(playerGrid.x));
        }
        if (mPath.size() > 1)
            FollowPath(DeltaTime);
    }

    G_DebugBatch.PreRender();
    for (auto grid : mPath)
    {
        NAV_MAP.DrawNode(FVector2(grid->GridX, grid->GridY), Colors::Cyan);
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
    mPath.clear();
    Ptr<Node> current = Target;
    while (current != Start)
    {
        mPath.push_back(current);
        current = current->Parent.lock();
    }
    NAV_MAP.tempPath = mPath;
}

void AStar::FollowPath(float DeltaTime)
{
    FVector NextPos = mPath.back()->WorldPos;
    FVector currentPos = GetOwnerActor()->GetWorldLocation();
    FVector direction = NextPos - currentPos + NAV_MAP.NodeCenter;
    if (abs(direction.Length()) < 10)
    {
        if (!IsPosUpdated)
        {
            mPath.pop_back();
            IsPosUpdated = true;
            // std::cout << "nextNode : x = " << NextPos.x << " y = " << NextPos.y <<
            //     "  direction : x = " << direction.x << " z = " << direction.z << std::endl;
        }
    }
    else
    {
        IsPosUpdated = false;
        FVector location = GetOwnerActor()->GetLocalLocation();
        location.x += direction.x / direction.Length() * mSpeed * DeltaTime;
        location.y += direction.y/ direction.Length() * mSpeed * DeltaTime;
        location.z += direction.z / direction.Length() * mSpeed * DeltaTime;
        GetOwnerActor()->SetLocalLocation(location);
    }
}
