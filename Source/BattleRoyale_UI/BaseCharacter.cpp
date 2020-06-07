// Copyright Juan Marcelo Portillo. All Rights Reserved

#include "BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MaxSpeed_Walking = 300.f;
	MaxSpeed_Sprinting = 500.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 10.f));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bEnableCameraLag = true;
	//SpringArm->bEnableCameraRotationLag = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetRelativeLocation(FVector(100.F, 50.f, 50.f));
	Camera->SetupAttachment(SpringArm);

	if (USkeletalMeshComponent* SKMesh = GetMesh())
	{
		SKMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		SKMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}

	if (UCharacterMovementComponent* MovComp = GetCharacterMovement())
	{
		MovComp->bUseControllerDesiredRotation = true;
		MovComp->GetNavAgentPropertiesRef().bCanCrouch = true;
		MovComp->MaxWalkSpeed = MaxSpeed_Walking;
		MovComp->MaxWalkSpeedCrouched = MaxSpeed_Walking * 0.75f;
	}

	WeaponEquipment = CreateDefaultSubobject<UWeaponEquipmentComponent>("Weapon Equipment");
	WeaponEquipment->OnWeaponRecoil.AddDynamic(this, &ABaseCharacter::OnWeaponRecoil);
	WeaponEquipment->SetupStoreComponent(GetMesh());
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#include "Components/InputComponent.h"
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Movement Input
	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);



	//Sprint
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ABaseCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ABaseCharacter::StopSprinting);

	//Crouch & UnCrouch : Can't call Super::Crouch here because of its bool parameter
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABaseCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ABaseCharacter::StopCrouch);

	//Rotation Input
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	//Weapon Equipment Input
	PlayerInputComponent->BindAction("GetWeapon", IE_Pressed, WeaponEquipment
		, &UWeaponEquipmentComponent::AddSearchedWeapon);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABaseCharacter::Shoot);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABaseCharacter::StopShooting);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponEquipment
		, &UWeaponEquipmentComponent::Reload);
}

void ABaseCharacter::MoveForward(float Value)
{
	AddMovementInput(FQuat(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetForwardVector(), Value);
}

void ABaseCharacter::MoveRight(float Value)
{
	AddMovementInput(FQuat(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetRightVector(), Value);
}

void ABaseCharacter::Shoot()
{
	WeaponEquipment->PullTrigger();
}

void ABaseCharacter::StopShooting()
{
	WeaponEquipment->ReleaseTrigger();
}

void ABaseCharacter::StartCrouch()
{
	Super::Crouch();
}

void ABaseCharacter::StopCrouch()
{
	Super::UnCrouch();
}

void ABaseCharacter::Sprint()
{
	UnCrouch();
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed_Sprinting;
}

void ABaseCharacter::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed_Walking;	
}

void ABaseCharacter::OnWeaponRecoil(float Pitch, float Yaw)
{
	AddControllerPitchInput(Pitch);
	AddControllerYawInput(Yaw);
}

