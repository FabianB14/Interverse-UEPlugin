#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InterverseStandardTypes.h"
#include "InterverseBlueprintLibrary.generated.h"

UCLASS()
class INTERVERSECHAINPLUGIN_API UInterverseBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // Asset Type Conversion Functions
    UFUNCTION(BlueprintCallable, Category = "Interverse|Conversion")
    static FInterverseBaseProperties ConvertFantasyToSciFiWeapon(
        const FInterverseBaseProperties& FantasyWeapon);

    UFUNCTION(BlueprintCallable, Category = "Interverse|Conversion")
    static FInterverseBaseProperties ConvertSciFiToFantasyWeapon(
        const FInterverseBaseProperties& SciFiWeapon);

    // Property Scaling Functions
    UFUNCTION(BlueprintPure, Category = "Interverse|Conversion")
    static float ScaleDamageValue(float InputDamage, 
                                const FString& FromGame, 
                                const FString& ToGame);

    UFUNCTION(BlueprintPure, Category = "Interverse|Conversion")
    static FLinearColor ConvertEffectColor(const FLinearColor& InputColor,
                                         const FString& EffectType);

    // Validation Functions
    UFUNCTION(BlueprintPure, Category = "Interverse|Validation")
    static bool ValidateAssetProperties(const FInterverseBaseProperties& Properties);

    UFUNCTION(BlueprintPure, Category = "Interverse|Validation")
    static FString GetAssetTypeString(EInterverseItemCategory Category);

    // JSON Utility Functions
    UFUNCTION(BlueprintPure, Category = "Interverse|Utility")
    static FString PropertiesToJson(const FInterverseBaseProperties& Properties);

    UFUNCTION(BlueprintPure, Category = "Interverse|Utility")
    static bool JsonToProperties(const FString& JsonString, 
                               FInterverseBaseProperties& OutProperties);
};