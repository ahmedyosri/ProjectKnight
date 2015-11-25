// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "SideScrollerCharacter.generated.h"

UENUM(BlueprintType)
enum class Flags : uint8{
	FacingRight UMETA(DisplayName = "Facing Right"),
	Moving UMETA(DisplayName = "Moving"),
	StoppedJumping UMETA(DisplayName = "Stopped Jumping"),

	LookingUp UMETA(DisplayName = "Looking Up"),
	LookingDown UMETA(DisplayName = "Looking Down"),
	PossibleToGoUp UMETA(DisplayName = "Possible To Go Up"),
	PossibleToGoDown UMETA(DisplayName = "Possible To Go Down"),

	FacingAWall UMETA(DisplayName = "Facing a Wall"),
	Sliding UMETA(DisplayName = "Sliding"),
	PossibleToWallJump UMETA(DisplayName = "Possible To WallJump")

};

UCLASS()
class KNIGHT3_API ASideScrollerCharacter : public ACharacter
{
	GENERATED_BODY()

	
	static const int32 MAX_FLAGS = 25;
	const FName	
		AXRight = "MoveRight",
		AXUp	= "LookUp",
		AXDown	= "LookDown",
		ACJump	= "Jump";

	void UpdateFlags();

public:
	// Sets default values for this character's properties
	ASideScrollerCharacter(const FObjectInitializer& ObjectInitializer);


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	void PrintText(FString txt = "Hello") const;

	UFUNCTION()
		void MoveRight(float AxisValue);
	UFUNCTION()
		void LookUp(float AxisValue);
	UFUNCTION()
		void LookDown(float AxisValue);

	UFUNCTION()
		void JumpPressed();
	UFUNCTION()
		void JumpReleased();

	UPROPERTY(VisibleDefaultsOnly, Category = "Platform Movement")
		UBoxComponent* FacingWallTestComp;

	UFUNCTION()
		void OnComponentHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//UFUNCTION()
	//	void OnWallTesterHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	
	//UPROPERTY(BlueprintReadOnly, Category = "Platform Movement")
	//bool	isThereWayUp, isThereWayDown,
	//		lastWallToRight;

	float	currDepthValue;
	float	currSlidingTimer;
	int32	currentLayer, targetLayer;

	FVector tmpVector;
	FVector* m_Velocity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float jumpVelocity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float layerDepth;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float slidingTimeout;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float slidingSpeed;

	UFUNCTION(BlueprintCallable, Category = "Platform Movement")
	bool IsIt(Flags flag) const;


protected:
	virtual bool CanJumpInternal_Implementation() const override;
			void WallJump();
			void ClimbWall();

			bool isIt[MAX_FLAGS];
};
