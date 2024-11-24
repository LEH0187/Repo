#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProcMeshSurfacePropertyComponent.h"
#include "GeometryControlComponent.h"
#include "ProceduralPlanet.generated.h"

struct FMeshDrawProperties
{
    TArray<FVector> Verties                 = TArray<FVector>();
    FJsonSerializableArrayInt Triangles     = FJsonSerializableArrayInt();
    TArray<FVector> Normals                 = TArray<FVector>();
    TArray<FVector2D> UVs                   = TArray<FVector2D>();
    TArray<FProcMeshTangent> Tangents       = TArray<FProcMeshTangent>();
};

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

protected:    
    void DrawMesh(FMeshDrawProperties MeshProperties);
    void UpdateProcMeshSurface();

private:
    UProcMeshSurfacePropertyComponent*  SurfacePropertyComp;
    UGeometryControlComponent* GeometryControl;

private:
    UProceduralMeshComponent* ProcComp;
    FCriticalSection Mutex;

};
