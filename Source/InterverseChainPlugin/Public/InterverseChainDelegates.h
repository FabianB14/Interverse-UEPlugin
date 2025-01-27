#pragma once

#include "CoreMinimal.h"
#include "InterverseStandardTypes.h"
#include "InterverseChainDelegates.generated.h"

UENUM(BlueprintType)
enum class EInterverseAssetType : uint8
{
    COSMETIC     UMETA(DisplayName = "Cosmetic"),
    WEAPON       UMETA(DisplayName = "Weapon"),
    COLLECTIBLE  UMETA(DisplayName = "Collectible"),
    CURRENCY     UMETA(DisplayName = "Currency"),
    UTILITY      UMETA(DisplayName = "Utility")
};

USTRUCT(BlueprintType)
struct INTERVERSECHAINPLUGIN_API FInterverseAsset
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    FString AssetId;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    FString Owner;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    FString OwnerGlobalID;  // Player global ID

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    EInterverseAssetType AssetType;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    EInterverseItemCategory Category;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    EInterverseRarity Rarity;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    TMap<FString, FString> Metadata;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAssetMinted, const FInterverseAsset&, Asset, const FString&, PlayerGlobalID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTransferComplete, const FString&, AssetId, const FString&, PlayerID, bool, Success);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBalanceUpdated, float, NewBalance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMiningComplete, float, Reward, const FString&, BlockHash);