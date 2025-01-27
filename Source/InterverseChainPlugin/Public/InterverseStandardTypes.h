#pragma once

#include "CoreMinimal.h"
#include "InterverseStandardTypes.generated.h"

UENUM(BlueprintType)
enum class EInterverseItemCategory : uint8
{
    Weapon      UMETA(DisplayName = "Weapon"),
    Armor       UMETA(DisplayName = "Armor"),
    Accessory   UMETA(DisplayName = "Accessory"),
    Consumable  UMETA(DisplayName = "Consumable"),
    Currency    UMETA(DisplayName = "Currency"),
    Cosmetic    UMETA(DisplayName = "Cosmetic"),
    Mount       UMETA(DisplayName = "Mount"),
    Pet         UMETA(DisplayName = "Pet")
};

UENUM(BlueprintType)
enum class EInterverseRarity : uint8
{
    Common      UMETA(DisplayName = "Common"),
    Uncommon    UMETA(DisplayName = "Uncommon"),
    Rare        UMETA(DisplayName = "Rare"),
    Epic        UMETA(DisplayName = "Epic"),
    Legendary   UMETA(DisplayName = "Legendary"),
    Mythic      UMETA(DisplayName = "Mythic")
};

USTRUCT(BlueprintType)
struct INTERVERSECHAINPLUGIN_API FInterverseBaseProperties
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    EInterverseItemCategory Category;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    EInterverseRarity Rarity;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    int32 Level;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    FString ModelIdentifier;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    FLinearColor PrimaryColor;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    FLinearColor SecondaryColor;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    TMap<FString, float> NumericProperties;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    TMap<FString, FString> StringProperties;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    TArray<FString> Tags;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    FString OwnerGlobalID;  // Player global ID

    UPROPERTY(BlueprintReadWrite, Category = "Interverse")
    FString TargetPlayerID;  // For transfers

    bool IsValid() const
    {
        return !ModelIdentifier.IsEmpty();
    }
};