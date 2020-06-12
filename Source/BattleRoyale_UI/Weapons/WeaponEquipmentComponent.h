// Copyright Juan Marcelo Portillo. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "BaseAttachment.h"
#include "Components/ActorComponent.h"
#include "WeaponEquipmentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponRecoilDelegate, float, Pitch, float, Yaw);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FItemOwnershipChanged, ABaseItem*, Item, bool, bItemAdded);

class APlayerCameraManager;

USTRUCT(BlueprintType)
struct BATTLEROYALE_UI_API FSlotGroup
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Slot Names")
	TSet<FName> Slots;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYALE_UI_API UWeaponEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()


public:	
	// Sets default values for this component's properties
	UWeaponEquipmentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	void SetupStoreComponent(class USceneComponent* Component) { StoreComponent = Component; }

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	 * Adds a new ABaseWeapon to the Set of Weapons carried (if it doesn't exist already)
	 * @param ABaseWeapon* Weapon - The weapon to add.
	 * @param bool bEquipWeapon - Whether to automatically make this the Current Weapon when there is an already equipped weapon
	*/
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void AddItem(ABaseItem* Item, bool bEquipWeapon);

	/*
	 * Removes a ABaseWeapon from the Set of Weapons carried (if it doesn't exist already)
	 * @param ABaseWeapon* Weapon - The weapon to remove.
	*/
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	bool DropItem(ABaseItem* Item);

	/* Returns the currently equipped Weapon (Might be NULL!) */
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void GetCurrentWeapon(ABaseWeapon*& outWeapon) const;

	/* Selects the Weapon at the Given Index, if it's Valid. */
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void SelectWeapon(int32 Index);

	/* Selects the Next Weapon, [DeltaIndex] ahead of the currently Selected Index */
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void SelectNextWeapon(int32 DeltaIndex);

	/*
	 * Does a line trace (Visibility) for Weapons.
	 * @param bAddWeapon - Whether the line trace adds the weapon to the 
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void SearchItem(const FVector& StartingPoint
		, const FVector& Direction, float Distance
		, bool bAddItem
		, bool bEquipWeapon);

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void StoreCurrentWeapon();

	/* Starts Shooting */
	void PullTrigger();

	/* Stops the Shooting */
	void ReleaseTrigger();

	/* Handles the Shooting Logic. Called Internally by Timer */
	void Fire_Internal(); 

	/* Adds the Weapon searched on the next tick (Only does it once). */
	void AddSearchedItem();
	
	/* Aims closely (uses Ironsight or Scope if available) (Stops Wide Aim)*/
	void FocusAim();

	/* Wide Aim, or the Standard Hip-look-from-afar aim (Stops Focus Aim)*/
	void WideAim();

	/* Reloads the Currently Equipped Gun */
	void Reload();

	TSet<FName> GetSlotGroup(ABaseWeapon* Weapon);


protected:

	bool EquipWeapon(ABaseWeapon* Weapon);
	bool StoreWeapon(ABaseWeapon* Weapon);

	bool EquipAttachment(ABaseAttachment* Attachment, ABaseWeapon* TargetWeapon, bool bOverrideCurrentAttachment);
	bool StoreAttachment(ABaseAttachment* Attachment);

	void AttachItem(ABaseItem* Item, USceneComponent* Target, const FName& SocketName);
	void DeattachItem(ABaseItem* Item);

public:

	UPROPERTY(BlueprintAssignable, Category = "Items")
	FItemOwnershipChanged OnItemOwnershipChanged;

	FTimerHandle FiringTimerHandle;


protected:
	//Weapons that are being carried Map. FString represents the name of the Weapon
	UPROPERTY(BlueprintReadOnly, Category = "Weapon Equipment Component")
	TArray<ABaseWeapon*> Weapons;

	//Attachments that are being carried Map. FString represents the name of the Weapon
	UPROPERTY(BlueprintReadOnly, Category = "Weapon Equipment Component")
	TArray<ABaseAttachment*> Attachments;

	//The Component where all the Weapons are going to be handled
	UPROPERTY()
	USceneComponent* StoreComponent;

	UPROPERTY()
	ABaseItem* TracedItem;

	/* Camera View Settings */
	UPROPERTY()
	APlayerController* OwnerPC;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Equipment Component")
	ABaseWeapon* CurrentWeapon;

	//This is Gathered everytime before we fire
	FWeaponCoreData WeaponCoreDataPreFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	float WeaponSearchRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	FName WeaponEquipSocket;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons")
    TMap<EWeaponGroup, FSlotGroup> StoreSlotGroups;

	//Set indicating all Available Store Slots for any weapon
	TSet<FName> StoreSlots;

	//Map indicating which weapon is in which Store slot
	TMap<ABaseWeapon*, FName> EquippedWeaponSlots;

	//A set indicating whether a Slot can be occupied or not
	TSet<FName> OccupiedSlots;

	uint8 bAddItemOnNextTick : 1;

	//Whether trigger is currently pulled (true) or released (false)
	UPROPERTY(BlueprintReadOnly, Category = "Weapons")
	uint8 bIsFiring : 1; 

	//Whether currently in Focused Aim (true) or Wide Aim(false)
	UPROPERTY(BlueprintReadOnly, Category = "Weapons")
	uint8 bIsAimFocused : 1;

	//Whether we want to Unequip when Equip is called on an Already Equipped Weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	uint8 bUnequipOnRedundantEquip : 1; 
};
