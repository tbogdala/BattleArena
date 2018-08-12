// Copyright 2018 by Timothy Bogdala. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class BATTLEARENA_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void FirePrimary();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void FireAlternate();


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USkeletalMeshComponent * MeshComponent;

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_FirePrimary();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_FireAlternate();

};
