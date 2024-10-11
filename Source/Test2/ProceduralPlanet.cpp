#include "ProceduralPlanet.h"
#include "DrawDebugHelpers.h"

// Sets default values
AProceduralPlanet::AProceduralPlanet()
{
    PrimaryActorTick.bCanEverTick = true;

    Radius = 6371000.f;
    bCompleteCreateInitialMesh = false;
    PrecomputedThreadCompleteNum = 0;
    RuntimeThreadCompleteNum = 0;

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
    CreateCubeMesh(Radius, PreComputedSubdivisionLevel);
}

void AProceduralPlanet::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    C = nullptr;
    ProceduralMesh->ClearAllMeshSections();
    Vertices.Empty();
    Triangles.Empty();
    Normals.Empty();
    Tangents.Empty();
    //VertexMap.Empty();

    for(int32 i = 0; i < RenderFaces; ++i)
    {
        ClearQuadTree(QuadRoot[i].Get());
    }

    ProceduralMesh->DestroyComponent();
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

    for(int32 i = 0; i < RenderFaces; ++i)
    {
        Async(EAsyncExecution::LargeThreadPool, [this, i ,NewSubdivideLevel, Quad]{     
            FScopeLock Lock(&Mutex);           
            QuadRoot[i]->QuadCenter =   (Quad[i][0] + Quad[i][1] + Quad[i][2] + Quad[i][3]) * 0.25f;
            QuadRoot[i]->Quad =         {Quad[i][0], Quad[i][1], Quad[i][2], Quad[i][3]};

            SubdividePannel(*QuadRoot[i], NewSubdivideLevel);

            AsyncTask(ENamedThreads::GameThread, [this]{
                FScopeLock Lock(&Mutex);    
                PrecomputedThreadCompleteNum++;
                UE_LOG(LogTemp, Warning, TEXT("Complete Create Cube Mesh / CompleteTasks : %d"), PrecomputedThreadCompleteNum);

                if(PrecomputedThreadCompleteNum >= 6)
                {
                    PrecomputedThreadCompleteNum = 0;
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
        FVector( -0.5f,  -0.5f,  -0.5f) * Radius,
        FVector(  0.5f,  -0.5f,  -0.5f) * Radius,
        FVector(  0.5f,   0.5f,  -0.5f) * Radius,
        FVector( -0.5f,   0.5f,  -0.5f) * Radius,
        FVector( -0.5f,  -0.5f,   0.5f) * Radius,
        FVector(  0.5f,  -0.5f,   0.5f) * Radius,
        FVector(  0.5f,   0.5f,   0.5f) * Radius,
        FVector( -0.5f,   0.5f,   0.5f) * Radius
    };
}

void AProceduralPlanet::SetupQuadTree()
{
    for(int32 i = 0; i < 6; ++i)
    {
        QuadRoot[i] = MakeShared<FQuad>();
    }
}

void AProceduralPlanet::SubdividePannel(FQuad& QuadTree, int32 MaxDepth, int32 CurrentDepth)
{

    if(CurrentDepth >= MaxDepth)
    {
        return;   
    }
    
    FVector V1; 
    FVector V2; 
    FVector V3; 
    FVector V4; 
    {/*Start Lock*/
        FScopeLock Lock(&Mutex);
        V1 = QuadTree.Quad[0];
        V2 = QuadTree.Quad[1];
        V3 = QuadTree.Quad[2];
        V4 = QuadTree.Quad[3];
    }/*End Lock*/

    FVector M1 = (V1 + V2) * 0.5f;
    FVector M2 = (V2 + V3) * 0.5f;
    FVector M3 = (V3 + V4) * 0.5f;
    FVector M4 = (V4 + V1) * 0.5f;
    FVector Center = (V1 + V2 + V3 + V4) * 0.25f;

    {/*Start Lock*/
        FScopeLock Lock(&Mutex);
        QuadTree.Children[0] = MakeShared<FQuad>();
        QuadTree.Children[0]->Parent = &QuadTree;
        QuadTree.Children[0]->Quad = {V1, M1, Center, M4};
        QuadTree.Children[0]->QuadCenter = ((V1+M1+Center+M4) * 0.25f);
        SubdividePannel(*QuadTree.Children[0], MaxDepth, CurrentDepth+1);

        QuadTree.Children[1] = MakeShared<FQuad>();
        QuadTree.Children[1]->Parent = &QuadTree;
        QuadTree.Children[1]->Quad = {M1, V2, M2, Center};
        QuadTree.Children[1]->QuadCenter = ((M1+V2+M2+Center) * 0.25f);
        SubdividePannel(*QuadTree.Children[1], MaxDepth, CurrentDepth+1);
        
        QuadTree.Children[2] = MakeShared<FQuad>();
        QuadTree.Children[2]->Parent = &QuadTree;
        QuadTree.Children[2]->Quad = {Center, M2, V3, M3};
        QuadTree.Children[2]->QuadCenter = ((Center+ M2+ V3+ M3) * 0.25f);
        SubdividePannel(*QuadTree.Children[2], MaxDepth, CurrentDepth+1);

        QuadTree.Children[3] = MakeShared<FQuad>();
        QuadTree.Children[3]->Parent = &QuadTree;
        QuadTree.Children[3]->Quad = {M4, Center, M3, V4};
        QuadTree.Children[3]->QuadCenter = ((M4+ Center+ M3+ V4) * 0.25f);
        SubdividePannel(*QuadTree.Children[3], MaxDepth, CurrentDepth+1);
    }/*End Lock*/
}

int32 AProceduralPlanet::AddUniqueVertex(const FVector& Vertex, TMap<FVector, int32>& _VertexMap, TArray<FVector>& OutVertices)
{
    /*Start Lock*/
    FScopeLock Lock(&Mutex);
    
    if (_VertexMap.Contains(Vertex))
    {
        return _VertexMap[Vertex];  // 중복된 정점의 인덱스 반환
    }
    // If vertex does not exist, add it and return its new index
    int32 NewIndex = OutVertices.Add(Vertex);
    _VertexMap.Add(Vertex, NewIndex);

    /*End Lock*/
    return NewIndex;
}

void AProceduralPlanet::DrawMesh()
{
    FScopeLock Lock(&Mutex);

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

#if DEBUG == 1
    UE_LOG(LogTemp, Warning, TEXT("Dist Camera to SquareCenter %f"), Dist2Player);
#endif // DEBUG
    
    if(FVector::Distance(L, PreviousDist) > 1000.f)
    {    
        PreviousDist = L;
        TMap<int32, int32> TJunctionPointsMap;
        for(int32 i = 0 ; i < RenderFaces; ++i)
        {
            Async(EAsyncExecution::LargeThreadPool, [this, i, L, &TJunctionPointsMap]{

                UpdateLODReculsive(*QuadRoot[i], L, this->Vertices, this->Triangles, RunTimeMaxSubdivsionLevel);
                GetTJunctionPoints(this->Vertices, this->Triangles, TJunctionPointsMap);
                MoveVerticesSquareLocationToSphereLocation(this->Vertices);
                AsyncTask(ENamedThreads::GameThread, [this]{
                    FScopeLock Lock(&Mutex);
                    RuntimeThreadCompleteNum++;
                    if(RuntimeThreadCompleteNum >= 6)
                    {
                        DrawMesh();
                        RuntimeThreadCompleteNum = 0;
                        Vertices.Empty();
                        Triangles.Empty();
                        VertexMap.Empty();
                    } 
                });
            });
        }
    }
    
}

void AProceduralPlanet::UpdateLODReculsive(FQuad& Quad, FVector CameraLoc, TArray<FVector>& UpdateVertices, FJsonSerializableArrayInt& UpdateTriangles, int32 MaxDepth, int32 CurrentDepth)
{
    float Dist2Quad;
    {
        FScopeLock Lock(&Mutex);
        Dist2Quad = FVector::Dist(CameraLoc, Quad.QuadCenter.GetSafeNormal()*Radius);
    }

    float BaseDistance = Radius*2;
    float Threshold = BaseDistance / FMath::Pow(1.63f, CurrentDepth); //////CurrentDepth Power [[[[1.63]]]]
    if(CurrentDepth < MaxDepth && Dist2Quad < Threshold)
    {
        for(int32 i = 0; i < 4; ++i)
        {
            {/*Start Lock*/
                FScopeLock Lock(&Mutex);
                if(!Quad.Children[i])
                {
                    SubdividePannel(Quad, 1);
                }
            }/*End Lock*/
            UpdateLODReculsive(*Quad.Children[i], CameraLoc, UpdateVertices, UpdateTriangles, MaxDepth, CurrentDepth+1);
        }
    }
    else
    { 
        {/*Start Lock*/
            FScopeLock Lock(&Mutex);

            FVector SphericVert[4]  = {
                Quad.Quad[0],
                Quad.Quad[1],
                Quad.Quad[2],
                Quad.Quad[3],
            };
            int32 TriIdx0 = AddUniqueVertex(SphericVert[0], VertexMap, UpdateVertices);
            int32 TriIdx1 = AddUniqueVertex(SphericVert[1], VertexMap, UpdateVertices);
            int32 TriIdx2 = AddUniqueVertex(SphericVert[2], VertexMap, UpdateVertices);
            int32 TriIdx3 = AddUniqueVertex(SphericVert[3], VertexMap, UpdateVertices);

            UpdateTriangles.Add(TriIdx2); UpdateTriangles.Add(TriIdx1); UpdateTriangles.Add(TriIdx0);
            UpdateTriangles.Add(TriIdx0); UpdateTriangles.Add(TriIdx3); UpdateTriangles.Add(TriIdx2);
        }/*End Lock*/
        return;
    }
    return;
}

void AProceduralPlanet::GetTJunctionPoints(TArray<FVector>& _Vertices, FJsonSerializableArrayInt& _Triangles, TMap<int32, int32>& TJunctionPointsMap)
{
    /*Start Lock*/
    FScopeLock Lock(&Mutex);
    TMap<int32, int32> CrackVert;
    for(int32 i = 0; i < _Triangles.Num()-3; i+=3)
    {
        FVector SphericVert[3]  = {
                _Vertices[_Triangles[i]],
                _Vertices[_Triangles[i+2]],
                _Vertices[_Triangles[i+3]]
            };

        if(VertexMap.Contains((SphericVert[0]+SphericVert[1])*0.5f))
        {
            CrackVert.Add(_Triangles[0], _Triangles[1]);
        }
        if(VertexMap.Contains((SphericVert[1]+SphericVert[2])*0.5f))
        {   
            CrackVert.Add(_Triangles[1], _Triangles[2]);
        }
    }
    CrackVert = MoveTemp(TJunctionPointsMap);
    /*End Lock*/
}

void AProceduralPlanet::InterpolateTJuncionPoints(TArray<FVector>& _Vertices, FJsonSerializableArrayInt& _Triangles, TMap<int32, int32>& TJunctionPointsMap)
{
    
}

void AProceduralPlanet::MoveVerticesSquareLocationToSphereLocation(TArray<FVector>& _Vertices)
{
    for(int32 i = 0; i < Vertices.Num();++i)
    {
        Vertices[i] = Vertices[i].GetSafeNormal() * Radius;
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
            Quad->Parent = nullptr;
        }
    }
}

float AProceduralPlanet::getMorphValue(float dist, float low, float high)
{
    float delta = high - low;             // LOD 범위 차이
    float factor = (dist - low) / delta;  // 현재 거리가 LOD 범위 내 어디에 위치하는지 비율 계산
    return FMath::Clamp(factor / 0.5f - 1.0f, 0.0f, 1.0f);  // 0.0 ~ 1.0 사이 값으로 변환
}
