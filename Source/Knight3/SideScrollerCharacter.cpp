// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight3.h"
#include "SideScrollerCharacter.h"

//const int32 ASideScrollerCharacter::MAX_FLAGS = 25;

// Sets default values
ASideScrollerCharacter::ASideScrollerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASideScrollerCharacter::OnComponentHit);

	FacingWallTestComp = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, "FacingWallTestComponent");
	FacingWallTestComp->AttachParent = GetRootComponent();

	for (int32 i = 0; i < MAX_FLAGS; i++)
		isIt[i] = false;
}

void ASideScrollerCharacter::UpdateFlags(){

	tmpVector = GetActorLocation();

	for (int32 i = 0; i < MAX_FLAGS; i++){
		if (isIt[i])
			PrintText(FString::FromInt(i));
		switch ((Flags)i)
		{

		case Flags::FacingRight:
			if (GetInputAxisValue(AXRight) != 0.0f){
				isIt[i] = GetInputAxisValue(AXRight) > 0.5f;
			}
			break;

		case Flags::Moving:
			isIt[i] = (GetInputAxisValue(AXRight) != 0.0f);
			break;

		case Flags::LookingUp:
			isIt[i] = GetInputAxisValue(AXUp) > 0.5f;
			break;

		case Flags::LookingDown:
			isIt[i] = GetInputAxisValue(AXDown) > 0.5f;
			break;

		case Flags::PossibleToGoUp:
			isIt[i] = (tmpVector.X > 750) && (FMath::Abs(tmpVector.Y) < 10);
			break;

		case Flags::PossibleToGoDown:
			isIt[i] = (tmpVector.X > 750) && (tmpVector.X < 1700) && FMath::IsNearlyEqual(tmpVector.Y, -1*layerDepth, 1);
			break;

		case Flags::Sliding:
			if (!IsIt(Flags::Moving) && currSlidingTimer <= 0 || !IsIt(Flags::FacingAWall))
				isIt[i] = false;
			//Also Updated in OnComponentHit
			break;

		case Flags::PossibleToWallJump:
			break;

		default:
			break;
		}
	}

	PrintText("========");
}

// Called when the game starts or when spawned
void ASideScrollerCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetFirstPlayerController()->Possess((APawn*)this);
}

// Called every frame
void ASideScrollerCharacter::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);

	// ================= Update Flags before anything
	UpdateFlags();

	// ================= Update Rotation

	FRotator actorRot = GetActorRotation();
	FRotator res = FMath::RInterpTo(actorRot, FRotator(0, IsIt(Flags::FacingRight) ? 0 : 180, 0), DeltaTime, 20);
	SetActorRotation(res);

	// ================= Update Depth
	
	currDepthValue = FMath::FInterpTo(currDepthValue, targetLayer*layerDepth*-1, DeltaTime, 4);
	if (FMath::Abs(currDepthValue - targetLayer*layerDepth*-1) < 20)
		currDepthValue = targetLayer * layerDepth * -1;
	tmpVector = GetActorLocation();
	tmpVector.Y = currDepthValue;
	SetActorLocation(tmpVector);


	// ================= Rest
	GetCharacterMovement()->JumpZVelocity = jumpVelocity;
	currSlidingTimer -= (currSlidingTimer > 0) * DeltaTime;
	if (GetCharacterMovement()->Velocity.Z < 0 && (IsIt(Flags::Sliding))){

		if (GetCharacterMovement()->Velocity.Z < -slidingSpeed)
			GetCharacterMovement()->Velocity.Z = -slidingSpeed;

	}

}

// Called to bind functionality to input
void ASideScrollerCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	
	InputComponent->BindAxis(AXRight,this, &ASideScrollerCharacter::MoveRight);
	InputComponent->BindAxis(AXUp,	 this, &ASideScrollerCharacter::LookUp);
	InputComponent->BindAxis(AXDown, this, &ASideScrollerCharacter::LookDown);
	InputComponent->BindAction(ACJump, IE_Pressed, this, &ASideScrollerCharacter::JumpPressed);

}

void ASideScrollerCharacter::MoveRight(float AxisValue){

	AddMovementInput(FVector(1, 0, 0), AxisValue);

	if (AxisValue == 0.0f) return;

}

void ASideScrollerCharacter::LookUp(float AxisValue){
}

void ASideScrollerCharacter::LookDown(float AxisValue){
	if (AxisValue > 0.5f)
	{
		currSlidingTimer = 0;
	}
}

void ASideScrollerCharacter::JumpPressed(){
	if (IsIt(Flags::LookingUp) && IsIt(Flags::PossibleToGoUp))
		targetLayer++;
	else if (IsIt(Flags::LookingDown) && IsIt(Flags::PossibleToGoDown)){
		targetLayer--;
		GetCharacterMovement()->JumpZVelocity = 50;
	}

	if (CanJumpInternal_Implementation()){
		Jump();
		currSlidingTimer = 0;
	}
	else if (IsIt(Flags::Moving) && IsIt(Flags::PossibleToWallJump)){
		currSlidingTimer = 0;
		ClimbWall();
	}
	else if (IsIt(Flags::PossibleToWallJump))
	{
		currSlidingTimer = 0;
		WallJump();
	}

}

void ASideScrollerCharacter::OnComponentHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){

	isIt[(int32)Flags::Sliding] = false;

	
	if (IsIt(Flags::Moving) && (Hit.Normal.X * GetCharacterMovement()->GetCurrentAcceleration().X) < 0 && GetCharacterMovement()->Velocity.Z < 0){
		isIt[(int32)Flags::Sliding] = true;


		currSlidingTimer = slidingTimeout;
	}
}

void ASideScrollerCharacter::PrintText(FString txt) const {
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, txt);
}

bool ASideScrollerCharacter::CanJumpInternal_Implementation() const {

	return Super::CanJumpInternal_Implementation() || IsIt(Flags::Sliding);
}

void ASideScrollerCharacter::WallJump(){
	GetCharacterMovement()->Velocity = FVector((IsIt(Flags::FacingRight) ? -1 : 1)*0.75f, 0, 0.75f) * jumpVelocity;

	isIt[(int32)Flags::FacingRight] = !IsIt(Flags::FacingRight);
}

void ASideScrollerCharacter::ClimbWall(){
	GetMovementComponent()->Velocity += FVector(0.1f, 0, 0.9f) * jumpVelocity;
}

bool ASideScrollerCharacter::IsIt(Flags flag) const {
	switch (flag){
	case Flags::FacingAWall:
	{
		TArray<AActor*> overlappingActors;
		FacingWallTestComp->GetOverlappingActors(overlappingActors, AStaticMeshActor::StaticClass());
		return overlappingActors.Num() > 0;
	}
	break;

	case Flags::PossibleToWallJump:
		return IsIt(Flags::FacingAWall) && GetCharacterMovement()->Velocity.Z != 0;
		break;

	default:
		return isIt[(int32)flag];
		break;
	}
}
