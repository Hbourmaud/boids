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

	virtual void Tick(const float DeltaTime) override;

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

	void ApplyBoidBehaviors();

	bool IsInFieldOfView(const FVector& OtherPosition) const;
};