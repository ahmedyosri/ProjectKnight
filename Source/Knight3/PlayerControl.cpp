// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight3.h"
#include "PlayerControl.h"


// Sets default values
APlayerControl::APlayerControl()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerControl::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerControl::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void APlayerControl::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

