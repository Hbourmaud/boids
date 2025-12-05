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

	ApplyBoidBehaviors();

	const FVector CurrentPosition = GetActorLocation();
	const FVector NewPosition = CurrentPosition + (Direction * Speed * DeltaTime);

	SetActorLocation(NewPosition);
	SetActorRotation(Direction.Rotation());
}

void ABoid::ApplyBoidBehaviors()
{
	if (!Spawner || AllBoids.Num() == 0)
	{
		return;
	}

	const FVector BoidPosition = GetActorLocation();

	const float MaxSeparationDist = Spawner->MaxSeparationDistance;
	const float MaxAlignmentDist = Spawner->MaxAlignmentDistance;
	const float MaxCohesionDist = Spawner->MaxCohesionDistance;

	const float SeparationStrength = Spawner->SeparationStrength;
	const float AlignmentStrength = Spawner->AlignmentStrength;
	const float CohesionStrength = Spawner->CohesionStrength;

	FVector SeparationDirection = Direction;
	FVector AlignmentDirection = FVector::ZeroVector;
	FVector Centroid = FVector::ZeroVector;

	int32 SeparationCount = 0;
	int32 AlignmentCount = 0;
	int32 CohesionCount = 0;

	for (ABoid* OtherBoid : AllBoids)
	{
		if (OtherBoid == this || !OtherBoid)
		{
			continue;
		}

		const FVector OtherPosition = OtherBoid->GetActorLocation();
		const FVector DifferenceVector = BoidPosition - OtherPosition;
		const float Distance = DifferenceVector.Size();

		const bool IsInFOV = IsInFieldOfView(OtherPosition);

		// SEPARATION
		if (IsInFOV && Distance < MaxSeparationDist && Distance > 0.0f)
		{
			const float Ratio = 1.0f - (Distance / MaxSeparationDist);
			const FVector OtherDirection = DifferenceVector.GetSafeNormal();

			SeparationDirection += OtherDirection * Ratio * SeparationStrength;
			SeparationCount++;

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

		// ALIGNMENT
		if (IsInFOV && Distance < MaxAlignmentDist)
		{
			AlignmentDirection += OtherBoid->Direction;
			AlignmentCount++;

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

		// COHESION
		if (IsInFOV && Distance < MaxCohesionDist)
		{
			Centroid += OtherPosition;
			CohesionCount++;

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

	FVector NewDirection = Direction;

	if (CohesionCount > 0)
	{
		Centroid /= CohesionCount;

		const FVector CohesionDirection = (Centroid - BoidPosition).GetSafeNormal();

		NewDirection += CohesionDirection * CohesionStrength;

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

	if (AlignmentCount > 0)
	{
		AlignmentDirection = AlignmentDirection.GetSafeNormal();

		NewDirection += AlignmentDirection * AlignmentStrength;

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

	if (SeparationCount > 0)
	{
		SeparationDirection = SeparationDirection.GetSafeNormal();
		NewDirection += SeparationDirection * SeparationStrength;
	}

	Direction = NewDirection.GetSafeNormal();

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

bool ABoid::IsInFieldOfView(const FVector& OtherPosition) const
{
	if (!Spawner)
	{
		return true; // needed
	}

	const FVector BoidPosition = GetActorLocation();
	const FVector BoidForward = Direction;
	const FVector DistanceToOther = (OtherPosition - BoidPosition).GetSafeNormal();

	const float DotProduct = FVector::DotProduct(BoidForward, DistanceToOther); // to rename

	const float AngleRadians = FMath::Acos(DotProduct); // to rename
	const float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);

	const bool IsInView = AngleDegrees <= Spawner->ViewAngle; // refacto ?

	if (Spawner->ShowDebugFOV && !IsInView)
	{
		DrawDebugLine(
			GetWorld(),
			BoidPosition,
			OtherPosition,
			FColor::Orange,
			false,
			-1.0f,
			0,
			1.0f
		);
	}

	return IsInView;
}