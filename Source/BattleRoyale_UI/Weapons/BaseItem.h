

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FItemTraceStateDelegate, AActor*, InstigatorActor, bool, bIsTracing);


UCLASS()
class BATTLEROYALE_UI_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitProperties() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
public:

	void SetPhysics(bool bEnabled);

	virtual void UpdateSettings() {}

protected:

	UPROPERTY()
	class UPrimitiveComponent* PhysicsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Components")
	class UBoxComponent* ItemCollisionBox;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	FString ItemName;

	UPROPERTY()
	class AItemUI* ItemUI;

public:

	UPROPERTY()
	AActor* ItemOwner;

	//Called when the Item has started or stopped being traced
	UPROPERTY(BlueprintAssignable, Category = "Item")
	FItemTraceStateDelegate ItemTraceStateDelegate;	

};
