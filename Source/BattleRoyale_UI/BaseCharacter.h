// Copyright Juan Marcelo Portillo. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapons/WeaponEquipmentComponent.h"
#include "BaseCharacter.generated.h"

UCLASS()
class BATTLEROYALE_UI_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void Shoot();
	void StopShooting();

	void StartCrouch();
	void StopCrouch();

	void Sprint();
	void StopSprinting();

	//Bound to be Called by WeaponEquipment Component when Weapon is Recoiling
	UFUNCTION()
	void OnWeaponRecoil(float Pitch, float Yaw);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Core Components")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Core Components")
	class UCameraComponent*		Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Components")
	UWeaponEquipmentComponent*	WeaponEquipment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement Settings")
	float MaxSpeed_Walking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement Settings")
	float MaxSpeed_Sprinting;

	bool bIsSprinting;
};
