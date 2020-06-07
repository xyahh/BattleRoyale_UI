// Copyright Juan Marcelo Portillo. All Rights Reserved


#include "WeaponEquipmentComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#define BR_LOG(Message, ...) UE_LOG(LogTemp, Warning, TEXT(Message), __VA_ARGS__)

// Sets default values for this component's properties
UWeaponEquipmentComponent::UWeaponEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	WeaponSearchRange = 500.f;
	bAddWeaponOnNextTick = false;

	CurrentWeapon = nullptr;
	StoreComponent = nullptr;
	PlayerCameraManager = nullptr;

	TracedItem = nullptr;
	// ...
}


// Called when the game starts
void UWeaponEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...

	//Init
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (APlayerController* PC = Cast<APlayerController>(Pawn->Controller))
			PlayerCameraManager = PC->PlayerCameraManager;
	}

	//Fill in Slots
	for (auto& SlotSet : StoreSlotGroups)
		StoreSlots.Append(SlotSet.Value.Slots);
}

#include "GameFramework/Pawn.h"
// Called every frame
void UWeaponEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PlayerCameraManager)
	{
		SearchItem(PlayerCameraManager->GetCameraLocation()
			, PlayerCameraManager->GetActorForwardVector()
			, WeaponSearchRange, bAddWeaponOnNextTick, false);
		bAddWeaponOnNextTick = false;
	}
}

void UWeaponEquipmentComponent::AddItem(ABaseItem* Item, bool bEquipWeapon)
{
	//Make sure that: 
	// 1) Weapon is Valid  
	// 2) Store Component is Valid
	// 3) There is at least 1 available Unoccupied Slot
	if (ABaseWeapon* Weapon = Cast<ABaseWeapon>(Item))
	{
		if (StoreComponent && OccupiedSlots.Num() < StoreSlots.Num())
		{
			Weapons.AddUnique(Weapon);
			Weapon->ItemOwner = GetOwner();
			if (StoreWeapon(Weapon) && bEquipWeapon)
				EquipWeapon(Weapon);
		}		
	}
	else if(ABaseAttachment * Attachment = Cast<ABaseAttachment>(Item))
	{
		Attachments.AddUnique(Attachment);
		Attachment->ItemOwner = GetOwner();
		StoreAttachment(Attachment);
	}
}

void UWeaponEquipmentComponent::GetCurrentWeapon(ABaseWeapon*& outWeapon) const
{
	outWeapon = CurrentWeapon;
}

void UWeaponEquipmentComponent::SelectWeapon(int32 Index)
{
	if (Weapons.IsValidIndex(Index))
		EquipWeapon(Weapons[Index]);
}

void UWeaponEquipmentComponent::SelectNextWeapon(int32 DeltaIndex)
{
	int32 CurrentIndex = Weapons.Find(CurrentWeapon);
	if (CurrentIndex != INDEX_NONE)
	{
		CurrentIndex = (CurrentIndex + DeltaIndex) % Weapons.Num();
		if (CurrentIndex < 0) CurrentIndex = Weapons.Num() + CurrentIndex; //e.g. case -1 would give Weapons.Num() - 1
		EquipWeapon(Weapons[CurrentIndex]);
	}
	else if (nullptr == CurrentWeapon)
	{
		SelectWeapon(0); //Default Weapon
	}
}

void UWeaponEquipmentComponent::SearchItem(const FVector& StartingPoint, const FVector& Direction
	, float Distance, bool bAddWeapon, bool bEquipWeapon)
{
	bool bTraceFailed = true;
	if (UWorld* World = GetWorld())
	{
		FHitResult OutHit;
		if (World->LineTraceSingleByChannel(OutHit, StartingPoint
			, StartingPoint + (Direction * Distance), ECollisionChannel::ECC_Visibility))
		{
			if (ABaseItem* Item = Cast<ABaseItem>(OutHit.Actor))
			{
				//Currently, it's just an extra step of validation 
				// (i.e. Base Weapons have collisions turned off for Mesh & Box when equipped
				// Possible Child Weapons with additional components need this type of check)
				if (IsValid(Item->ItemOwner)) //check if WeaponOwner is not nullptr
					return; //do not handle anything if there's someone already handling 

				bTraceFailed = false;

				//Check that the new weapon is a different one
				if (Item != TracedItem)
				{
					if(TracedItem)
						TracedItem->ItemTraceStateDelegate.Broadcast(GetOwner(), false);
					TracedItem = Item;
					TracedItem->ItemTraceStateDelegate.Broadcast(GetOwner(), true);
				}

				if (bAddWeapon)
				{
					AddItem(Item, bEquipWeapon);
				}
			}			
		}
	}

	if (bTraceFailed && TracedItem)
	{
		TracedItem->ItemTraceStateDelegate.Broadcast(GetOwner(), false);
		TracedItem = nullptr;
	}

}

void UWeaponEquipmentComponent::StoreCurrentWeapon()
{
	if (CurrentWeapon)
		StoreWeapon(CurrentWeapon);
}

void UWeaponEquipmentComponent::PullTrigger()
{
	FWeaponCoreData WeaponData;
	if (CurrentWeapon && CurrentWeapon->GetWeaponData(WeaponData))
	{
		bIsFiring = true;
		
		float TimeRate = (WeaponData.BaseData.RateOfFire > 0.f) ?
			1.f / WeaponData.BaseData.RateOfFire : 0.f;
		
		GetWorld()->GetTimerManager().SetTimer(FiringTimerHandle, this
			, &UWeaponEquipmentComponent::Fire_Internal
			, TimeRate, true, 0.f);
	} else
	{ 
		UE_LOG(LogTemp, Warning, TEXT("Invalid Weapon or Weapon Data. Cannot Fire!"));
	}
	
}

void UWeaponEquipmentComponent::ReleaseTrigger()
{
	bIsFiring = false;
	GetWorld()->GetTimerManager().ClearTimer(FiringTimerHandle);
}

void UWeaponEquipmentComponent::Fire_Internal()
{
	UE_LOG(LogTemp, Warning, TEXT("Firing!!"));
}

void UWeaponEquipmentComponent::AddSearchedWeapon()
{
	bAddWeaponOnNextTick = true;
}

void UWeaponEquipmentComponent::Reload()
{

}

TSet<FName> UWeaponEquipmentComponent::GetSlotGroup(ABaseWeapon* Weapon)
{
	if (Weapon)
	{
		FWeaponCoreData WeaponData;
		if (Weapon->GetWeaponData(WeaponData))
		{
			if (FSlotGroup* SlotGroup = StoreSlotGroups.Find(WeaponData.SlotGroup))
				return SlotGroup->Slots;
		}
		
	}
	return TSet<FName>();
}

bool UWeaponEquipmentComponent::EquipWeapon(ABaseWeapon* Weapon)
{
	if (CurrentWeapon == Weapon) 
		return false; //do not try to requip the same weapon. Meaningless.

	if (CurrentWeapon)
		StoreWeapon(CurrentWeapon); //store the current (i.e. weapon in hand) before equipping new
	AttachItem(Weapon, StoreComponent, WeaponEquipSocket);
	CurrentWeapon = Weapon;
	return true;
}

bool UWeaponEquipmentComponent::StoreWeapon(ABaseWeapon* Weapon)
{
	//Check if the Weapon already Exists in the Map (i.e. already has a Reserved Slot)
	if (FName* Slot = EquippedWeaponSlots.Find(Weapon))
	{
		AttachItem(Weapon, StoreComponent, *Slot);

		if (CurrentWeapon == Weapon)
		{
			ReleaseTrigger(); //Stop Firing the Current Weapon
			CurrentWeapon = nullptr;
		}
		return true;
	}

	//Else, we just check as if it's a new Weapon to be Stored
	FWeaponCoreData CoreData;
	if (Weapon->GetWeaponData(CoreData))
	{
		//Check Weapons Slots available to Equip for Specific Group
		TSet<FName> SlotsAssigned = StoreSlotGroups[CoreData.SlotGroup].Slots;

		//Check which slots are available and which are not
		TSet<FName> AvailableSlots = SlotsAssigned.Difference(OccupiedSlots);

		if (AvailableSlots.Num() > 0)
		{
			FName FirstAvailableSlot = *AvailableSlots.begin();
			AttachItem(Weapon, StoreComponent, FirstAvailableSlot);

			OccupiedSlots.Add(FirstAvailableSlot);
			EquippedWeaponSlots.Add(Weapon, FirstAvailableSlot);

			return true;
		}
	}
	return false;
}

bool UWeaponEquipmentComponent::DropItem(ABaseItem* Item)
{
	if (ABaseWeapon* Weapon = Cast<ABaseWeapon>(Item))
	{
		if (FName* WeaponSlot = EquippedWeaponSlots.Find(Weapon))
		{
			EquippedWeaponSlots.Remove(Weapon);
			OccupiedSlots.Remove(*WeaponSlot);
			Weapons.Remove(Weapon);
			
			Weapon->ItemOwner = nullptr;

			DeattachItem(Weapon);
			if (Weapon == CurrentWeapon)
			{
				ReleaseTrigger();
				CurrentWeapon = nullptr;
			}
			return true;
		}
	}
	
	return false;
}


#include "Components/SkeletalMeshComponent.h"
bool UWeaponEquipmentComponent::EquipAttachment(ABaseAttachment* Attachment
	, ABaseWeapon* TargetWeapon, bool bOverrideCurrentAttachment)
{
	FWeaponCoreData WeaponData;
	FWeaponAttachmentData AttachmentData;

	if (TargetWeapon && TargetWeapon->GetWeaponData(WeaponData) 
		&& Attachment && Attachment->GetAttachmentData(AttachmentData))
	{
		FName* AttachmentSlotName = WeaponData.AttachmentSlots.Find(AttachmentData.WeaponSlot);
		if (TargetWeapon->InstallAttachment(Attachment, bOverrideCurrentAttachment) 
			&& AttachmentSlotName)
		{
			AttachItem(Attachment, TargetWeapon->GetMesh(), *AttachmentSlotName);
			return true;
		}		
	}
	return false;
}

bool UWeaponEquipmentComponent::StoreAttachment(ABaseAttachment* Attachment)
{
	Attachment->SetHidden(true);
	Attachment->SetWeaponOwner(nullptr);
	return true;
}

void UWeaponEquipmentComponent::AttachItem(ABaseItem* Item
	, USceneComponent* Target
	, const FName& SocketName)
{
	Item->SetPhysics(false);
	Item->AttachToComponent(Target
		, FAttachmentTransformRules::SnapToTargetNotIncludingScale
		, SocketName);
}

void UWeaponEquipmentComponent::DeattachItem(ABaseItem* Item)
{
	Item->SetPhysics(true);
	Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}


