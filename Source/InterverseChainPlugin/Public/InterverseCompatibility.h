#pragma once

#include "CoreMinimal.h"
#include "InterverseStandardTypes.h"
#include "InterverseChainDelegates.h"
#include "Json.h"

namespace InterverseCompat
{
    inline FString GetEndpointPath(const FString& Endpoint)
    {
        FString Path = Endpoint;
        if (!Path.StartsWith(TEXT("verse/")))
        {
            Path = FString::Printf(TEXT("verse/%s"), *Path);
        }
        return Path;
    }

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

    inline TSharedPtr<FJsonObject> ConvertAssetToJson(const FInterverseBaseProperties& Properties, 
                                                     const TMap<FString, FString>& CustomProperties)
    {
        TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
        
        // Convert base properties
        JsonObject->SetStringField(TEXT("category"), ConvertItemCategory(Properties.Category));
        JsonObject->SetStringField(TEXT("rarity"), ConvertRarity(Properties.Rarity));
        JsonObject->SetNumberField(TEXT("level"), Properties.Level);
        JsonObject->SetStringField(TEXT("model_id"), Properties.ModelIdentifier);

        // Add colors
        TSharedPtr<FJsonObject> PrimaryColor = MakeShared<FJsonObject>();
        PrimaryColor->SetNumberField(TEXT("r"), Properties.PrimaryColor.R);
        PrimaryColor->SetNumberField(TEXT("g"), Properties.PrimaryColor.G);
        PrimaryColor->SetNumberField(TEXT("b"), Properties.PrimaryColor.B);
        PrimaryColor->SetNumberField(TEXT("a"), Properties.PrimaryColor.A);
        JsonObject->SetObjectField(TEXT("primary_color"), PrimaryColor);

        TSharedPtr<FJsonObject> SecondaryColor = MakeShared<FJsonObject>();
        SecondaryColor->SetNumberField(TEXT("r"), Properties.SecondaryColor.R);
        SecondaryColor->SetNumberField(TEXT("g"), Properties.SecondaryColor.G);
        SecondaryColor->SetNumberField(TEXT("b"), Properties.SecondaryColor.B);
        SecondaryColor->SetNumberField(TEXT("a"), Properties.SecondaryColor.A);
        JsonObject->SetObjectField(TEXT("secondary_color"), SecondaryColor);

        // Convert numeric properties
        TSharedPtr<FJsonObject> NumericProps = MakeShared<FJsonObject>();
        for (const auto& Pair : Properties.NumericProperties)
        {
            NumericProps->SetNumberField(Pair.Key, Pair.Value);
        }
        JsonObject->SetObjectField(TEXT("numeric_properties"), NumericProps);

        // Convert string properties
        TSharedPtr<FJsonObject> StringProps = MakeShared<FJsonObject>();
        for (const auto& Pair : Properties.StringProperties)
        {
            StringProps->SetStringField(Pair.Key, Pair.Value);
        }
        JsonObject->SetObjectField(TEXT("string_properties"), StringProps);

        // Convert tags array
        TArray<TSharedPtr<FJsonValue>> TagsArray;
        for (const FString& Tag : Properties.Tags)
        {
            TagsArray.Add(MakeShared<FJsonValueString>(Tag));
        }
        JsonObject->SetArrayField(TEXT("tags"), TagsArray);

        // Add custom properties
        TSharedPtr<FJsonObject> CustomProps = MakeShared<FJsonObject>();
        for (const auto& Pair : CustomProperties)
        {
            CustomProps->SetStringField(Pair.Key, Pair.Value);
        }
        JsonObject->SetObjectField(TEXT("custom_properties"), CustomProps);

        return JsonObject;
    }

    inline bool ConvertJsonToAsset(const TSharedPtr<FJsonObject>& JsonObject, FInterverseAsset& OutAsset)
    {
        if (!JsonObject.IsValid())
            return false;

        OutAsset.AssetId = JsonObject->GetStringField(TEXT("asset_id"));
        OutAsset.Owner = JsonObject->GetStringField(TEXT("owner"));
        
        FString CategoryStr = JsonObject->GetStringField(TEXT("category"));
        if (CategoryStr == TEXT("WEAPON")) OutAsset.Category = EInterverseItemCategory::Weapon;
        else if (CategoryStr == TEXT("ARMOR")) OutAsset.Category = EInterverseItemCategory::Armor;
        // ... add other category conversions

        // Convert metadata
        const TSharedPtr<FJsonObject>* MetadataObj;
        if (JsonObject->TryGetObjectField(TEXT("metadata"), MetadataObj))
        {
            for (const auto& Pair : (*MetadataObj)->Values)
            {
                OutAsset.Metadata.Add(Pair.Key, Pair.Value->AsString());
            }
        }

        return true;
    }
}