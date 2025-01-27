#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InterverseChainComponent.h"
#include "InterverseInventoryComponent.h"
#include "InterverseSubsystem.generated.h"

UCLASS()
class INTERVERSECHAINPLUGIN_API UInterverseSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Interverse")
    UInterverseChainComponent* GetChainComponent() const { return ChainComponent; }

    UFUNCTION(BlueprintCallable, Category = "Interverse")
    UInterverseInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

private:
    UPROPERTY()
    UInterverseChainComponent* ChainComponent;

    UPROPERTY()
    UInterverseInventoryComponent* InventoryComponent;
};