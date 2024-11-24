// Fill out your copyright notice in the Description page of Project Settings.


#include "Procedural/GeometryControlComponent.h"

UGeometryControlComponent::UGeometryControlComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bCreatedInitialMesh = false;
    StopAsyncTask = false;
    bEndPlay = false;
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));


    
}

void UGeometryControlComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    bEndPlay = true;
    StopAsyncTask = true;
}

void UGeometryControlComponent::Initialize(float _Radius, int32 _RunTimeMaxSubdivsionLevel, int32 _PreComputedSubdivisionLevel)
{
	Radius = _Radius;
	RunTimeMaxSubdivsionLevel = _RunTimeMaxSubdivsionLevel;
	PreComputedSubdivisionLevel = _PreComputedSubdivisionLevel;
	C = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
    SetupQuadTree();
    InitializeCubeMeshData();
	Subdivision1x1CubeToPreComputedSubdivisionLevel();
}


void UGeometryControlComponent::InitializeCubeMeshData()
{
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

	MakedInitial1x1CubeVertices = {
      	{ InitialVertices[4], InitialVertices[5], InitialVertices[6], InitialVertices[7] },
      	{ InitialVertices[1], InitialVertices[2], InitialVertices[6], InitialVertices[5] },   
      	{ InitialVertices[3], InitialVertices[2], InitialVertices[1], InitialVertices[0] },
      	{ InitialVertices[0], InitialVertices[4], InitialVertices[7], InitialVertices[3] },
      	{ InitialVertices[1], InitialVertices[5], InitialVertices[4], InitialVertices[0] },
      	{ InitialVertices[3], InitialVertices[7], InitialVertices[6], InitialVertices[2] }
	};

    TSharedPtr<FQuad> InitialQuads[6];
    for(int8 i = 0; i < 6; ++i)
    {
        InitialQuads[i] = MakeShared<FQuad>();

        InitialQuads[i]->QuadCenter =   (MakedInitial1x1CubeVertices[i][0] + MakedInitial1x1CubeVertices[i][1] + MakedInitial1x1CubeVertices[i][2] + MakedInitial1x1CubeVertices[i][3]) * 0.25f;
        InitialQuads[i]->Quad =         {MakedInitial1x1CubeVertices[i][0], MakedInitial1x1CubeVertices[i][1], MakedInitial1x1CubeVertices[i][2], MakedInitial1x1CubeVertices[i][3]};
        //QuadRoot[i]->PrecomputedNoise = { GetNoise3D(Quad[i][0]), GetNoise3D(Quad[i][1]), GetNoise3D(Quad[i][2]), GetNoise3D(Quad[i][3]) };
        SubdividePannel(*InitialQuads[i], 1);

        for(int8 j = 0; j < 4; ++j)
        {
            UE_LOG(LogTemp, Log, TEXT("%d"), i*4+j);
            QuadRoot[i*4+j] = InitialQuads[i]->Children[j];
        }
    }
    InitialQuads->Reset();
}

void UGeometryControlComponent::InitializeGeometryData()
{
    FScopeLock Lock(&Mutex);
    Vertices.Empty();
    Triangles.Empty();
    VertexMap.Empty();
    DetectJunctionMap.Empty();
}

void UGeometryControlComponent::Subdivision1x1CubeToPreComputedSubdivisionLevel()
{
    static int32 PrecomputedThreadCompleteNum = 0;


    for(int32 i = 0; i < 24; i++)
    {
        FFunctionGraphTask::CreateAndDispatchWhenReady([this, i](){
            SubdividePannel(*QuadRoot[i], PreComputedSubdivisionLevel);

            
            {/*Start Lock*/
                FScopeLock Lock(&Mutex);
                PrecomputedThreadCompleteNum ++;
                UE_LOG(LogTemp, Warning, TEXT("%d Task Complete. CurrentCompleted Tasks:%d"), i, PrecomputedThreadCompleteNum);
                
                if(PrecomputedThreadCompleteNum == 24)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Complete Initial Subdivide"));
                    bCreatedInitialMesh = true;
                    PrecomputedThreadCompleteNum = 0;
                }
            }/*End Lock*/
        }, TStatId(), nullptr, ENamedThreads::AnyBackgroundThreadNormalTask);
    }
}



void UGeometryControlComponent::SetupQuadTree()
{
    for(int32 i = 0; i < 24; ++i)
    {
        QuadRoot[i] = MakeShared<FQuad>();
    }
}

void UGeometryControlComponent::SubdividePannel(FQuad& QuadTree, int32 MaxDepth, int32 CurrentDepth)
{

    if(CurrentDepth >= MaxDepth)
    {
        return;   
    }

    if(StopAsyncTask)
    {
        return;
    }

    if(!&QuadTree)
    {
        check(true);
    }

    FVector V1 = QuadTree.Quad[0]; 
    FVector V2 = QuadTree.Quad[1]; 
    FVector V3 = QuadTree.Quad[2]; 
    FVector V4 = QuadTree.Quad[3]; 

    FVector M1 = (V1 + V2) * 0.5f;
    FVector M2 = (V2 + V3) * 0.5f;
    FVector M3 = (V3 + V4) * 0.5f;
    FVector M4 = (V4 + V1) * 0.5f;
    FVector Center = (V1 + V2 + V3 + V4) * 0.25f;

    FVector Quads[4][4] = {
        {V1, M1, Center, M4},
        {M1, V2, M2, Center},
        {Center, M2, V3, M3},
        {M4, Center, M3, V4}
    };

    for(int32 i = 0; i < 4; ++i)
    {
        {//Start Lock
            FScopeLock Lock(&Mutex);
            QuadTree.Children[i] = MakeShared<FQuad>();
            QuadTree.Children[i]->Parent = &QuadTree;
            QuadTree.Children[i]->Quad = {Quads[i][0], Quads[i][1], Quads[i][2], Quads[i][3]};
            QuadTree.Children[i]->QuadCenter = (Quads[i][0] + Quads[i][1] + Quads[i][2] + Quads[i][3]) * 0.25f;
        // QuadTree.Children[i]->PrecomputedNoise = { GetNoise3D(Quads[i][0]), GetNoise3D(Quads[i][1]), GetNoise3D(Quads[i][2]), GetNoise3D(Quads[i][3]) };
        } //End Lock
        
        SubdividePannel(*QuadTree.Children[i], MaxDepth, CurrentDepth+1);
    }
}

int32 UGeometryControlComponent::AddUniqueVertex(const FVector& Vertex, TMap<FVector, int32>& _VertexMap, TArray<FVector>& OutVertices)
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

void UGeometryControlComponent::AddUniqueJunctionMap(const FVector& Point, TPair<FVector, FTriangles>& _PutInValue, TMap<FVector, TPair<FVector, FTriangles>>& _TJunctionMap)
{
    FScopeLock Lock(&Mutex);
    if (_TJunctionMap.Contains(Point))
    {
        return;
    }
    _TJunctionMap.Add(Point, _PutInValue);
}

FGraphEventRef& UGeometryControlComponent::UpdateLOD()
{
    FVector L = C->GetCameraLocation();
    static FVector PreviousDist = L;

    FConvexVolume Frustum;// = GetCameraFrustum();

    ///카메라 속도에 따른 동적 거리조정 필요
    if(FVector::Distance(L, PreviousDist) > 1000.f)
    {   
        PreviousDist = L;
        SubdivideAsync = FFunctionGraphTask::CreateAndDispatchWhenReady([this, L, Frustum](){
            for(int32 i = 0 ; i < 24; ++i)
            {
                UpdateLODReculsive(*QuadRoot[i], Frustum, L, Vertices, Triangles, RunTimeMaxSubdivsionLevel);
            }
            GetAndFixTJunctionPoints(Vertices, Triangles, DetectJunctionMap);
        }, TStatId(), nullptr, ENamedThreads::BackgroundThreadPriority);    
    }
    return SubdivideAsync;
}

void UGeometryControlComponent::UpdateLODReculsive(FQuad& Quad, FConvexVolume Frustum, FVector CameraLoc, TArray<FVector>& UpdateVertices, FJsonSerializableArrayInt& UpdateTriangles, int32 MaxDepth, int32 CurrentDepth)
{   
    float Dist2Quad = FVector::Dist(CameraLoc, Quad.QuadCenter.GetSafeNormal()*Radius);
    float BaseDistance = Radius*2.5f;
    float Threshold = BaseDistance / FMath::Pow(1.63f, CurrentDepth); //////CurrentDepth Power [[[[1.63]]]]
    if(CurrentDepth < MaxDepth && Dist2Quad < Threshold)
    {
        for(int32 i = 0; i < 4; ++i)
        {
            if(!Quad.Children[i])
            {
                SubdividePannel(Quad, 1);
            }
            UpdateLODReculsive(*Quad.Children[i], Frustum, CameraLoc, UpdateVertices, UpdateTriangles, MaxDepth, CurrentDepth+1);
        }
    }
    else
    { 
        TArray<FVector> SphericVert = {
            Quad.Quad[0].GetSafeNormal() * Radius,
            Quad.Quad[1].GetSafeNormal() * Radius,
            Quad.Quad[2].GetSafeNormal() * Radius,
            Quad.Quad[3].GetSafeNormal() * Radius,
            Quad.QuadCenter.GetSafeNormal() * Radius
        };
        {/*Start Lock*/
            FScopeLock Lock(&Mutex);
            
            // SphericVert[0].Z += Quad.PrecomputedNoise[0];
            // SphericVert[1].Z += Quad.PrecomputedNoise[1];
            // SphericVert[2].Z += Quad.PrecomputedNoise[2];
            // SphericVert[3].Z += Quad.PrecomputedNoise[3];

            int32 TriIdx[5];
            for(int32 i = 0; i < 5; ++i)
            {
                TriIdx[i] = AddUniqueVertex(SphericVert[i], VertexMap, UpdateVertices);
            }
            UpdateTriangles.Add(TriIdx[0]); UpdateTriangles.Add(TriIdx[4]); UpdateTriangles.Add(TriIdx[1]);
            UpdateTriangles.Add(TriIdx[1]); UpdateTriangles.Add(TriIdx[4]); UpdateTriangles.Add(TriIdx[2]);
            UpdateTriangles.Add(TriIdx[2]); UpdateTriangles.Add(TriIdx[4]); UpdateTriangles.Add(TriIdx[3]);
            UpdateTriangles.Add(TriIdx[3]); UpdateTriangles.Add(TriIdx[4]); UpdateTriangles.Add(TriIdx[0]);
        }/*End Lock*/

        for(int32 i = 0; i<4; ++i)
        {
            FVector point = (Quad.Quad[i] + Quad.Quad[(i+1)%4]) * 0.5f;
            FVector SphericPoint = point.GetSafeNormal() * Radius;
            
            int32 currentIdx = UpdateTriangles.Num()-(12 - (i*3));
            FTriangles T= {currentIdx, currentIdx+1, currentIdx+2};
            TPair pair = TPair<FVector, FTriangles>(SphericPoint, T);
            AddUniqueJunctionMap(SphericPoint, pair, DetectJunctionMap);
        }
        return;
    }
    return;
}

void UGeometryControlComponent::GetAndFixTJunctionPoints(TArray<FVector>& _Vertices, TArray<int32>& _Triangles, TMap<FVector, TPair<FVector, FTriangles>>& _DetectJunctionMap)
{
    for(int32 i = 0; i < _Vertices.Num(); ++i)
    {
        if(_DetectJunctionMap.Contains(_Vertices[i]))
        {
            FVector JunctionPoint = _DetectJunctionMap[_Vertices[i]].Get<0>();
            FTriangles JunctionTriangles = _DetectJunctionMap[_Vertices[i]].Get<1>();

            /*Start Lock*/
            FScopeLock Lock(&Mutex);
            {
                int32 TJIdx = AddUniqueVertex(JunctionPoint, VertexMap, _Vertices);
                int32 Idx2 = _Triangles[JunctionTriangles.i2];
                int32 Idx3 = _Triangles[JunctionTriangles.i3];
    
                _Triangles.Add(TJIdx); 
                _Triangles.Add(Idx2); 
                _Triangles.Add(Idx3); 
                _Triangles[JunctionTriangles.i3] = TJIdx;
            }
            /*End Lock*/

        }
    }
}
void UGeometryControlComponent::ClearQuadTree(FQuad* Quad)
{
    if(!Quad) return;

    for (int32 i = 0; i < Quad->Children.Num(); ++i)
    {
        if (Quad->Children[i])
        {
            ClearQuadTree(Quad->Children[i].Get());
        }
    }
}


FConvexVolume UGeometryControlComponent::GetCameraFrustum()
{
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("not exsite PC"));
        return FConvexVolume();
    }

    ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
    if (LocalPlayer && LocalPlayer->ViewportClient)
    {
        FSceneViewFamilyContext ViewFamily(
            FSceneViewFamily::ConstructionValues(
                LocalPlayer->ViewportClient->Viewport,
                GetWorld()->Scene,
                LocalPlayer->ViewportClient->EngineShowFlags
            ).SetRealtimeUpdate(true)
        );

        FVector ViewLocation;
        FRotator ViewRotation;
        PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

        FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation, LocalPlayer->ViewportClient->Viewport);
        if (SceneView)
        {
            return SceneView->ViewFrustum;
        }
        UE_LOG(LogTemp, Warning, TEXT("Empty FConvexVolume"));
    }
    return FConvexVolume(); // 기본 빈 프러스텀 반환
}

