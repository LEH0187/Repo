#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProcMeshSurfacePropertyComponent.h"
#include "GeometryControlComponent.h"
#include "ProceduralPlanet.generated.h"

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
    void DrawMesh();
    void UpdateProcMeshSurface();

private:
    UProcMeshSurfacePropertyComponent*  SurfacePropertyComp;
    UGeometryControlComponent* GeometryControl;

private:
    UProceduralMeshComponent* ProcComp;
    FCriticalSection Mutex;

public:
    UPROPERTY(EditAnywhere)
    int32 RunTimeMaxSubdivsionLevel;
    UPROPERTY(EditAnywhere)
    int32 PreComputedSubdivisionLevel;

};
