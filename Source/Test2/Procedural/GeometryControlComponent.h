	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"
#include "Test2/NoiseGenerator.h"
#include "Test2/FrustumCulling.h"
#include "GeometryControlComponent.generated.h"


struct FQuad
{
    TArray<FVector> Quad;
    TArray<float> PrecomputedNoise;
    TArray<TSharedPtr<FQuad>> Children;
    FQuad* Parent;
    FVector QuadCenter;

    FQuad()
    {
        Quad.SetNum(4);
        Children.SetNum(4);
        Parent = nullptr;
        QuadCenter = FVector::ZeroVector;
    }
};

struct FTriangles
{
    int32 i1;
    int32 i2;
    int32 i3;
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST2_API UGeometryControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGeometryControlComponent();

protected:
    virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:
	void Initialize(float Radius, int32 RunTimeMaxSubdivsionLevel, int32 PreComputedSubdivisionLevel);
    void InitializeCubeMeshData();
    void InitializeGeometryData();
	bool& UpdateLOD();


	TArray<FVector> GetVertices() const { return Vertices; }
	FJsonSerializableArrayInt GetTriangles() const {return Triangles; }
	bool IsReadyInitialMesh() const { return bCreatedInitialMesh; }


private:
    void SetupQuadTree();
    void ClearQuadTree(FQuad* Quad);

    void Subdivision1x1CubeToPreComputedSubdivisionLevel();

    void SubdividePannel(FQuad& QuadTree, int32 MaxDepth, int32 CurrentDepth = 0);

    int32 AddUniqueVertex(const FVector& Vertex, TMap<FVector, int32>& VertexMap, TArray<FVector>& OutVertices);
    
    void AddUniqueJunctionMap(const FVector& Point, TPair<FVector,FTriangles>& _PutInValue, TMap<FVector, TPair<FVector, FTriangles>>& _TJunctionMap);

    void UpdateLODReculsive(FQuad& Quad, FConvexVolume Frustum, FVector CameraLoc, TArray<FVector>& UpdateVertices, 
                                FJsonSerializableArrayInt& UpdateTriangles, int32 MaxDepth, int32 CurrentDepth = 0);

    void GetAndFixTJunctionPoints(TArray<FVector>& _Vertices, TArray<int32>& _Triangles, TMap<FVector, TPair<FVector, FTriangles>>& _DetectJunctionMap);

public:
    

private:
    UProceduralMeshComponent*           ProceduralMesh;
    UNoiseGenerator*                    Noise;
    TMap<FVector, int32>                VertexMap;
    TSharedPtr<FQuad>                   QuadRoot[24];

    APlayerCameraManager*               C;
    FFrustumCulling*                    FrustumCulling;
    bool                                bCreatedInitialMesh;
    bool                                bCompleteUpdateLODReculsiveAll;

    bool                                bEndPlay;

    FCriticalSection                    Mutex;
    TArray<FGraphEventRef>              SubdivideAsync;
    TAtomic<bool>                       StopAsyncTask;
    

    TMap<FVector, TPair<FVector, FTriangles>> DetectJunctionMap;     

private:
    float Radius;
    int32 RunTimeMaxSubdivsionLevel;
    int32 PreComputedSubdivisionLevel;
	TArray<FVector> Vertices;
	FJsonSerializableArrayInt Triangles;


private:
	///Geometry Data
	TArray<FVector> InitialVertices;
	TArray<TArray<FVector>> MakedInitial1x1CubeVertices;
};
