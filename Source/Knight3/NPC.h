// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SideScrollerCharacter.h"
#include "NPC.generated.h"

UENUM(BlueprintType)
enum class NPCState : uint8{
	Patrolling UMETA(DisplayName = "Patrolling"),
	Alerted UMETA(DisplayName = "Alerted"),

	Attacking UMETA(DisplayName = "Attacking"),
	Defending UMETA(DisplayName = "Defending"),
	Chasing UMETA(DisplayName = "Chasing")
};

/**
 * 
 */
UCLASS()
class KNIGHT3_API ANPC : public ASideScrollerCharacter
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
		TArray<FVector> m_PatrollingPath;

	UPROPERTY(EditAnywhere)
		NPCState m_CurrentState;

	UFUNCTION(BlueprintCallable, category = "NPC Basic")
		NPCState GetCurrentState();

	UFUNCTION(BlueprintCallable, category = "NPC Basic")
		void SetCurrentState(NPCState newState);

	UFUNCTION(BlueprintCallable, category = "NPC Basic")
		TArray<FVector> GetPatrollingPathActors();
};
