#include "InterverseGameInstance.h"

UInterverseGameInstance::UInterverseGameInstance()
{
}

void UInterverseGameInstance::Init()
{
    Super::Init();

    // Create components
    ChainComponent = NewObject<UInterverseChainComponent>(this, TEXT("ChainComponent"));
    InventoryComponent = NewObject<UInterverseInventoryComponent>(this, TEXT("InventoryComponent"));

    // Initialize components if needed
    if (ChainComponent)
    {
        // Setup chain component
        ChainComponent->RegisterComponent();
    }

    if (InventoryComponent)
    {
        // Setup inventory component
        InventoryComponent->RegisterComponent();
    }
}