// Copyright Juan Marcelo Portillo. All Rights Reserved
#pragma once

#include "CoreMinimal.h"
#include "WeaponData.h"
#include "BaseItem.h"
#include "BaseWeapon.generated.h"

class ABaseAttachment;
class USkeletalMeshComponent;

UCLASS()
class BATTLEROYALE_UI_API ABaseWeapon : public ABaseItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	bool GetWeaponData(FWeaponCoreData& outWeaponData) const;

	//Installs the given Attachment to the Weapon, if the slot is available
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	bool InstallAttachment(ABaseAttachment* Attachment, bool bOverrideCurrentAttachment);

	//Uninstalls the given Attachment to the Weapon, and frees the slot
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	bool UninstallAttachment(ABaseAttachment* Attachment);

	USkeletalMeshComponent* GetMesh() const { return WeaponMesh; }

protected:

	//Attachment Slots that are currently in use
	UPROPERTY(BlueprintReadOnly, Category = "Weapon Attachments")
	TMap<EWeaponSlot, ABaseAttachment*> CurrentAttachments;

	virtual void UpdateSettings() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Components")
	USkeletalMeshComponent* WeaponMesh;

	FWeaponCoreData* WeaponData;

};
