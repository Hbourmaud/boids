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

	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	int32 NumberOfBoids = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float SpawnRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Behavior")
	float BoidSpeed = 100.0f;

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

	UPROPERTY()
	TArray<ABoid*> SpawnedBoids;

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void SpawnBoids();
};