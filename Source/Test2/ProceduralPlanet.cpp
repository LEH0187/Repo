#include "ProceduralPlanet.h"
#include "DrawDebugHelpers.h"

// Sets default values
AProceduralPlanet::AProceduralPlanet()
{
    PrimaryActorTick.bCanEverTick = true;

    Radius = 6371000.f;
    bCompleteCreateInitialMesh = false;
    ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
    RootComponent = ProceduralMesh;
    Noise = NewObject<UNoiseGenerator>();
    SetupQuadTree();
    InitializeCubeMeshData();
}

// Called when the game starts or when spawned
void AProceduralPlanet::BeginPlay()
{
    Super::BeginPlay(); 
    C = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
    CreateCubeMesh(Radius, 12);
}

void AProceduralPlanet::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    ProceduralMesh->ClearAllMeshSections();
    Vertices.Empty();
    Triangles.Empty();
    Normals.Empty();
    Tangents.Empty();
    VertexMap.Empty();

    for(int32 i = 0; i < 1; ++i)
    {
        ClearQuadTree(QuadRoot[i].Get());
    }

    Destroy();
}

void AProceduralPlanet::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
    if(bCompleteCreateInitialMesh)
    {
        UpdateLOD();
    }
}

void AProceduralPlanet::CreateCubeMesh(float NewRadius, int32 NewSubdivideLevel)
{
    Radius = NewRadius;

    FVector Quad[6][4] = {
                         { InitialVertices[4], InitialVertices[5], InitialVertices[6], InitialVertices[7] },
                         { InitialVertices[1], InitialVertices[2], InitialVertices[6], InitialVertices[5] },   
                         { InitialVertices[3], InitialVertices[2], InitialVertices[1], InitialVertices[0] },
                         { InitialVertices[0], InitialVertices[4], InitialVertices[7], InitialVertices[3] },
                         { InitialVertices[1], InitialVertices[5], InitialVertices[4], InitialVertices[0] },
                         { InitialVertices[3], InitialVertices[7], InitialVertices[6], InitialVertices[2] }
                    };

    for(int32 i = 0; i < 1; ++i)
    {
        Async(EAsyncExecution::LargeThreadPool, [this, i ,NewSubdivideLevel, Quad]{                
            QuadRoot[i]->QuadCenter =   (Quad[i][0] + Quad[i][1] + Quad[i][2] + Quad[i][3]) * 0.25f;
            QuadRoot[i]->Quad =         {Quad[i][0], Quad[i][1], Quad[i][2], Quad[i][3]};

            FScopeLock Lock(&Mutex);
            SubdividePannel(*QuadRoot[i], NewSubdivideLevel);

            AsyncTask(ENamedThreads::GameThread, [this]{
                static int32 currentCompleteTasks = 0;
                currentCompleteTasks++;
                UE_LOG(LogTemp, Warning, TEXT("Complete Create Cube Mesh / CompleteTasks : %d"), currentCompleteTasks);

                if(currentCompleteTasks >= 1)
                {
                    bCompleteCreateInitialMesh = true;
                    UE_LOG(LogTemp, Warning, TEXT("Complete Create All Cube Meshes"));
                }
            });
        });
    }
}


void AProceduralPlanet::InitializeCubeMeshData()
{
    // 1x1 정육면체의 정점 좌표
    InitialVertices = {
        FVector( -0.5f,  -0.5f,  -0.5f),
        FVector(  0.5f,  -0.5f,  -0.5f),
        FVector(  0.5f,   0.5f,  -0.5f),
        FVector( -0.5f,   0.5f,  -0.5f),
        FVector( -0.5f,  -0.5f,   0.5f),
        FVector(  0.5f,  -0.5f,   0.5f),
        FVector(  0.5f,   0.5f,   0.5f),
        FVector( -0.5f,   0.5f,   0.5f)
    };
}

void AProceduralPlanet::SetupQuadTree()
{
    for(int32 i = 0; i < 6; ++i)
    {
        QuadRoot[i] = MakeUnique<FQuad>();
    }
}

void AProceduralPlanet::SubdividePannel(FQuad& QuadTree, int32 MaxDepth, int32 CurrentDepth)
{
    if(CurrentDepth >= MaxDepth)
    {
        return;   
    }

    FVector V1 = QuadTree.Quad[0] * Radius;
    FVector V2 = QuadTree.Quad[1] * Radius;
    FVector V3 = QuadTree.Quad[2] * Radius;
    FVector V4 = QuadTree.Quad[3] * Radius;

    FVector M1 = (V1 + V2) * 0.5f;
    FVector M2 = (V2 + V3) * 0.5f;
    FVector M3 = (V3 + V4) * 0.5f;
    FVector M4 = (V4 + V1) * 0.5f;
    FVector Center = (V1 + V2 + V3 + V4) * 0.25f;

    QuadTree.Children[0] = MakeUnique<FQuad>();
    QuadTree.Children[0]->Parent = &QuadTree;
    QuadTree.Children[0]->Quad = {V1, M1, Center, M4};
    SubdividePannel(*QuadTree.Children[0], MaxDepth, CurrentDepth+1);

    QuadTree.Children[1] = MakeUnique<FQuad>();
    QuadTree.Children[1]->Parent = &QuadTree;
    QuadTree.Children[1]->Quad = {M1, V2, M2, Center};
    SubdividePannel(*QuadTree.Children[1], MaxDepth, CurrentDepth+1);

    QuadTree.Children[2] = MakeUnique<FQuad>();
    QuadTree.Children[2]->Parent = &QuadTree;
    QuadTree.Children[2]->Quad = {Center, M2, V3, M3};
    SubdividePannel(*QuadTree.Children[2], MaxDepth, CurrentDepth+1);

    QuadTree.Children[3] = MakeUnique<FQuad>();
    QuadTree.Children[3]->Parent = &QuadTree;
    QuadTree.Children[3]->Quad = {M4, Center, M3, V4};
    SubdividePannel(*QuadTree.Children[3], MaxDepth, CurrentDepth+1);
}

int32 AProceduralPlanet::AddUniqueVertex(const FVector& Vertex, TMap<FVector, int32>& _VertexMap, TArray<FVector>& OutVertices)
{
    if (_VertexMap.Contains(Vertex))
    {
        return _VertexMap[Vertex];  // 중복된 정점의 인덱스 반환
    }
    // If vertex does not exist, add it and return its new index
    int32 NewIndex = OutVertices.Add(Vertex);
    _VertexMap.Add(Vertex, NewIndex);
    return NewIndex;
}

void AProceduralPlanet::DrawMesh()
{
    ProceduralMesh->ClearAllMeshSections();
    ProceduralMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, TArray<FLinearColor>(), Tangents, true);
}


void AProceduralPlanet::CalculateNormals(TArray<FVector>& _Vertices, TArray<int32>& _Triangles, TArray<FVector>& _Normals)
{
    _Normals.SetNum(_Vertices.Num());

    // 모든 법선 벡터 초기화
    for (int32 i = 0; i < _Normals.Num(); ++i)
    {
        _Normals[i] = FVector::ZeroVector;
    }

    // 삼각형을 기반으로 법선 _Triangles 계산
    for (int32 i = 0; i < _Triangles.Num(); i += 3)
    {
        FVector V0 = _Vertices[_Triangles[i]];
        FVector V1 = _Vertices[_Triangles[i + 1]];
        FVector V2 = _Vertices[_Triangles[i + 2]];

        // 삼각형 면의 법선 계산
        FVector Normal = FVector::CrossProduct(V2 - V0, V1 - V0).GetSafeNormal();

        // 각 정점에 대해 법선 벡터 추가
        _Normals[_Triangles[i]] += Normal;
        _Normals[_Triangles[i + 1]] += Normal;
        _Normals[_Triangles[i + 2]] += Normal;
    }

    // 정점 법선 벡터를 정규화
    for (int32 i = 0; i < _Normals.Num(); ++i)
    {
        _Normals[i].Normalize();
    }
}

void AProceduralPlanet::CalculateUVs(TArray<FVector>& _Vertices, TArray<FVector2D>& _UVs)
{
    _UVs.SetNum(_Vertices.Num()); // Ensure UVs array is the same size as Vertices

    for (int32 i = 0; i < _Vertices.Num(); ++i)
    {
        // Normalize the vertex to project it onto a unit sphere
        FVector NormalizedVertex = _Vertices[i].GetSafeNormal();

        // Spherical UV mapping (based on longitude and latitude)
        float U = 0.5f + (atan2(NormalizedVertex.X, NormalizedVertex.Y) / (2.0f * PI));
        float V = 0.5f - (asin(NormalizedVertex.Z) / PI);

        _UVs[i] = FVector2D(U, V);
    }
}

void AProceduralPlanet::CalculateTangents(TArray<FVector>& _Vertices, TArray<FProcMeshTangent>& _Tangents)
{
    _Tangents.SetNum(_Vertices.Num());

    for (int32 i = 0; i < _Vertices.Num(); ++i)
    {
        FVector _Normal = _Vertices[i].GetSafeNormal();
        FVector _UpVector = FVector::UpVector;
        FVector _Tangent = FVector::CrossProduct(_UpVector, _Normal).GetSafeNormal();
        _Tangents[i] = FProcMeshTangent(_Tangent, false);
    }
}

void AProceduralPlanet::UpdateLOD()
{
    FVector L = C->GetCameraLocation();
    static FVector PreviousDist = L;

    Vertices.Empty();
    Triangles.Empty();
    VertexMap.Empty();

#if DEBUG == 1
    UE_LOG(LogTemp, Warning, TEXT("Dist Camera to SquareCenter %f"), Dist2Player);
#endif // DEBUG
    
    if(FVector::Distance(L, PreviousDist) > 1000.f)
    {
        PreviousDist = L;

        for(int32 i = 0 ; i < 1; ++i)
        {
            Async(EAsyncExecution::LargeThreadPool, [this, i, L]{
                FScopeLock Lock(&Mutex);
                UpdateLODReculsive(*QuadRoot[i], L, this->Vertices, this->Triangles, 6);

                
                AsyncTask(ENamedThreads::GameThread, [this]{
                    static int32 currentCompleteTasks = 0;
                    currentCompleteTasks++;
                    if(currentCompleteTasks >= 1)
                    {
                        //UE_LOG(LogTemp, Log, TEXT("DrawMesh Vertices:%d Triangles:%d"), Vertices.Num(), Triangles.Num());
                        DrawMesh();
                    } 
                });
            });
        }
    }
    
}

void AProceduralPlanet::UpdateLODReculsive(FQuad& Quad, FVector CameraLoc, TArray<FVector>& UpdateVertices, FJsonSerializableArrayInt& UpdateTriangles, int32 MaxDepth, int32 CurrentDepth)
{
    float Dist2Quad = FVector::Dist(CameraLoc, Quad.QuadCenter);
    float BaseDistance = 6371000.0f *3;
    float Threshold = BaseDistance / FMath::Pow(1.63f, CurrentDepth); //////CurrentDepth Power [[[[1.63]]]]
    //UE_LOG(LogTemp, Log, TEXT("Dist2Quad:%f / Threshold:%f"), Dist2Quad, Threshold);
    if(CurrentDepth < MaxDepth && Dist2Quad < Threshold)
    {
        for(int32 i = 0; i < 4; ++i)
        {
            if(!Quad.Children[i])
            {
                SubdividePannel(Quad, 1);
            }
            UpdateLODReculsive(*Quad.Children[i], CameraLoc, UpdateVertices, UpdateTriangles, MaxDepth, CurrentDepth+1);
        }
    }
    else
    {
        int32 TriIdx0 = AddUniqueVertex(Quad.Quad[0].GetSafeNormal() * Radius, VertexMap, UpdateVertices);
        int32 TriIdx1 = AddUniqueVertex(Quad.Quad[1].GetSafeNormal() * Radius, VertexMap, UpdateVertices);
        int32 TriIdx2 = AddUniqueVertex(Quad.Quad[2].GetSafeNormal() * Radius, VertexMap, UpdateVertices);
        int32 TriIdx3 = AddUniqueVertex(Quad.Quad[3].GetSafeNormal() * Radius, VertexMap, UpdateVertices);

        UpdateTriangles.Add(TriIdx0); UpdateTriangles.Add(TriIdx2); UpdateTriangles.Add(TriIdx1);
        UpdateTriangles.Add(TriIdx0); UpdateTriangles.Add(TriIdx3); UpdateTriangles.Add(TriIdx2);
        return;
    }
}

void AProceduralPlanet::ClearQuadTree(FQuad* Quad)
{
    for (int32 i = 0; i < 4; ++i)
    {
        if (Quad->Children[i])
        {
            ClearQuadTree(Quad->Children[i].Get());
            Quad->Children[i].Reset();
        }
    }
}