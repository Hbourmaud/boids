#pragma once

#include "CoreMinimal.h"
#include "BoidSpawner.h"
#include "Boid.generated.h"

UCLASS()
class ABoid : public AActor {
	GENERATED_BODY()

public:
	ABoid();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	UStaticMeshComponent* Mesh;

	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	float Speed = 100.0f;

	UPROPERTY()
	ABoidSpawner* Spawner;

	UPROPERTY()
	FVector Direction;

	UPROPERTY()
	TArray<ABoid*> AllBoids;

	void CalculateBoidBehaviors();

	void ApplyMovement(float DeltaTime);

private:
	FVector NextDirection;

	bool IsInFieldOfView(const FVector& OtherPosition) const;
	
	TArray<FVector> GenerateGoldenSpherePoints(int32 NumPoints) const;
	FVector CalculateObjectAvoidance();

	TArray<ABoid*> GetNearbyBoids() const;
};