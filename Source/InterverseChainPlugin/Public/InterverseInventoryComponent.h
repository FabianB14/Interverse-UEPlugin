#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InterverseChainComponent.h"
#include "InterverseInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInterverseInventoryItem
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    FInterverseAsset Asset;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    bool IsEquipped;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    int32 Slot;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdated, const TArray<FInterverseInventoryItem>&, Items);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INTERVERSECHAINPLUGIN_API UInterverseInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    UInterverseInventoryComponent();

    UPROPERTY(BlueprintReadWrite, Category = "Interverse|Inventory")
    TArray<FInterverseInventoryItem> Items;

    UPROPERTY(BlueprintAssignable, Category = "Interverse|Inventory")
    FOnInventoryUpdated OnInventoryUpdated;

    UFUNCTION(BlueprintCallable, Category = "Interverse|Inventory")
    bool AddItem(const FInterverseAsset& Asset);

    UFUNCTION(BlueprintCallable, Category = "Interverse|Inventory")
    bool RemoveItem(const FString& AssetId);

    UFUNCTION(BlueprintCallable, Category = "Interverse|Inventory")
    bool EquipItem(const FString& AssetId);

    UFUNCTION(BlueprintPure, Category = "Interverse|Inventory")
    TArray<FInterverseInventoryItem> GetItemsByCategory(EInterverseItemCategory Category) const;

    UFUNCTION(BlueprintPure, Category = "Interverse|Inventory")
    bool HasItem(const FString& AssetId) const;

    UFUNCTION(BlueprintPure, Category = "Interverse|Inventory")
    int32 GetInventorySize() const;
};