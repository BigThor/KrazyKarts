// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

UCLASS()
class KRAZYKARTS_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	FVector GetAirResistance();
	FVector GetRollingResistance();
	void UpdateRotation(float DeltaTime);
	void UpdateLocationFromVelocity(float DeltaTime);

	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForward(float Value);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Value);

private:
	/** The mass of the car (Kg).*/
	UPROPERTY(EditAnywhere)
	float Mass = 1000.f;
	/** The force factor applied (N). */
	UPROPERTY(EditAnywhere)
	float ForceScaleFactor = 15000.f;
	/** Minimum radius of thee car turning circle at full lock (m). */
	UPROPERTY(EditAnywhere)
	float MinimumTurningRadius = 10.f;
	/** Drag coefficient that implies how aerodynamic the car is. */
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 6;
	/** Resistance coefficient that  */
	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015;

	UPROPERTY(Replicated)
	float Throttle;
	UPROPERTY(Replicated)
	float SteeringThrow;
	UPROPERTY(Replicated)
	FVector Velocity;

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedTransform)
	FTransform ReplicatedTransform;
	UFUNCTION()
	void OnRep_ReplicatedTransform();
};
