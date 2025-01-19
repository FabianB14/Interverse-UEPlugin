#include "InterverseInventoryComponent.h"

UInterverseInventoryComponent::UInterverseInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UInterverseInventoryComponent::AddItem(const FInterverseAsset& Asset)
{
    FInterverseInventoryItem NewItem;
    NewItem.Asset = Asset;
    NewItem.IsEquipped = false;
    NewItem.Slot = Items.Num();
    
    Items.Add(NewItem);
    OnInventoryUpdated.Broadcast(Items);
    return true;
}

bool UInterverseInventoryComponent::RemoveItem(const FString& AssetId)
{
    int32 Index = Items.IndexOfByPredicate([AssetId](const FInterverseInventoryItem& Item) {
        return Item.Asset.AssetId == AssetId;
    });

    if (Index != INDEX_NONE)
    {
        Items.RemoveAt(Index);
        OnInventoryUpdated.Broadcast(Items);
        return true;
    }
    return false;
}

bool UInterverseInventoryComponent::EquipItem(const FString& AssetId)
{
    for (FInterverseInventoryItem& Item : Items)
    {
        if (Item.Asset.AssetId == AssetId)
        {
            // Unequip any other items of the same category
            for (FInterverseInventoryItem& OtherItem : Items)
            {
                if (OtherItem.Asset.Category == Item.Asset.Category)
                {
                    OtherItem.IsEquipped = false;
                }
            }
            
            Item.IsEquipped = true;
            OnInventoryUpdated.Broadcast(Items);
            return true;
        }
    }
    return false;
}

TArray<FInterverseInventoryItem> UInterverseInventoryComponent::GetItemsByCategory(EInterverseItemCategory Category) const
{
    TArray<FInterverseInventoryItem> FilteredItems;
    for (const FInterverseInventoryItem& Item : Items)
    {
        if (Item.Asset.Category == Category)
        {
            FilteredItems.Add(Item);
        }
    }
    return FilteredItems;
}

bool UInterverseInventoryComponent::HasItem(const FString& AssetId) const
{
    return Items.ContainsByPredicate([AssetId](const FInterverseInventoryItem& Item) {
        return Item.Asset.AssetId == AssetId;
    });
}

int32 UInterverseInventoryComponent::GetInventorySize() const
{
    return Items.Num();
}