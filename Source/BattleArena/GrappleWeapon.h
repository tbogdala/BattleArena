// Copyright 2018 by Timothy Bogdala. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "GrappleWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEARENA_API AGrappleWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	AGrappleWeapon();

	void FirePrimary();
    void FireAlternate();

	bool IsGrappleReady();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AActor> GrappleProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float GrappleProjectileSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float GrappleDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float GrappleCooldownSec;

	void OnGrappleLanded();

	float LastGrappleTime;
	FVector_NetQuantize LastGrappleHitLocation;
	FTimerHandle TimerHandle_OnGrappleLanded;
	FTimerHandle TimerHandle_OnGrappleProjectileDespawn;
};
