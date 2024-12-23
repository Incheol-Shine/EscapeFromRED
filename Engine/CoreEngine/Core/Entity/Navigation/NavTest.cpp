#include "NavTest.h"

#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Level/MLevelManager.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Entity/Component/AI/BT/BT_BOSS.h"
// #include "Core/Utils/Math/MathUtility.h"

void NavTest::Initialize()
{
    LoadMapFile("rsc\\GameResource\\GridMap\\GridMap.png");
    NodeRadius = 50.0f;
    NodeCenter = FVector(NodeRadius, 0, - NodeRadius);
    GridDivs = FVector2(200, 200);
    GridCenter = FVector(0, 101, 0); // x, y, z
    
    NodeDiameter = NodeRadius * 2.0f;
    GridWorldSize = FVector2(GridDivs.x * NodeDiameter, GridDivs.y * NodeDiameter);
    NodeScale = FVector2(GridDivs.x * NodeRadius, GridDivs.y * NodeRadius); // col, row
    GridTopLeft = FVector(GridCenter.x - GridWorldSize.x/2,
                            GridCenter.y,
                            GridCenter.z + GridWorldSize.y/2);
    ObstacleScale = 50 / NodeRadius;

    SubGridDivs = FVector2(50, 50);

    
    for (int row = 0; row < GridDivs.y; row++)
    {
        for (int col = 0; col < GridDivs.x; col++)
        {
            if (MapFile->mRGBAData[row * GridDivs.x + col] > 200)
                // mGridGraph[row][col]->Walkable = false;
                Obstacles.push_back(FVector2(col, row));

        }
    }
    SetGraph();
    // FVector2 InitPlayerGrid = GridFromWorldPoint(PlayerPos);
    // CreateDynamicMapper(mGridGraph, InitPlayerGrid.y, InitPlayerGrid.x);
}

void NavTest::Update(float DeltaTime)
{
    currentFrame++;
    for(auto& actor : GetWorld.LevelManager->GetActiveLevel()->mActors)
    {
        /*if (firstRun && actor.get()->GetName().starts_with("S"))
        {
            FVector worldPos = actor.get()->GetWorldLocation();
            FVector2 targetGrid = FVector2(GridFromWorldPoint(worldPos).x, GridFromWorldPoint(worldPos).y);
            for (int row = -ObstacleScale; row <= ObstacleScale; row++)
            {
                for (int col = -ObstacleScale; col <= ObstacleScale; col++)
                {
                    Obstacles.push_back(FVector2(targetGrid.x + col, targetGrid.y + row));
                }
            }
        }
        else */
        if (actor.get()->GetName() == "Test Player")
        {
            PlayerPos = actor.get()->GetWorldLocation();
        }
        else if (firstRun && actor.get()->GetName().starts_with("SK_BigZombie"))
        {
            static int32_t enemyNum = 0;
            JText enemyCompName = std::format("Enemy_{}", enemyNum++);
            actor.get()->CreateDefaultSubObject<BT_BOSS>(enemyCompName, enemyNum -1);
        }

        JSceneComponent* boxCollider = actor.get()->GetChildSceneComponentByName("123123");
        if (firstRun && boxCollider)
        {
        	JBoxComponent* collider = static_cast<JBoxComponent*>(boxCollider);
            collider->IsNPC = true;
            ColliderTarget.insert(collider);
        }

        JSceneComponent* planeCollider = actor.get()->GetChildSceneComponentByName("BoxCollider");
        if (firstRun && planeCollider)
        {
            JBoxComponent* collider = static_cast<JBoxComponent*>(planeCollider);
            GroundColliders.insert(collider);
        }
        
        if (firstRun && actor.get()->GetName().starts_with("Cone"))
        {
            static int32_t rayNum = 0;
            JText rayCompName = std::format("ray_{}", rayNum++);
            actor.get()->CreateDefaultSubObject<SimpleRayComponent>(rayCompName, actor.get(), actor.get());
            SimpleRayComponent* ray = static_cast<SimpleRayComponent*>(actor.get()->GetComponentByName("ray_0") );
            RayCollider.insert(ray);
        }
    }
    if (firstRun)
    {
        firstRun = false;
        SetObstacle();
    }
}

void NavTest::Render()
{
    auto* cam = GetWorld.CameraManager->GetCurrentMainCam();
    G_DebugBatch.PreRender(cam->GetViewMatrix(), cam->GetProjMatrix());
    {
        G_DebugBatch.DrawGrid_Implement(
            {NodeScale.x, 0, 0},
            {0, 0, NodeScale.y},
            GridCenter,
            GridDivs.x,
            GridDivs.y,
            Colors::Gray
        );
        DrawNode(GridFromWorldPoint(PlayerPos), Colors::Cyan);
        DrawUnWalkable();
        // for (auto grid : tempPath)
        // {
        //     DrawNode(FVector2(grid->GridX, grid->GridY), Colors::Cyan);
        // }
        
        /*DrawNode(GridFromWorldPoint(WorldPosFromGridPos(0, 1)), Colors::Pink);
        DrawNode(GridFromWorldPoint(WorldPosFromGridPos(0, 2)), Colors::Pink);
        DrawNode(GridFromWorldPoint(WorldPosFromGridPos(1, 5)), Colors::Pink);
        
        DrawNode(mGridGraph[0][0]->GridPos, Colors::Pink);
        DrawNode(mGridGraph[0][1]->GridPos, Colors::Pink);
        DrawNode(mGridGraph[0][2]->GridPos, Colors::Pink);*/
    }
    G_DebugBatch.PostRender();
}

void NavTest::Release()
{
}

void NavTest::DrawNode(FVector2 grid, FXMVECTOR InColor)
{
    G_DebugBatch.DrawQuad_Implement(
        // tl - tr - br - bl
        {GridTopLeft.x + grid.x * NodeDiameter, 101.1, GridTopLeft.z - grid.y * NodeDiameter},
        {GridTopLeft.x + (grid.x + 1) * NodeDiameter, 101.1, GridTopLeft.z - grid.y * NodeDiameter},
        {GridTopLeft.x + (grid.x + 1) * NodeDiameter, 101.1, GridTopLeft.z - (grid.y + 1) * NodeDiameter},
        {GridTopLeft.x + grid.x * NodeDiameter, 101.1, GridTopLeft.z - (grid.y + 1) * NodeDiameter},
        InColor
    );
}

FVector NavTest::WorldPosFromGridPos(int col, int row)
{
    float x = GridTopLeft.x + col * NodeDiameter + NodeCenter.x;
    float z = (GridTopLeft.z - row * NodeDiameter + NodeCenter.z);
    return FVector(x, GridTopLeft.y, z);
}

FVector NavTest::WorldPosFromGridPos(FVector2 GridPos)
{
    int col = GridPos.x;
    int row = GridPos.y;
    return WorldPosFromGridPos(col, row);
}

FVector2 NavTest::GridFromWorldPoint(FVector worldPos)
{
    int gridX = floor((worldPos.x - GridTopLeft.x) / NodeDiameter);
    int gridY = -ceil((worldPos.z - GridTopLeft.z) / NodeDiameter);

    gridX = FMath::Clamp(gridX, 0, GridDivs.x - 1);
    gridY = FMath::Clamp(gridY, 0, GridDivs.y - 1);
    
    return FVector2(gridX, gridY);
}

void NavTest::SetGraph()
{
    mGridGraph.resize(GridDivs.y);
    for (int row = 0; row < GridDivs.y; row++)
    {
        mGridGraph[row].resize(GridDivs.x);
        for (int col = 0; col < GridDivs.x; col++)
        {
            mGridGraph[row][col] = MakePtr<Node>(true, WorldPosFromGridPos(col, row), col, row);
        }
    }
}

void NavTest::SetObstacle()
{
    for (auto node : Obstacles) // node : col, row
    {
        if (node.x >= 0 && node.x < GridDivs.x && node.y >= 0 && node.y < GridDivs.y)
           mGridGraph[node.y][node.x]->Walkable = false;
    }
    for (int row = 0; row < GridDivs.y; row++)
    {
        for (int col = 0; col < GridDivs.x; col++)
        {
            SetChildNode(row, col);
        }
    }
}

void NavTest::SetChildNode(int row, int col)
{
    // if (mGridGraph[row][col]->Walkable == false)
    //     return;
    int ChildIdx[8][2] = {
            {-1, -1}, {-1, 0}, {-1, 1},
            {0, -1},  {0, 1},
            {1, -1}, {1, 0}, {1, 1}
        };
    for (auto node : ChildIdx)
    {
        if (row + node[0] >= 0 && row + node[0] < GridDivs.y && col + node[1] >= 0 && col + node[1] < GridDivs.x )
        {
            if (mGridGraph[row + node[0]][col + node[1]]->Walkable)
                mGridGraph[row][col]->Children.push_back(mGridGraph[row + node[0]][col + node[1]]);
        }
    }
}

void NavTest::DrawUnWalkable()
{
    for (auto location : Obstacles)
    {
        DrawNode(location, Colors::Red);
    }
}

void NavTest::LoadMapFile(const JText& FileName)
{
    // MapFile = GetWorld.TextureManager->Load(FileName);
    MapFile = new JTexture(FileName, true);
    // MapFile = MakePtr<JTexture>(FileName, true).get();
}

// void NavTest::CreateDynamicMapper(const std::vector<std::vector<Ptr<Node>>>& mGridGraph, int playerRow, int playerCol)
// {
//     int startRow = FMath::Max(0, playerRow - static_cast<int>(SubGridDivs.y) / 2);
//     int startCol = FMath::Max(0, playerCol - static_cast<int>(SubGridDivs.x) / 2);
//
//     int endRow = startRow + static_cast<int>(SubGridDivs.y);
//     int endCol = startCol + static_cast<int>(SubGridDivs.x);
//
//     // 동적 매핑 함수 정의
//     GetNodeInSubGrid = [&mGridGraph, startRow, startCol, endRow, endCol](int row, int col) -> Node* {
//         int globalRow = startRow + row;
//         int globalCol = startCol + col;
//
//         // **범위 검사 추가**
//         if (globalRow < startRow || globalRow >= endRow || globalCol < startCol || globalCol >= endCol)
//             return nullptr; // 범위 밖이면 nullptr 반환
//
//         return mGridGraph[globalRow][globalCol].get();
//     };
// }

