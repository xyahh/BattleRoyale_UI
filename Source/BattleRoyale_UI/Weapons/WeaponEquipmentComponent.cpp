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
	bAddItemOnNextTick = false;
	bUnequipOnRedundantEquip = true;

	CurrentWeapon = nullptr;
	StoreComponent = nullptr;
	OwnerPC = nullptr;

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
		OwnerPC = Cast<APlayerController>(Pawn->Controller);

	//Fill in Slots
	for (auto& SlotSet : StoreSlotGroups)
		StoreSlots.Append(SlotSet.Value.Slots);
}

#include "GameFramework/Pawn.h"
// Called every frame
void UWeaponEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerPC && OwnerPC->PlayerCameraManager)
	{
		SearchItem(OwnerPC->PlayerCameraManager->GetCameraLocation()
			, OwnerPC->PlayerCameraManager->GetActorForwardVector()
			, WeaponSearchRange, bAddItemOnNextTick, false);
		bAddItemOnNextTick = false;
	}
}

void UWeaponEquipmentComponent::AddItem(ABaseItem* Item, bool bEquipWeapon)
{
	int32 ItemIndex = INDEX_NONE;

	//Make sure that: 
	// 1) Weapon is Valid  
	// 2) Store Component is Valid
	// 3) There is at least 1 available Unoccupied Slot
	if (ABaseWeapon* Weapon = Cast<ABaseWeapon>(Item))
	{

		if (StoreComponent && OccupiedSlots.Num() < StoreSlots.Num() 
			&& INDEX_NONE == Weapons.Find(Weapon)) //Weapon must not be in list
		{
			ItemIndex = Weapons.Add(Weapon);
			Weapon->ItemOwner = GetOwner();
			if (StoreWeapon(Weapon) && bEquipWeapon)
				EquipWeapon(Weapon);
		}		
	}
	else if(ABaseAttachment * Attachment = Cast<ABaseAttachment>(Item))
	{
		if (INDEX_NONE == Attachments.Find(Attachment))
		{
			ItemIndex = Attachments.Add(Attachment);
			Attachment->ItemOwner = GetOwner();
			StoreAttachment(Attachment);
			//should we also handle bEquipWeapon here?
		}
	}

	if (ItemIndex != INDEX_NONE)
	{
		OnItemOwnershipChanged.Broadcast(Item, ItemIndex, true);
	}
}

void UWeaponEquipmentComponent::GetCurrentWeapon(ABaseWeapon*& outWeapon) const
{
	outWeapon = CurrentWeapon;
}

void UWeaponEquipmentComponent::SelectWeaponIndex(int32 Index)
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

void UWeaponEquipmentComponent::SelectWeapon(ABaseWeapon* Weapon)
{
	if (EquippedWeaponSlots.Find(Weapon))
		EquipWeapon(Weapon);
}

void UWeaponEquipmentComponent::SearchItem(const FVector& StartingPoint, const FVector& Direction
	, float Distance, bool bAddItem, bool bEquipWeapon)
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

				if (bAddItem)
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
	if (CurrentWeapon && CurrentWeapon->GetWeaponData(WeaponCoreDataPreFire, true))
	{
		bIsFiring = true;
		
		float TimeRate = (WeaponCoreDataPreFire.BaseData.RateOfFire > 0.f) ?
			1.f / WeaponCoreDataPreFire.BaseData.RateOfFire : 0.f;

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
	if (OwnerPC)
	{
		float RecoilStrength = FMath::FRandRange(-1.f, 1.f);
		OwnerPC->AddPitchInput(WeaponCoreDataPreFire.BaseData.VerticalRecoil * -FMath::Abs(RecoilStrength));
		OwnerPC->AddYawInput(WeaponCoreDataPreFire.BaseData.HorizontalRecoil * RecoilStrength);
	}
	

	UE_LOG(LogTemp, Warning, TEXT("Firing!!"));
}

void UWeaponEquipmentComponent::AddSearchedItem()
{
	bAddItemOnNextTick = true;
}

#include "Camera/CameraComponent.h"
void UWeaponEquipmentComponent::FocusAim()
{
	if (false == bIsAimFocused && CurrentWeapon && OwnerPC)
	{
		bIsAimFocused = true;
		OwnerPC->SetViewTargetWithBlend(CurrentWeapon, 0.1f);
	}
}

void UWeaponEquipmentComponent::WideAim()
{
	if (true == bIsAimFocused && OwnerPC)
	{
		bIsAimFocused = false;
		OwnerPC->SetViewTargetWithBlend(GetOwner(), 0.1f);
	}
}

void UWeaponEquipmentComponent::Reload()
{

}

TSet<FName> UWeaponEquipmentComponent::GetSlotGroup(ABaseWeapon* Weapon)
{
	if (Weapon)
	{
		FWeaponCoreData WeaponData;
		if (Weapon->GetWeaponData(WeaponData, false))
		{
			if (FSlotGroup* SlotGroup = StoreSlotGroups.Find(WeaponData.SlotGroup))
				return SlotGroup->Slots;
		}
		
	}
	return TSet<FName>();
}

bool UWeaponEquipmentComponent::EquipWeapon(ABaseWeapon* Weapon)
{
	if (CurrentWeapon == Weapon) //When we are trying to equip the same weapon
	{
		if (bUnequipOnRedundantEquip)
		{
			StoreWeapon(CurrentWeapon);
			return true;

		} else 
			return false; //do not try to requip the same weapon. Meaningless.
	}

	WideAim();

	if (CurrentWeapon) //When there is a Current Weapon BUT its not the one we want to equip
		StoreWeapon(CurrentWeapon); //store the current (i.e. weapon in hand) before equipping new
	AttachItem(Weapon, StoreComponent, WeaponEquipSocket);
	
	CurrentWeapon = Weapon;
	OnWeaponSelectionChanged.Broadcast(Weapon, true);
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
			WideAim();
			CurrentWeapon = nullptr;
			OnWeaponSelectionChanged.Broadcast(Weapon, false);
		}
		return true;
	}

	//Else, we just check as if it's a new Weapon to be Stored
	FWeaponCoreData CoreData;
	if (Weapon->GetWeaponData(CoreData, false))
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
	int32 ItemIndex = INDEX_NONE;

	if (ABaseWeapon* Weapon = Cast<ABaseWeapon>(Item))
	{
		if (FName* WeaponSlot = EquippedWeaponSlots.Find(Weapon))
		{
			ItemIndex = Weapons.Find(Weapon);
			if (INDEX_NONE != ItemIndex)
			{
				Weapons.RemoveAt(ItemIndex);

				EquippedWeaponSlots.Remove(Weapon);
				OccupiedSlots.Remove(*WeaponSlot);

				Weapon->ItemOwner = nullptr;

				DeattachItem(Weapon);
				if (Weapon == CurrentWeapon)
				{
					ReleaseTrigger();
					CurrentWeapon = nullptr;
				}
			}
			
		}
	}
	else if (ABaseAttachment* Attachment = Cast<ABaseAttachment>(Item))
	{
		ItemIndex = Attachments.Find(Attachment);
		if (INDEX_NONE != ItemIndex)
		{
			if (ABaseWeapon* WeaponOwner = Attachment->GetWeaponOwner())
				WeaponOwner->UninstallAttachment(Attachment);

			Attachments.RemoveAt(ItemIndex);

			Attachment->ItemOwner = nullptr;
			Attachment->SetWeaponOwner(nullptr);
			Attachment->SetActorHiddenInGame(false);
			Attachment->SetWeaponOwner(nullptr);
			DeattachItem(Attachment);
		}

		
	}

	if (ItemIndex != INDEX_NONE)
	{
		OnItemOwnershipChanged.Broadcast(Item, ItemIndex, false);
	}

	return ItemIndex != INDEX_NONE;
}


#include "Components/SkeletalMeshComponent.h"
bool UWeaponEquipmentComponent::EquipAttachment(ABaseAttachment* Attachment
	, ABaseWeapon* TargetWeapon, bool bOverrideCurrentAttachment)
{
	FWeaponCoreData WeaponData;
	FWeaponAttachmentData AttachmentData;

	if (TargetWeapon && TargetWeapon->GetWeaponData(WeaponData, false) 
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
	Attachment->SetActorHiddenInGame(true);
	Attachment->SetWeaponOwner(nullptr);
	Attachment->SetPhysics(false);
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


