// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight3.h"
#include "PlayerControl.h"

//const int32 APlayerControl::MAX_FLAGS = 25;

// Sets default values
APlayerControl::APlayerControl()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APlayerControl::OnComponentHit);

	for (int32 i = 0; i < MAX_FLAGS; i++)
		isIt[i] = false;
}

void APlayerControl::UpdateFlags(){

	tmpVector = GetActorLocation();

	for (int32 i = 0; i < MAX_FLAGS; i++){
		//if (isIt[i])
		//	PrintText(FString::FromInt(i));
		switch (i)
		{

		case Flags::FacingRight:
			if (GetInputAxisValue(AXRight) != 0.0f){
				isIt[i] = GetInputAxisValue(AXRight) > 0.5f;
			}
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
			isIt[i] = (tmpVector.X > 750) && (tmpVector.X < 1700) && FMath::IsNearlyEqual(tmpVector.Y, layerDepth, 1);
			break;

		case Flags::Sliding:
			if (GetInputAxisValue(AXRight) == 0.0f)
				isIt[i] = false;
			//Also Updated in OnComponentHit
			break;

		case Flags::TargetingRightWallForSlide:
			// Updated in OnComponentHit
			break;

		case Flags::TryingToSlide:
			// Updated in OnComponentHit
			break;

		case Flags::PossibleToWallJump:
			isIt[i] = !IsIt(Flags::Sliding) || currSlidingTimer > 0.0f;
			break;
		case Flags::PossibleToClimbWall:
			break;

		default:
			break;
		}
	}

	//PrintText("========");
}

// Called when the game starts or when spawned
void APlayerControl::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetFirstPlayerController()->Possess((APawn*)this);
}

// Called every frame
void APlayerControl::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);

	// ================= Update Flags before anything
	UpdateFlags();

	// ================= Update Rotation

	FRotator actorRot = GetActorRotation();
	FRotator res = FMath::RInterpTo(actorRot, FRotator(0, IsIt(Flags::FacingRight) ? 0 : 180, 0), DeltaTime, 20);
	SetActorRotation(res);

	// ================= Update Depth
	
	currDepthValue = FMath::FInterpTo(currDepthValue, targetLayer*layerDepth, DeltaTime, 2);
	if (FMath::Abs(currDepthValue - targetLayer*layerDepth) < 20)
		currDepthValue = targetLayer * layerDepth;
	tmpVector = GetActorLocation();
	tmpVector.Y = currDepthValue;
	SetActorLocation(tmpVector);


	// ================= Rest
	GetCharacterMovement()->JumpZVelocity = jumpVelocity;

	if (GetCharacterMovement()->Velocity.Z < 0 && (IsIt(Flags::Sliding) || (currSlidingTimer -= DeltaTime) > 0)){

		GetCharacterMovement()->Velocity.Z += 6;
		if (GetCharacterMovement()->Velocity.Z < -slidingSpeed)
			GetCharacterMovement()->Velocity.Z = -slidingSpeed;


	}

}

// Called to bind functionality to input
void APlayerControl::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	
	InputComponent->BindAxis(AXRight,this, &APlayerControl::MoveRight);
	InputComponent->BindAxis(AXUp,	 this, &APlayerControl::LookUp);
	InputComponent->BindAxis(AXDown, this, &APlayerControl::LookDown);
	InputComponent->BindAction(ACJump, IE_Pressed, this, &APlayerControl::JumpPressed);

}

void APlayerControl::MoveRight(float AxisValue){

	AddMovementInput(FVector(1, 0, 0), AxisValue);

	if (AxisValue == 0.0f) return;

	if (currSlidingTimer > 0)
	{
		if (IsIt(Flags::TargetingRightWallForSlide) && AxisValue < 0.5f)
			currSlidingTimer = 0;

		else if (!IsIt(Flags::TargetingRightWallForSlide) && AxisValue > 0.5f)
			currSlidingTimer = 0;
	}


	if (IsIt(Flags::TryingToSlide))
	{
		if (IsIt(Flags::TargetingRightWallForSlide) && AxisValue < 0.5f)
			isIt[(int32)Flags::TryingToSlide] = false;

		else if (!IsIt(Flags::TargetingRightWallForSlide) && AxisValue > 0.5f)
			isIt[(int32)Flags::TryingToSlide] = false;
	}

}

void APlayerControl::LookUp(float AxisValue){
}

void APlayerControl::LookDown(float AxisValue){
	if (AxisValue > 0.5f)
	{
		currSlidingTimer = 0;
	}
}

void APlayerControl::JumpPressed(){
	if (IsIt(Flags::LookingUp) && IsIt(Flags::PossibleToGoUp))
		targetLayer++;
	else if (IsIt(Flags::LookingDown) && IsIt(Flags::PossibleToGoDown)){
		targetLayer--;
		GetCharacterMovement()->JumpZVelocity = 50;
	}

	
	
	if (CanJumpInternal_Implementation()){
		Jump();
		currSlidingTimer = 0;
		PrintText("Jump");
	}
	else if (IsIt(Flags::Sliding) || (IsIt(Flags::TryingToSlide) && ((IsIt(Flags::TargetingRightWallForSlide) && GetInputAxisValue(AXRight) > 0.5f) || (!IsIt(Flags::TargetingRightWallForSlide) && GetInputAxisValue(AXRight) < -0.5f)))){
		currSlidingTimer = 0;
		ClimbWall();
		PrintText("Sliding: " + FString::FromInt(IsIt(Flags::Sliding)));
		PrintText("TryingTo: " + FString::FromInt(IsIt(Flags::TryingToSlide)));

		PrintText("Climb");
	}
	else if (IsIt(Flags::TryingToSlide) || currSlidingTimer > 0.0f)
		/*&&
		((IsIt(TargetingRightWallForSlide) && GetInputAxisValue(AXRight) > 0.5f) || (!IsIt(TargetingRightWallForSlide) && GetInputAxisValue(AXRight) < 0.5f)) &&
		GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling
		)*/
	{
		currSlidingTimer = 0;
		WallJump();

		PrintText("Wall Jump");
	}
}

void APlayerControl::OnComponentHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){

	isIt[(int32)Flags::Sliding] = false;
	if (FMath::Abs(Hit.Normal.X) > 0 && FMath::Abs(GetInputAxisValue(AXRight)) > 0.5f && GetCharacterMovement()->Velocity.Z < 0){
		isIt[(int32)Flags::Sliding] = true;
		isIt[(int32)Flags::TargetingRightWallForSlide] = Hit.Normal.X < 0;

		currSlidingTimer = slidingTimeout;
	}

	if (FMath::Abs(Hit.Normal.X) > 0){
		isIt[(int32)Flags::TryingToSlide] = true;
		isIt[(int32)Flags::TargetingRightWallForSlide] = Hit.Normal.X < 0;
	}
	else{
		isIt[(int32)Flags::TryingToSlide] = false;
	}

	
}

void APlayerControl::PrintText(FString txt) const {
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, txt);
}

bool APlayerControl::CanJumpInternal_Implementation() const {

	return Super::CanJumpInternal_Implementation() || IsIt(Flags::Sliding);
}

void APlayerControl::WallJump(){
	GetCharacterMovement()->Velocity = FVector((IsIt(Flags::FacingRight) ? -1 : 1)*0.75f, 0, 0.75f) * jumpVelocity;

	isIt[(int32)Flags::FacingRight] = !IsIt(Flags::FacingRight);
}

void APlayerControl::ClimbWall(){
	GetMovementComponent()->Velocity += FVector(0.1f, 0, 0.9f) * jumpVelocity;
}

bool APlayerControl::IsIt(Flags flag) const {
	return isIt[(int32)flag];
}
