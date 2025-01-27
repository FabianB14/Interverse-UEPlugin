#include "InterverseBlueprintLibrary.h"
#include "Json.h"
#include "JsonObjectConverter.h"
#include "InterverseCompatibility.h"

FInterverseBaseProperties UInterverseBlueprintLibrary::ConvertFantasyToSciFiWeapon(const FInterverseBaseProperties& FantasyWeapon)
{
    FInterverseBaseProperties SciFiWeapon = FantasyWeapon;
    
    // Convert properties based on the fantasy to sci-fi mapping
    if (SciFiWeapon.StringProperties.Contains(TEXT("DamageType")))
    {
        FString DamageType = SciFiWeapon.StringProperties[TEXT("DamageType")];
        if (DamageType == TEXT("Fire"))
        {
            SciFiWeapon.StringProperties.Add(TEXT("DamageType"), TEXT("Plasma"));
        }
        else if (DamageType == TEXT("Ice"))
        {
            SciFiWeapon.StringProperties.Add(TEXT("DamageType"), TEXT("Cryo"));
        }
    }

    // Scale damage values
    if (SciFiWeapon.NumericProperties.Contains(TEXT("Damage")))
    {
        float Damage = SciFiWeapon.NumericProperties[TEXT("Damage")];
        SciFiWeapon.NumericProperties.Add(TEXT("Damage"), Damage * 10.0f); // Sci-fi weapons deal more base damage
    }

    return SciFiWeapon;
}

FInterverseBaseProperties UInterverseBlueprintLibrary::ConvertSciFiToFantasyWeapon(const FInterverseBaseProperties& SciFiWeapon)
{
    FInterverseBaseProperties FantasyWeapon = SciFiWeapon;
    
    // Convert properties based on the sci-fi to fantasy mapping
    if (FantasyWeapon.StringProperties.Contains(TEXT("DamageType")))
    {
        FString DamageType = FantasyWeapon.StringProperties[TEXT("DamageType")];
        if (DamageType == TEXT("Plasma"))
        {
            FantasyWeapon.StringProperties.Add(TEXT("DamageType"), TEXT("Fire"));
        }
        else if (DamageType == TEXT("Cryo"))
        {
            FantasyWeapon.StringProperties.Add(TEXT("DamageType"), TEXT("Ice"));
        }
    }

    // Scale damage values
    if (FantasyWeapon.NumericProperties.Contains(TEXT("Damage")))
    {
        float Damage = FantasyWeapon.NumericProperties[TEXT("Damage")];
        FantasyWeapon.NumericProperties.Add(TEXT("Damage"), Damage * 0.1f); // Fantasy weapons deal less base damage
    }

    return FantasyWeapon;
}

float UInterverseBlueprintLibrary::ScaleDamageValue(float InputDamage, const FString& FromGame, const FString& ToGame)
{
    // Simple scaling logic - can be expanded based on specific game requirements
    if (FromGame == TEXT("Fantasy") && ToGame == TEXT("SciFi"))
    {
        return InputDamage * 10.0f;
    }
    else if (FromGame == TEXT("SciFi") && ToGame == TEXT("Fantasy"))
    {
        return InputDamage * 0.1f;
    }
    return InputDamage;
}

FLinearColor UInterverseBlueprintLibrary::ConvertEffectColor(const FLinearColor& InputColor, const FString& EffectType)
{
    // Convert effect colors based on type
    if (EffectType == TEXT("Fire"))
    {
        return FLinearColor(1.0f, 0.2f, 0.0f, InputColor.A); // Red-orange for fire
    }
    else if (EffectType == TEXT("Ice"))
    {
        return FLinearColor(0.0f, 0.8f, 1.0f, InputColor.A); // Light blue for ice
    }
    else if (EffectType == TEXT("Plasma"))
    {
        return FLinearColor(0.6f, 0.0f, 1.0f, InputColor.A); // Purple for plasma
    }
    
    return InputColor;
}

bool UInterverseBlueprintLibrary::ValidateAssetProperties(const FInterverseBaseProperties& Properties)
{
    // Basic validation
    if (!Properties.IsValid())
    {
        return false;
    }

    // Additional validation logic
    bool bHasRequiredProperties = true;
    switch (Properties.Category)
    {
        case EInterverseItemCategory::Weapon:
            bHasRequiredProperties = Properties.NumericProperties.Contains(TEXT("Damage"));
            break;
        case EInterverseItemCategory::Armor:
            bHasRequiredProperties = Properties.NumericProperties.Contains(TEXT("Defense"));
            break;
        default:
            break;
    }

    return bHasRequiredProperties;
}

FString UInterverseBlueprintLibrary::GetAssetTypeString(EInterverseItemCategory Category)
{
    return InterverseCompat::ConvertItemCategory(Category);
}

FString UInterverseBlueprintLibrary::PropertiesToJson(const FInterverseBaseProperties& Properties)
{
    FString OutputString;
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    FJsonObjectConverter::UStructToJsonObject(FInterverseBaseProperties::StaticStruct(), &Properties, JsonObject.ToSharedRef(), 0, 0);
    
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    
    return OutputString;
}

bool UInterverseBlueprintLibrary::JsonToProperties(const FString& JsonString, FInterverseBaseProperties& OutProperties)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    
    if (!FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        return false;
    }
    
    return FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FInterverseBaseProperties::StaticStruct(), &OutProperties, 0, 0);
}