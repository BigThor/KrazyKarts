// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "DrawDebugHelpers.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
	Velocity = FVector::ZeroVector;
}

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
		break;
	case ROLE_SimulatedProxy:
		return "Simulated Proxy";
		break;
	case ROLE_AutonomousProxy:
		return "Autonomous Proxy";
		break;
	case ROLE_Authority:
		return "Authority";
		break;
	default:
		return "Error";
		break;
	}
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Force += GetAirResistance() + GetRollingResistance();
	FVector Acceleration = Force / Mass;

	GetRollingResistance();

	Velocity = Velocity + Acceleration * DeltaTime;

	UpdateRotation(DeltaTime);
	UpdateLocationFromVelocity(DeltaTime);

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(GetLocalRole()), this, FColor::Emerald, DeltaTime);
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!IsValid(PlayerInputComponent))
	{
		return;
	}

	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);
}

FVector AGoKart::GetAirResistance()
{
	float Speed = Velocity.Size();
	float AirResistance = -(Speed * Speed * DragCoefficient);
	return Velocity.GetSafeNormal() * AirResistance;
}

FVector AGoKart::GetRollingResistance()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return FVector::ZeroVector;
	}
	// Converting to meters
	float Gravity = World->GetGravityZ() / 100.f;
	FVector RollingResistance = Velocity.GetSafeNormal() * Gravity * Mass * RollingResistanceCoefficient;
	return RollingResistance;
}

void AGoKart::UpdateRotation(float DeltaTime)
{
	float DeltaLocation = FVector::DotProduct(Velocity, GetActorForwardVector()) * DeltaTime;
	float RotationAngleRadians = SteeringThrow * DeltaLocation / MinimumTurningRadius;
	FQuat RotationDelta(GetActorUpVector(), RotationAngleRadians);
	AddActorWorldRotation(RotationDelta);
	// Update velocity direction to match new rotation
	Velocity = RotationDelta.RotateVector(Velocity);
}

void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	// Times 100 to make it work in meters
	FVector Translation = Velocity * DeltaTime * 100.f;

	FHitResult HitResult;
	AddActorWorldOffset(Translation, true, &HitResult);
	if (HitResult.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

void AGoKart::MoveForward(float Value)
{
	Force = GetActorForwardVector() * Value * ForceScaleFactor;
	Server_MoveForward(Value);
}

void AGoKart::MoveRight(float Value)
{
	SteeringThrow = Value;
	Server_MoveRight(Value);
}


void AGoKart::Server_MoveForward_Implementation(float Value)
{
	Force = GetActorForwardVector() * Value * ForceScaleFactor;
}

bool AGoKart::Server_MoveForward_Validate(float Value)
{
	return FMath::Abs(Value) <= 1.0f;
}

void AGoKart::Server_MoveRight_Implementation(float Value)
{
	SteeringThrow = Value;
}

bool AGoKart::Server_MoveRight_Validate(float Value)
{
	return FMath::Abs(Value) <= 1.0f;
}
