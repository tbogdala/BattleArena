// Copyright 2018 by Timothy Bogdala. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BattleArenaGameModeBase.generated.h"

class AActor;

/**
 * 
 */
UCLASS()
class BATTLEARENA_API ABattleArenaGameModeBase : public AGameMode
{
	GENERATED_BODY()
	
	
public:
	ABattleArenaGameModeBase();
	
	UFUNCTION(BlueprintCallable, Category = "Components")
	void SetKillBox(AActor* KillBox);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void ActorHitKillBox(AActor* ActorHit);


protected:
	void Tick(float DeltaSeconds);

	void HandleMatchHasStarted();

	bool WeAreReadyToStartMatch();

	void FinishRound(AActor* Winner, AActor* Loser);

	void ConsiderSpawningBots();

	AActor* GetFurthestSpawnFromActor(AActor *PlayerActor);

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	float RoundDurationSec;

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	float PostRoundDurationSec;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TSubclassOf<AActor> PlayerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TSubclassOf<AActor> AIClass;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TSubclassOf<AActor> PlayerStartClass;


	AActor* KillBox;
	float InitialKillBoxSize;
	float RoundRemainingSec;

};
