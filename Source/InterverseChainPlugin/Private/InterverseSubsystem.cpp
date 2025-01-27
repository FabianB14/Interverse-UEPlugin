#include "InterverseSubsystem.h"

void UInterverseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    ChainComponent = NewObject<UInterverseChainComponent>(this, TEXT("ChainComponent"));
    InventoryComponent = NewObject<UInterverseInventoryComponent>(this, TEXT("InventoryComponent"));

    if (ChainComponent)
    {
        ChainComponent->RegisterComponent();
    }

    if (InventoryComponent)
    {
        InventoryComponent->RegisterComponent();
    }
}

void UInterverseSubsystem::Deinitialize()
{
    if (ChainComponent)
    {
        ChainComponent->UnregisterComponent();
    }

    if (InventoryComponent)
    {
        InventoryComponent->UnregisterComponent();
    }
}