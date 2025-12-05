#include "Boid.h"

ABoid::ABoid()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetEnableGravity(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMeshAsset(TEXT("/Engine/BasicShapes/Cone"));
	if (ConeMeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(ConeMeshAsset.Object);
		Mesh->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	}
}

void ABoid::BeginPlay()
{
	Super::BeginPlay();

	Direction = GetActorForwardVector();
}

void ABoid::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	ApplyCohesion();
	ApplyAlignment();
	ApplySeparation();

	const FVector CurrentPosition = GetActorLocation();
	const FVector NewPosition = CurrentPosition + (Direction * Speed * DeltaTime);

	SetActorLocation(NewPosition);
	SetActorRotation(Direction.Rotation());
}

void ABoid::ApplySeparation()
{
	if (!Spawner || AllBoids.Num() == 0)
	{
		return;
	}

	const FVector BoidPosition = GetActorLocation();
	FVector SeparationDirection = Direction;
	const float MaxDistance = Spawner->MaxSeparationDistance;
	const float Strength = Spawner->SeparationStrength;

	for (ABoid* OtherBoid : AllBoids)
	{
		if (OtherBoid == this || !OtherBoid)
		{
			continue;
		}

		const FVector OtherPosition = OtherBoid->GetActorLocation();
		const FVector DifferenceVector = BoidPosition - OtherPosition;
		const float Distance = DifferenceVector.Size();

		if (Distance < MaxDistance && Distance > 0.0f)
		{
			const float Ratio = 1.0f - (Distance / MaxDistance);
			const FVector OtherDirection = DifferenceVector.GetSafeNormal();

			SeparationDirection += OtherDirection * Ratio * Strength;

			if (Spawner->ShowDebugSeparation) {
				DrawDebugLine(
					GetWorld(),
					BoidPosition,
					OtherPosition,
					FColor::Red,
					false,
					-1.0f,
					0,
					2.0f
				);
			}
		}
	}

	Direction = SeparationDirection.GetSafeNormal();

	if (Spawner->ShowDebugSeparation) {
		DrawDebugLine(
			GetWorld(),
			BoidPosition,
			BoidPosition + Direction * 100.0f,
			FColor::Green,
			false,
			-1.0f,
			0,
			3.0f
		);
	}
}

void ABoid::ApplyAlignment()
{
	if (!Spawner || AllBoids.Num() == 0)
	{
		return;
	}

	const FVector BoidPosition = GetActorLocation();
	const float MaxDistance = Spawner->MaxAlignmentDistance;
	const float Strength = Spawner->AlignmentStrength;

	FVector AlignmentDirection = FVector::ZeroVector;
	int32 NeighbourCount = 0;

	for (ABoid* OtherBoid : AllBoids)
	{
		if (OtherBoid == this || !OtherBoid)
		{
			continue;
		}

		const FVector OtherPosition = OtherBoid->GetActorLocation();
		const float Distance = FVector::Dist(BoidPosition, OtherPosition);

		if (Distance < MaxDistance)
		{
			AlignmentDirection += OtherBoid->Direction;
			NeighbourCount++;

			if (Spawner->ShowDebugAlignment) {
				DrawDebugLine(
					GetWorld(),
					BoidPosition,
					OtherPosition,
					FColor::Blue,
					false,
					-1.0f,
					0, 1.0f
				);
			}
		}
	}

	if (NeighbourCount > 0)
	{
		AlignmentDirection = AlignmentDirection.GetSafeNormal();

		Direction = (Direction + AlignmentDirection * Strength).GetSafeNormal();

		if (Spawner->ShowDebugAlignment) {
			DrawDebugLine(
				GetWorld(),
				BoidPosition,
				BoidPosition + AlignmentDirection * 80.0f,
				FColor::Yellow,
				false,
				-1.0f,
				0, 2.0f
			);
		}
	}
}

void ABoid::ApplyCohesion()
{
	if (!Spawner || AllBoids.Num() == 0)
	{
		return;
	}

	const FVector BoidPosition = GetActorLocation();
	const float MaxDistance = Spawner->MaxCohesionDistance;
	const float Strength = Spawner->CohesionStrength;

	FVector Centroid = FVector::ZeroVector;
	int32 NeighbourCount = 0;

	for (ABoid* OtherBoid : AllBoids)
	{
		if (OtherBoid == this || !OtherBoid)
		{
			continue;
		}

		const FVector OtherPosition = OtherBoid->GetActorLocation();
		const float Distance = FVector::Dist(BoidPosition, OtherPosition);

		if (Distance < MaxDistance)
		{
			Centroid += OtherPosition;
			NeighbourCount++;

			if (Spawner->ShowDebugCohesion) {
				DrawDebugLine(
					GetWorld(),
					BoidPosition,
					OtherPosition,
					FColor::Cyan,
					false,
					-1.0f,
					0,
					1.0f
				);
			}
		}
	}

	if (NeighbourCount > 0)
	{
		Centroid /= NeighbourCount;

		const FVector CohesionDirection = (Centroid - BoidPosition).GetSafeNormal();

		Direction = (Direction + CohesionDirection * Strength).GetSafeNormal();

		if (Spawner->ShowDebugCohesion) {
		DrawDebugLine(
			GetWorld(),
			BoidPosition,
			Centroid,
			FColor::Magenta,
			false,
			-1.0f,
			0, 2.0f
		);
		DrawDebugSphere(
			GetWorld(),
			Centroid,
			10.0f,
			8,
			FColor::Magenta,
			false,
			-1.0f,
			0,
			2.0f
		);
		}
	}
}