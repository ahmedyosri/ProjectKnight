// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight3.h"
#include "NPC.h"


void ANPC::BeginPlay(){

	Super::BeginPlay();

	//Fetch Patrolling points
	TArray<AActor*> tmpActors;
	GetAttachedActors(tmpActors);
	for (int32 i = tmpActors.Num()-1; i >= 0 ; i--)	//Traversing in reverse order because they're reversed in GetAttachedActors
	{
		if (tmpActors[i]->ActorHasTag("PatrolPoint")){
			m_PatrollingPath.Add(tmpActors[i]->GetActorLocation());
		}
	}


}

//===============================================================================

NPCState ANPC::GetCurrentState(){
	return m_CurrentState;
}

void ANPC::SetCurrentState(NPCState newState){
	m_CurrentState = newState;
}

TArray<FVector> ANPC::GetPatrollingPathActors(){
	return m_PatrollingPath;
}