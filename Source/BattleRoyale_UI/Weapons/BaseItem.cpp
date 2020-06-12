


#include "BaseItem.h"
#include "../BattleRoyaleSettings.h"
#include "Components/PrimitiveComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	ItemCollisionBox = CreateDefaultSubobject<UBoxComponent>("Item Box");
	ItemCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility
		, ECollisionResponse::ECR_Block);

	ItemName = "None";
	ItemUI = nullptr;
	PhysicsComponent = nullptr;

}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	UpdateSettings();

	//Spawn Weapon UI
	if (UBattleRoyaleSettings* Settings = GetMutableDefault<UBattleRoyaleSettings>())
	{
		if (UClass* ItemUIClass = Settings->GetItemUIClass())
		{
			FTransform SpawnTransform = GetActorTransform();
			ItemUI = GetWorld()->SpawnActorDeferred<AItemUI>(ItemUIClass
				, SpawnTransform, this);

			if (ItemUI)
			{
				ItemUI->ReferenceItem = this;
				ItemUI->FinishSpawning(SpawnTransform);
			}

		}
	}
}

void ABaseItem::PostInitProperties()
{
	Super::PostInitProperties();
	UpdateSettings();

	if(ItemCollisionBox)
		ItemCollisionBox->SetupAttachment(PhysicsComponent);
}

#if WITH_EDITOR
void ABaseItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateSettings();
}
#endif

void ABaseItem::SetPhysics(bool bEnabled)
{
	if (PhysicsComponent)
	{
		PhysicsComponent->SetSimulatePhysics(bEnabled);

		ItemCollisionBox->SetCollisionEnabled(bEnabled ?
			ECollisionEnabled::Type::QueryAndPhysics
			: ECollisionEnabled::Type::NoCollision);

		PhysicsComponent->SetCollisionEnabled(bEnabled ? 
			ECollisionEnabled::Type::QueryAndPhysics
			: ECollisionEnabled::Type::NoCollision);
		
	}
}
