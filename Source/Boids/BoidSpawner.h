#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoidSpawner.generated.h"

class ABoid;

UCLASS()
class BOIDS_API ABoidSpawner : public AActor
{
	GENERATED_BODY()

public:
	ABoidSpawner();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	int32 NumberOfBoids = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float SpawnRadius = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Behavior")
	float BoidSpeed = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Debug")
	bool ShowDebugSeparation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Behavior")
	float MaxSeparationDistance = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Behavior")
	float SeparationStrength = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Debug")
	bool ShowDebugAlignment = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Behavior")
	float MaxAlignmentDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Behavior")
	float AlignmentStrength = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Debug")
	bool ShowDebugCohesion = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Behavior")
	float MaxCohesionDistance = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Behavior")
	float CohesionStrength = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Debug")
	bool ShowDebugFOV = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Behavior", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float ViewAngle = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Behavior")
	float AvoidanceDistance = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Behavior")
	int32 AvoidanceRayCount = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Behavior")
	float AvoidanceStrength = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Debug")
	bool ShowDebugAvoidance = false;

	UPROPERTY()
	TArray<ABoid*> SpawnedBoids;

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void SpawnBoids();

	void UpdateAllBoids(float DeltaTime);
};