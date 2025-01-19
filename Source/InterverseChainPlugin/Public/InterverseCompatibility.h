#pragma once

#include "CoreMinimal.h"
#include "InterverseStandardTypes.h"
#include "Json.h"
#include "Text.h"

namespace InterverseCompat
{
    // Convert endpoints from Interverse to VERSE format
    inline FString GetEndpointPath(const FString& Endpoint)
    {
        // Convert Interverse endpoints to VERSE endpoints
        FString Path = Endpoint;
        if (!Path.StartsWith(TEXT("verse/")))
        {
            Path = FString::Printf(TEXT("verse/%s"), *Path);
        }
        return Path;
    }

    // Convert Interverse category enums to VERSE strings
    inline FString ConvertItemCategory(EInterverseItemCategory Category)
    {
        switch(Category)
        {
            case EInterverseItemCategory::Weapon: return TEXT("WEAPON");
            case EInterverseItemCategory::Armor: return TEXT("ARMOR");
            case EInterverseItemCategory::Accessory: return TEXT("ACCESSORY");
            case EInterverseItemCategory::Consumable: return TEXT("CONSUMABLE");
            case EInterverseItemCategory::Currency: return TEXT("CURRENCY");
            case EInterverseItemCategory::Cosmetic: return TEXT("COSMETIC");
            case EInterverseItemCategory::Mount: return TEXT("MOUNT");
            case EInterverseItemCategory::Pet: return TEXT("PET");
            default: return TEXT("UNKNOWN");
        }
    }

    // Convert Interverse rarity enums to VERSE strings
    inline FString ConvertRarity(EInterverseRarity Rarity)
    {
        switch(Rarity)
        {
            case EInterverseRarity::Common: return TEXT("COMMON");
            case EInterverseRarity::Uncommon: return TEXT("UNCOMMON");
            case EInterverseRarity::Rare: return TEXT("RARE");
            case EInterverseRarity::Epic: return TEXT("EPIC");
            case EInterverseRarity::Legendary: return TEXT("LEGENDARY");
            case EInterverseRarity::Mythic: return TEXT("MYTHIC");
            default: return TEXT("COMMON");
        }
    }

    // Convert Interverse asset properties to VERSE JSON format
    inline TSharedPtr<FJsonObject> ConvertAssetToJson(const FInterverseBaseProperties& Properties, 
                                                     const TMap<FString, FString>& CustomProperties)
    {
        TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
        
        // Convert base properties
        JsonObject->SetStringField("category", ConvertItemCategory(Properties.Category));
        JsonObject->SetStringField("rarity", ConvertRarity(Properties.Rarity));
        JsonObject->SetNumberField("level", Properties.Level);
        JsonObject->SetStringField("model_id", Properties.ModelIdentifier);

        // Add colors
        TSharedPtr<FJsonObject> PrimaryColor = MakeShared<FJsonObject>();
        PrimaryColor->SetNumberField("r", Properties.PrimaryColor.R);
        PrimaryColor->SetNumberField("g", Properties.PrimaryColor.G);
        PrimaryColor->SetNumberField("b", Properties.PrimaryColor.B);
        PrimaryColor->SetNumberField("a", Properties.PrimaryColor.A);
        JsonObject->SetObjectField("primary_color", PrimaryColor);

        TSharedPtr<FJsonObject> SecondaryColor = MakeShared<FJsonObject>();
        SecondaryColor->SetNumberField("r", Properties.SecondaryColor.R);
        SecondaryColor->SetNumberField("g", Properties.SecondaryColor.G);
        SecondaryColor->SetNumberField("b", Properties.SecondaryColor.B);
        SecondaryColor->SetNumberField("a", Properties.SecondaryColor.A);
        JsonObject->SetObjectField("secondary_color", SecondaryColor);

        // Convert numeric properties
        TSharedPtr<FJsonObject> NumericProps = MakeShared<FJsonObject>();
        for (const auto& Pair : Properties.NumericProperties)
        {
            NumericProps->SetNumberField(Pair.Key, Pair.Value);
        }
        JsonObject->SetObjectField("numeric_properties", NumericProps);

        // Convert string properties
        TSharedPtr<FJsonObject> StringProps = MakeShared<FJsonObject>();
        for (const auto& Pair : Properties.StringProperties)
        {
            StringProps->SetStringField(Pair.Key, Pair.Value);
        }
        JsonObject->SetObjectField("string_properties", StringProps);

        // Convert tags array
        TArray<TSharedPtr<FJsonValue>> TagsArray;
        for (const FString& Tag : Properties.Tags)
        {
            TagsArray.Add(MakeShared<FJsonValueString>(Tag));
        }
        JsonObject->SetArrayField("tags", TagsArray);

        // Add custom properties
        TSharedPtr<FJsonObject> CustomProps = MakeShared<FJsonObject>();
        for (const auto& Pair : CustomProperties)
        {
            CustomProps->SetStringField(Pair.Key, Pair.Value);
        }
        JsonObject->SetObjectField("custom_properties", CustomProps);

        return JsonObject;
    }

    // Convert VERSE JSON response to Interverse asset
    inline bool ConvertJsonToAsset(const TSharedPtr<FJsonObject>& JsonObject, FInterverseAsset& OutAsset)
    {
        if (!JsonObject.IsValid())
            return false;

        OutAsset.AssetId = JsonObject->GetStringField("asset_id");
        OutAsset.Owner = JsonObject->GetStringField("owner");
        
        // Convert enums back from strings
        FString CategoryStr = JsonObject->GetStringField("category");
        if (CategoryStr == "WEAPON") OutAsset.Category = EInterverseItemCategory::Weapon;
        else if (CategoryStr == "ARMOR") OutAsset.Category = EInterverseItemCategory::Armor;
        // ... add other category conversions

        // Convert metadata
        const TSharedPtr<FJsonObject>* MetadataObj;
        if (JsonObject->TryGetObjectField("metadata", MetadataObj))
        {
            for (const auto& Pair : (*MetadataObj)->Values)
            {
                OutAsset.Metadata.Add(Pair.Key, Pair.Value->AsString());
            }
        }

        return true;
    }
};