#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/ScopeLock.h"
#include "Engine/World.h"
#include "NoiseGenerator.h"
#include "ProceduralPlanet.generated.h"

// 0 : No Debug
// 1 : Dist Camera to Square
// 2 : Vertices Num
#define DEBUG 0
//#define LOD_MESH_RENDER

UCLASS()
class TEST2_API AProceduralPlanet : public AActor
{
    GENERATED_BODY()

public:
    AProceduralPlanet();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
    virtual void Tick(float deltaTime) override;    

    UFUNCTION(BlueprintCallable)
    void CreateCubeMesh(float NewRadius, int32 NewSubdivideLevel);

    UFUNCTION(BlueprintCallable)
    void InitializeCubeMeshData();


public:
    struct FQuad
    {
        TArray<FVector> Quad;
        FJsonSerializableArrayInt Triangles;
        TArray<TUniquePtr<FQuad>> Children;
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

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Radius;

protected:    

    void SetupQuadTree();
    void ClearQuadTree(FQuad* Quad);
    void SubdividePannel(FQuad& QuadTree, int32 MaxDepth, int32 CurrentDepth = 0);
    int32 AddUniqueVertex(const FVector& Vertex, TMap<FVector, int32>& VertexMap, TArray<FVector>& OutVertices);
    void DrawMesh();
    ////Mesh section config
    void CalculateNormals(TArray<FVector>& _Vertices, TArray<int32>& _Triangles, TArray<FVector>& _Normals);
    void CalculateUVs(TArray<FVector>& _Vertices, TArray<FVector2D>& UVs);
    void CalculateTangents(TArray<FVector>& _Vertices, TArray<FProcMeshTangent>& _Tangents);

    void UpdateLOD();
    void UpdateLODReculsive(FQuad& Quad, FVector CameraLoc, TArray<FVector>& UpdateVertices, 
                                FJsonSerializableArrayInt& UpdateTriangles, int32 MaxDepth, int32 CurrentDepth = 0);
private:

    UPROPERTY(VisibleAnywhere)
    UProceduralMeshComponent*           ProceduralMesh;
    UNoiseGenerator*                    Noise;
    TMap<FVector, int32>                VertexMap;
    TUniquePtr<FQuad>                   QuadRoot[6];

    APlayerCameraManager*               C;
    bool                                bCompleteCreateInitialMesh;

    FCriticalSection                    Mutex;

    TArray<FVector>                     InitialVertices;
    TArray<FVector>                     Vertices;
    FJsonSerializableArrayInt           Triangles;
    TArray<FVector>                     Normals;
    TArray<FVector2D>                   UVs;
    TArray<FProcMeshTangent>            Tangents;
};
