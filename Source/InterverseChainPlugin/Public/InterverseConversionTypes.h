#pragma once

#include "CoreMinimal.h"
#include "InterverseStandardTypes.h"
#include "InterverseConversionTypes.generated.h"

USTRUCT(BlueprintType)
struct INTERVERSECHAINPLUGIN_API FInterverseConversionRule
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interverse|Conversion")
    FString FromGameType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interverse|Conversion")
    FString ToGameType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interverse|Conversion")
    EInterverseItemCategory ItemCategory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interverse|Conversion")
    TMap<FString, float> NumericConversionRates;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interverse|Conversion")
    TMap<FString, FString> PropertyMappings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interverse|Conversion")
    TMap<FString, FLinearColor> ColorMappings;
};

UCLASS(Blueprintable, BlueprintType)
class INTERVERSECHAINPLUGIN_API UInterverseConversionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Initialize and register default conversions
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // Add a new conversion rule
    UFUNCTION(BlueprintCallable, Category = "Interverse|Conversion")
    void RegisterConversionRule(const FInterverseConversionRule& Rule);

    // Convert asset properties between game types
    UFUNCTION(BlueprintCallable, Category = "Interverse|Conversion")
    FInterverseBaseProperties ConvertAsset(
        const FInterverseBaseProperties& Properties,
        const FString& FromGame,
        const FString& ToGame
    );

    // Blueprint event that can be implemented to modify conversion results
    UFUNCTION(BlueprintImplementableEvent, Category = "Interverse|Conversion")
    void OnAssetConverted(
        const FInterverseBaseProperties& OriginalProperties,
        UPARAM(ref) FInterverseBaseProperties& ConvertedProperties,
        const FString& FromGame,
        const FString& ToGame
    );

protected:
    UPROPERTY()
    TArray<FInterverseConversionRule> ConversionRules;

    // Helper function to find applicable conversion rule
    FInterverseConversionRule* FindConversionRule(
        const FString& FromGame,
        const FString& ToGame,
        EInterverseItemCategory Category
    );
};