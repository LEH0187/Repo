#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "NoiseGenerator.h"
#include "FrustumCulling.h"
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
    

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProceduralPlanetProperty")
    float Radius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProceduralPlanetProperty")
    int32 RunTimeMaxSubdivsionLevel;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProceduralPlanetProperty")
    int32 PreComputedSubdivisionLevel;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProceduralPlanetProperty")
    int32 RenderFaces;

protected:    

    void SetupQuadTree();
    void ClearQuadTree(FQuad* Quad);

    void SubdividePannel(FQuad& QuadTree, int32 MaxDepth, int32 CurrentDepth = 0);
    float getMorphValue(float dist, float low, float high);
    int32 AddUniqueVertex(const FVector& Vertex, TMap<FVector, int32>& VertexMap, TArray<FVector>& OutVertices);
    void AddUniqueJunctionMap(const FVector& Point, TPair<FVector,FTriangles>& _PutInValue, TMap<FVector, TPair<FVector, FTriangles>>& _TJunctionMap);
    void DrawMesh();
    
    void CalculateNormals(TArray<FVector>& _Vertices, TArray<int32>& _Triangles, TArray<FVector>& _Normals);
    void CalculateUVs(TArray<FVector>& _Vertices, TArray<FVector2D>& UVs);
    void CalculateTangents(TArray<FVector>& _Vertices, TArray<FProcMeshTangent>& _Tangents);
    float GetNoise3D(FVector _Point);

    void UpdateLOD();
    void UpdateLODReculsive(FQuad& Quad, FConvexVolume Frustum, FVector CameraLoc, TArray<FVector>& UpdateVertices, 
                                FJsonSerializableArrayInt& UpdateTriangles, int32 MaxDepth, int32 CurrentDepth = 0);

    void GetAndFixTJunctionPoints(TArray<FVector>& _Vertices, TArray<int32>& _Triangles, TMap<FVector, TPair<FVector, FTriangles>>& _DetectJunctionMap);


    FConvexVolume GetCameraFrustum();
private:
    UProceduralMeshComponent*           ProceduralMesh;
    UNoiseGenerator*                    Noise;
    TMap<FVector, int32>                VertexMap;
    TSharedPtr<FQuad>                   QuadRoot[6];

    APlayerCameraManager*               C;
    FFrustumCulling                     FrustumCulling;
    bool                                bCompleteCreateInitialMesh;

    FCriticalSection                    Mutex;

    TArray<FVector>                     InitialVertices;
    TArray<FVector>                     Vertices;
    FJsonSerializableArrayInt           Triangles;
    TArray<FVector>                     Normals;
    TArray<FVector2D>                   UVs;
    TArray<FProcMeshTangent>            Tangents;

    TMap<FVector, TPair<FVector, FTriangles>> DetectJunctionMap;             

    int32 PrecomputedThreadCompleteNum;
};
