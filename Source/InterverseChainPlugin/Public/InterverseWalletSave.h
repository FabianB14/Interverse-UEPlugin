#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "InterverseWalletSave.generated.h"

UCLASS()
class INTERVERSECHAINPLUGIN_API UInterverseWalletSave : public USaveGame
{
    GENERATED_BODY()

public:
    UInterverseWalletSave(); // Declare constructor

    UPROPERTY(BlueprintReadWrite, Category = "Wallet")
    FString WalletAddress;

    UPROPERTY(BlueprintReadWrite, Category = "Wallet")
    float Balance;

    UPROPERTY(BlueprintReadWrite, Category = "Wallet")
    FString LastLoginTime;
};