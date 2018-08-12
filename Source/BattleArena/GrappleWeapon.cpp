// Copyright 2018 by Timothy Bogdala. All Rights Reserved.

#include "GrappleWeapon.h"
#include "BattleArena.h"
#include "BattleCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Public/CollisionQueryParams.h"
#include "Engine/Public/DrawDebugHelpers.h"
#include "Engine/Public/TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"



AGrappleWeapon::AGrappleWeapon()
{
	GrappleProjectileSpeed = 10000.0f;
	GrappleDistance = 3000.0f;
	GrappleCooldownSec = 2.0f;
	LastGrappleTime = -1000.0f;
}

bool AGrappleWeapon::IsGrappleReady()
{
	UWorld* World = GetWorld();

	// if we have not yet met the cooldown then it's not ready
	float fireTimeDelta = World->TimeSeconds - LastGrappleTime;
	if (fireTimeDelta < GrappleCooldownSec) {
		return false;
	}

	return true;
}

void AGrappleWeapon::FirePrimary()
{
	UE_LOG(LogTemp, Log, TEXT("Fire primary grapple"));

	// if this is being called on the client, make a RPS call out to the server for authority implementation
	if (Role < ROLE_Authority)
	{
		Server_FirePrimary();
	}

	UWorld* World = GetWorld();
	ABattleCharacter* Owner = Cast<ABattleCharacter>(GetOwner());

	const float WEAPON_OFFSET = 200.0f;
	FVector EyeLocation;
	FRotator EyeRotation;
	Owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	FVector StartLoc = EyeLocation;
	StartLoc += EyeRotation.Vector() * WEAPON_OFFSET;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* NewProjectile = GetWorld()->SpawnActor<AActor>(GrappleProjectileClass, StartLoc, EyeRotation, SpawnParams);
	if (NewProjectile)
	{
		// get the first static mesh which will be used for physics calculations
		UStaticMeshComponent *projMesh = Cast<UStaticMeshComponent>(NewProjectile->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		projMesh->AddForce(EyeRotation.Vector() * GrappleProjectileSpeed, NAME_None, false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to spawn projectile for grappling weapon"));
	}
}

void AGrappleWeapon::FireAlternate()
{
	UE_LOG(LogTemp, Log, TEXT("Fire alternate grapple"));

	// if this is being called on the client, make a RPS call out to the server for authority implementation
	if (Role < ROLE_Authority)
	{
		Server_FireAlternate();
	}

	UWorld* World = GetWorld();

	// if we have not yet met the cooldown then simply return
	if (!IsGrappleReady()) {
		return;
	}

	// we have surpassed the cooldown, so update the fire time logger
	LastGrappleTime = World->TimeSeconds;

	AActor* Owner = GetOwner();
	FVector EyeLocation;
	FRotator EyeRotation;
	Owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * GrappleDistance);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);
	Params.AddIgnoredActor(this);
	Params.bTraceComplex = true;
	Params.bReturnPhysicalMaterial = false;

	if (World->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, Params))
	{
		// DEBUG there was a blocking hit
		DrawDebugLine(World, EyeLocation, Hit.Location, FColor::Red, false, 4.0f, 0, 5.0f);

		// TODO fire the 'grapple' pulse
		LastGrappleHitLocation = Hit.Location;
		GetWorldTimerManager().SetTimer(TimerHandle_OnGrappleLanded, this, &AGrappleWeapon::OnGrappleLanded, 0.0001f, false);
		UE_LOG(LogTemp, Log, TEXT("Sending grapple to hit location"));
	}
	else 
	{
		// DEBUG draw the miss
		DrawDebugLine(World, EyeLocation, TraceEnd, FColor::Blue, false, 4.0f, 0, 5.0f);
	}
}


void AGrappleWeapon::OnGrappleLanded()
{
	UE_LOG(LogTemp, Log, TEXT("ON GRAPPLE LANDED"));

	AActor* Owner = GetOwner();
	ABattleCharacter* OwnerCharacter = Cast<ABattleCharacter>(Owner);
	if (OwnerCharacter)
	{
		float GrappleForce = 2000.0f;
		FVector DistanceVector = LastGrappleHitLocation - Owner->GetActorLocation();
		DistanceVector.Normalize();
		//OwnerCharacter->GetMesh()->AddForce(DistanceVector * GrappleForce, NAME_None, false);
		OwnerCharacter->LaunchCharacter(DistanceVector * GrappleForce, false, true);
	}
}