// Copyright Juan Marcelo Portillo. All Rights Reserved


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemUI.generated.h"

UCLASS()
class BATTLEROYALE_UI_API AItemUI : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemUI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void ItemTraced(AActor* InstigatorActor, bool bIsItemTraced);

	UPROPERTY(BlueprintReadOnly, Category = "Items")
	class ABaseItem* ReferenceItem;

protected:

	UPROPERTY()
	class APawn* PlayerPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item UI")
	class USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item UI")
	class UWidgetComponent* ItemWidgetComponent;
};
