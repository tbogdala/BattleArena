// Copyright 2018 by Timothy Bogdala. All Rights Reserved.

#include "BattleArenaGameModeBase.h"
#include "BattleCharacter.h"
#include "BattleArenaPlayerState.h"
#include "BattleArenaGameInstance.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GameSession.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#define MAX_PLAYERS 2


ABattleArenaGameModeBase::ABattleArenaGameModeBase()
{
	RoundDurationSec = 60.0f;
	PostRoundDurationSec = 5.0f;
}


void ABattleArenaGameModeBase::SetKillBox(AActor* KillBox)
{
	this->KillBox = KillBox;

	FVector Origin, Extent;
	KillBox->GetActorBounds(true, Origin, Extent);
	InitialKillBoxSize = Extent.X;
}

void ABattleArenaGameModeBase::ActorHitKillBox(AActor* ActorHit)
{
	// we only care about hits if the match is in progress
	if (!IsMatchInProgress()) {
		return;
	}

	// TODO update player state for the player that wasn't hit to reward them with a point

	if (!PlayerClass) {
		UE_LOG(LogTemp, Error, TEXT("NO PLAYER CLASS SET IN THE GAME MODE"));
		return;
	}

	AActor *Winner = nullptr;
	AActor *Loser = nullptr;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerClass, FoundActors);
	for (int i = 0; i < FoundActors.Num(); i++) {
		if (FoundActors[i] == ActorHit) 
		{
			Loser = FoundActors[i];
		}
		else 
		{
			Winner = FoundActors[i];
		}
	}

	// just do a sanity check on the number of players found
	if (FoundActors.Num() > 2 || FoundActors.Num() < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Found a non-standard number of players for this game."));
	}

	FinishRound(Winner, Loser);
}

void ABattleArenaGameModeBase::Tick(float DeltaSeconds)
{
	RoundRemainingSec -= DeltaSeconds;

	if (GetMatchState() == MatchState::InProgress)
	{
		// scale the killbox's size based on round time left
		if (KillBox)
		{
			float kbScale = RoundRemainingSec / RoundDurationSec;
			KillBox->SetActorScale3D(FVector(kbScale, kbScale, 1.0));
		}
	}
	else if (GetMatchState() == MatchState::WaitingPostMatch)
	{
		if (RoundRemainingSec <= 0.0f) {
			UE_LOG(LogTemp, Log, TEXT("ABattleArenaGameModeBase restarting round"));

			// TODO notify player of restart

			// reset the size of the box
			KillBox->SetActorScale3D(FVector(1.0, 1.0, 1.0));

			TArray<AActor*> FoundSpawns;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerStartClass, FoundSpawns);
			if (FoundSpawns.Num() != 2)
			{
				UE_LOG(LogTemp, Error, TEXT("THERE MUST BE TWO PLAYER START LOCATIONS IN A LEVEL"));
				return;
			}

			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerClass, FoundActors);
			for (int i = 0; i < FoundActors.Num() && i<=MAX_PLAYERS; i++)
			{
				FoundActors[i]->SetActorLocation(FoundSpawns[i]->GetActorLocation());
			}

			TArray<AActor*> FoundAI;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIClass, FoundAI);
			if (FoundAI.Num() > 0)
			{
				// if we have an ai, then the only player will be at index 0
				AActor *FarSpawnActor = GetFurthestSpawnFromActor(FoundActors[0]);
				FoundAI[0]->SetActorLocation(FarSpawnActor->GetActorLocation());
			}


			// and we're back in progress
			RoundRemainingSec = RoundDurationSec;
			SetMatchState(MatchState::InProgress);
		}
	}
}

AActor* ABattleArenaGameModeBase::GetFurthestSpawnFromActor(AActor *PlayerActor)
{
	FVector PlayerLocation = PlayerActor->GetActorLocation();

	TArray<AActor*> FoundSpawns;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerStartClass, FoundSpawns);
	if (FoundSpawns.Num() != 2)
	{
		UE_LOG(LogTemp, Error, TEXT("THERE MUST BE TWO PLAYER START LOCATIONS IN A LEVEL"));
		return nullptr;
	}

	float farDistance = 0.0;
	AActor* farSpawn = FoundSpawns[0];

	for (int i = 0; i < 2; i++)
	{
		FVector SpawnLocation = FoundSpawns[i]->GetActorLocation();
		float delta = FVector::Dist(SpawnLocation, PlayerLocation);
		if (delta > farDistance)
		{
			farDistance = delta;
			farSpawn = FoundSpawns[i];
		}
	}

	return farSpawn;
}

void ABattleArenaGameModeBase::ConsiderSpawningBots()
{
	// if game instance says we should run bots, consider running some bots
	UBattleArenaGameInstance *GI = Cast<UBattleArenaGameInstance>(GetGameInstance());
	if (GI && GI->bSpawnBots)
	{
		UWorld* World = GetWorld();

		// do we have only one player?
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(World, PlayerClass, FoundActors);
		if (FoundActors.Num() < 2)
		{
			// do we already have an AI player?
			TArray<AActor*> FoundAI;
			UGameplayStatics::GetAllActorsOfClass(World, AIClass, FoundAI);
			if (FoundAI.Num() < 1)
			{
				AActor *FarSpawnActor = GetFurthestSpawnFromActor(FoundActors[0]);

				// now that we have the furthest spawn, add the AI player
				if (FarSpawnActor)
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					AActor* AIActor = World->SpawnActor<AActor>(AIClass, FarSpawnActor->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
					if (AIActor)
					{
						return;
					}
				}

				// error either finding the spawn or spawning the ai
				UE_LOG(LogTemp, Error, TEXT("Failed to spawn AI player"));
			}
		}
	}
}

void ABattleArenaGameModeBase::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	RoundRemainingSec = RoundDurationSec;

	// spawn bots if enabled and needed
	ConsiderSpawningBots();

	// TODO Consider notifying clients of round start to update HUD
}


bool ABattleArenaGameModeBase::WeAreReadyToStartMatch()
{
	int requiredPlayerCount = 2;

	// we only need one player if we're spawning bots
	UBattleArenaGameInstance *GI = Cast<UBattleArenaGameInstance>(GetGameInstance());
	if (GI && GI->bSpawnBots)
	{
		requiredPlayerCount = 1;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerClass, FoundActors);
	if (FoundActors.Num() >= requiredPlayerCount)
	{
		return true;
	}
	
	return false;
}

void ABattleArenaGameModeBase::FinishRound(AActor* Winner, AActor* Loser)
{
	// only finish rounds in progress
	if (IsMatchInProgress()) 
	{
		UE_LOG(LogTemp, Log, TEXT("Finishing round with a winner and loser"));

		ABattleCharacter* WinnerPlayer = Cast<ABattleCharacter>(Winner);
		if (WinnerPlayer)
		{
			ABattleArenaPlayerState* PS = Cast<ABattleArenaPlayerState>(WinnerPlayer->PlayerState);
			if (PS)
			{
				PS->Wins += 1;
			}
		}

		ABattleCharacter* LosingPlayer = Cast<ABattleCharacter>(Loser);
		if (LosingPlayer)
		{
			ABattleArenaPlayerState* PS = Cast<ABattleArenaPlayerState>(LosingPlayer->PlayerState);
			if (PS)
			{
				PS->Losses += 1;
			}
		}

		EndMatch();

		RoundRemainingSec = PostRoundDurationSec;
	}
}