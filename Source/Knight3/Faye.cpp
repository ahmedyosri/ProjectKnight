// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight3.h"
#include "Faye.h"


void AFaye::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetFirstPlayerController()->Possess((APawn*)this);
}


void AFaye::Tick(float DeltaTime){

	Super::Tick(DeltaTime);

	// ================= Update Rotation
	FRotator actorRot = GetActorRotation();
	FRotator res = FMath::RInterpTo(actorRot, FRotator(0, IsIt(Flags::FacingRight) ? 0 : 180, 0), DeltaTime, 20);
	SetActorRotation(res);

}