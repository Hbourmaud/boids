#include "Boid.h"

ABoid::ABoid() {
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetEnableGravity(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMeshAsset(TEXT("/Engine/BasicShapes/Cone"));
	if (ConeMeshAsset.Succeeded()) {
		Mesh->SetStaticMesh(ConeMeshAsset.Object);
		Mesh->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	}
}

void ABoid::BeginPlay() {
	Super::BeginPlay();

	Direction = GetActorForwardVector();
	NextDirection = Direction;
}

TArray<ABoid*> ABoid::GetNearbyBoids() const {
	if (!Spawner) {
		return TArray<ABoid*>();
	}

	FSpatialGrid* SpatialGrid = Spawner->GetSpatialGrid();
	if (SpatialGrid) {
		const float MaxRange = FMath::Max3(
			Spawner->MaxSeparationDistance,
			Spawner->MaxAlignmentDistance,
			Spawner->MaxCohesionDistance
		);

		return SpatialGrid->GetNearbyBoids(GetActorLocation(), MaxRange);
	}

	return TArray<ABoid*>();
}

void ABoid::CalculateBoidBehaviors() {
	if (!Spawner) {
		NextDirection = Direction;
		return;
	}

	const FVector BoidPosition = GetActorLocation();

	const float MaxSeparationDistSq = Spawner->MaxSeparationDistance * Spawner->MaxSeparationDistance;
	const float MaxAlignmentDistSq = Spawner->MaxAlignmentDistance * Spawner->MaxAlignmentDistance;
	const float MaxCohesionDistSq = Spawner->MaxCohesionDistance * Spawner->MaxCohesionDistance;

	const float SeparationStrength = Spawner->SeparationStrength;
	const float AlignmentStrength = Spawner->AlignmentStrength;
	const float CohesionStrength = Spawner->CohesionStrength;

	FVector SeparationDirection = FVector::ZeroVector;
	FVector AlignmentDirection = FVector::ZeroVector;
	FVector Centroid = FVector::ZeroVector;

	int32 SeparationCount = 0;
	int32 AlignmentCount = 0;
	int32 CohesionCount = 0;

	const TArray<ABoid*> NearbyBoids = GetNearbyBoids();

	for (const ABoid* OtherBoid : NearbyBoids) {
		if (OtherBoid == this || !OtherBoid) {
			continue;
		}

		const FVector OtherPosition = OtherBoid->GetActorLocation();
		const FVector DifferenceVector = BoidPosition - OtherPosition;
		const float DistanceSq = DifferenceVector.SizeSquared();

		if (DistanceSq > MaxCohesionDistSq) {
			continue;
		}

		const bool IsInFOV = IsInFieldOfView(OtherPosition);

		// SEPARATION
		if (IsInFOV && DistanceSq < MaxSeparationDistSq && DistanceSq > SMALL_NUMBER) {
			const float Distance = FMath::Sqrt(DistanceSq);
			const float Ratio = 1.0f - (Distance / Spawner->MaxSeparationDistance);
			const FVector OtherDirection = DifferenceVector / Distance;

			SeparationDirection += OtherDirection * Ratio;
			SeparationCount++;
		}

		// ALIGNMENT
		if (IsInFOV && DistanceSq < MaxAlignmentDistSq) {
			AlignmentDirection += OtherBoid->Direction;
			AlignmentCount++;
		}

		// COHESION
		if (IsInFOV && DistanceSq < MaxCohesionDistSq) {
			Centroid += OtherPosition;
			CohesionCount++;
		}
	}

	FVector NewDirection = Direction;

	if (CohesionCount > 0) {
		Centroid /= CohesionCount;

		const FVector CohesionDirection = (Centroid - BoidPosition).GetSafeNormal();

		NewDirection += CohesionDirection * CohesionStrength;
	}

	if (AlignmentCount > 0)	{
		AlignmentDirection = AlignmentDirection.GetSafeNormal();

		NewDirection += AlignmentDirection * AlignmentStrength;
	}

	if (SeparationCount > 0) {
		SeparationDirection = SeparationDirection.GetSafeNormal();
		NewDirection += SeparationDirection * SeparationStrength;
	}

	NewDirection += CalculateObjectAvoidance();

	NextDirection = NewDirection.GetSafeNormal();
}

void ABoid::ApplyMovement(float DeltaTime) {
	Direction = NextDirection;

	const FVector CurrentPosition = GetActorLocation();
	const FVector NewPosition = CurrentPosition + (Direction * Speed * DeltaTime);

	SetActorLocation(NewPosition);
	SetActorRotation(Direction.Rotation());
}

FVector ABoid::CalculateObjectAvoidance() {
	if (!Spawner) {
		return FVector::ZeroVector;
	}

	const FVector BoidPosition = GetActorLocation();
	const float AvoidanceDistance = Spawner->AvoidanceDistance;
	const int32 RayCount = Spawner->AvoidanceRayCount;
	const float AvoidanceStrength = Spawner->AvoidanceStrength;

	TArray<FVector> RayDirections = GenerateGoldenSpherePoints(RayCount);

	FVector AvoidanceVector = FVector::ZeroVector;
	int32 HitCount = 0;

	for (const FVector& LocalDirection : RayDirections)	{
		const FRotator BoidRotation = Direction.Rotation();
		const FVector WorldDirection = BoidRotation.RotateVector(LocalDirection);
		const FVector RayStart = BoidPosition;
		const FVector RayEnd = BoidPosition + (WorldDirection * AvoidanceDistance);

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		const TArray<ABoid*> NearbyBoids = GetNearbyBoids();

		for (ABoid* OtherBoid : NearbyBoids) {
			if (OtherBoid && OtherBoid != this) {
				QueryParams.AddIgnoredActor(OtherBoid);
			}
		}

		const bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			RayStart,
			RayEnd,
			ECC_Visibility,
			QueryParams
		);

		if (bHit) {
			const FVector ToHit = (HitResult.ImpactPoint - BoidPosition).GetSafeNormal();
			const FVector AvoidDirecttion = -ToHit;
			const float HitDistance = FVector::Dist(BoidPosition, HitResult.ImpactPoint);
			const float Weight = 1.0f - (HitDistance / AvoidanceDistance);

			AvoidanceVector += AvoidDirecttion * Weight;
			HitCount++;
		}
	}

	if (HitCount > 0) {
		AvoidanceVector = AvoidanceVector.GetSafeNormal();

		return (AvoidanceVector * AvoidanceStrength).GetSafeNormal();
	}

	return FVector::ZeroVector;
}

TArray<FVector> ABoid::GenerateGoldenSpherePoints(int32 NumPoints) const {
	TArray<FVector> Points;
	Points.Reserve(NumPoints);

	const float GoldenRatio = (1.0f + FMath::Sqrt(5.0f)) / 2.0f;

	for (int32 i = 0; i < NumPoints; i++) {
		const float NormalizedIndex = static_cast<float>(i) / static_cast<float>(NumPoints);
		const float CosTheta = 1.0f - 2.0f * NormalizedIndex;
		const float SinTheta = FMath::Sqrt(1.0f - CosTheta * CosTheta);

		const float PhiRadians = FMath::DegreesToRadians(360.0f * GoldenRatio * static_cast<float>(i));

		const FVector Point(
			SinTheta * FMath::Cos(PhiRadians),
			SinTheta * FMath::Sin(PhiRadians),
			CosTheta
		);

		Points.Add(Point);
	}

	return Points;
}

bool ABoid::IsInFieldOfView(const FVector& OtherPosition) const {
	if (!Spawner) {
		return true;
	}

	const FVector BoidPosition = GetActorLocation();
	const FVector BoidForward = Direction;
	const FVector DistanceToOther = (OtherPosition - BoidPosition).GetSafeNormal();

	const float DotProduct = FVector::DotProduct(BoidForward, DistanceToOther);

	const float AngleRadians = FMath::Acos(DotProduct);
	const float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);

	const bool IsInView = AngleDegrees <= Spawner->ViewAngle;

	return IsInView;
}