// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "SideScrollerCharacter.generated.h"

UENUM(BlueprintType)
enum class Flags : uint8{
	FacingRight UMETA(DisplayName = "FacingRight"),
	Moving UMETA(DisplayName = "Moving"),

	LookingUp UMETA(DisplayName = "LookingUp"),
	LookingDown UMETA(DisplayName = "LookingDown"),
	PossibleToGoUp UMETA(DisplayName = "PossibleToGoUp"),
	PossibleToGoDown UMETA(DisplayName = "PossibleToGoDown"),

	FacingAWall UMETA(DisplayName = "FacingAWall"),
	Sliding UMETA(DisplayName = "Sliding"),
	PossibleToWallJump UMETA(DisplayName = "PossibleToWallJump")

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
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32	targetLayer;

	FVector tmpVector;

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
