#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "InterverseChainComponent.h"
#include "InterverseInventoryComponent.h"
#include "InterverseGameInstance.generated.h"

UCLASS()
class INTERVERSECHAINPLUGIN_API UInterverseGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UInterverseGameInstance();

    virtual void Init() override;

    UPROPERTY()
    UInterverseChainComponent* ChainComponent;

    UPROPERTY()
    UInterverseInventoryComponent* InventoryComponent;
};