// Copyright Juan Marcelo Portillo. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "AttachmentData.h"
#include "BaseAttachment.generated.h"

UCLASS()
class BATTLEROYALE_UI_API ABaseAttachment : public ABaseItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseAttachment();

public:	

	UFUNCTION(BlueprintCallable, Category = "Attachment Data")
	bool GetAttachmentData(FWeaponAttachmentData& outAttachmentData) const;

	virtual void UpdateSettings() override;

	void SetWeaponOwner(class ABaseWeapon* Weapon);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attachment Components")
	class UStaticMeshComponent* AttachmentMesh;

	UPROPERTY()
	class ABaseWeapon* WeaponOwner;

	FWeaponAttachmentData* AttachmentData;

};
