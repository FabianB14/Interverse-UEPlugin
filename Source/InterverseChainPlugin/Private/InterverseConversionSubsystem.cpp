#include "InterverseConversionTypes.h"

void UInterverseConversionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Register default Fantasy to SciFi conversion rules
    FInterverseConversionRule FantasyToSciFiWeapons;
    FantasyToSciFiWeapons.FromGameType = TEXT("Fantasy");
    FantasyToSciFiWeapons.ToGameType = TEXT("SciFi");
    FantasyToSciFiWeapons.ItemCategory = EInterverseItemCategory::Weapon;
    
    // Default numeric conversions
    FantasyToSciFiWeapons.NumericConversionRates.Add(TEXT("Damage"), 10.0f);
    FantasyToSciFiWeapons.NumericConversionRates.Add(TEXT("DurabilityPoints"), 2.0f);
    
    // Property mappings
    FantasyToSciFiWeapons.PropertyMappings.Add(TEXT("Fire"), TEXT("Plasma"));
    FantasyToSciFiWeapons.PropertyMappings.Add(TEXT("Ice"), TEXT("Cryo"));
    FantasyToSciFiWeapons.PropertyMappings.Add(TEXT("Lightning"), TEXT("Electric"));
    
    RegisterConversionRule(FantasyToSciFiWeapons);
}

void UInterverseConversionSubsystem::RegisterConversionRule(const FInterverseConversionRule& Rule)
{
    // Remove any existing rule for the same conversion
    ConversionRules.RemoveAll([&](const FInterverseConversionRule& Existing) {
        return Existing.FromGameType == Rule.FromGameType &&
               Existing.ToGameType == Rule.ToGameType &&
               Existing.ItemCategory == Rule.ItemCategory;
    });
    
    ConversionRules.Add(Rule);
}

FInterverseBaseProperties UInterverseConversionSubsystem::ConvertAsset(
    const FInterverseBaseProperties& Properties,
    const FString& FromGame,
    const FString& ToGame)
{
    FInterverseBaseProperties ConvertedProperties = Properties;
    
    FInterverseConversionRule* Rule = FindConversionRule(FromGame, ToGame, Properties.Category);
    if (Rule)
    {
        // Apply numeric conversions
        for (const auto& Pair : ConvertedProperties.NumericProperties)
        {
            if (Rule->NumericConversionRates.Contains(Pair.Key))
            {
                float ConversionRate = Rule->NumericConversionRates[Pair.Key];
                ConvertedProperties.NumericProperties[Pair.Key] = Pair.Value * ConversionRate;
            }
        }
        
        // Apply property mappings
        for (const auto& Pair : ConvertedProperties.StringProperties)
        {
            if (Rule->PropertyMappings.Contains(Pair.Value))
            {
                ConvertedProperties.StringProperties[Pair.Key] = Rule->PropertyMappings[Pair.Value];
            }
        }
        
        // Apply color mappings if available
        if (Rule->ColorMappings.Contains(TEXT("Primary")))
        {
            ConvertedProperties.PrimaryColor = Rule->ColorMappings[TEXT("Primary")];
        }
        if (Rule->ColorMappings.Contains(TEXT("Secondary")))
        {
            ConvertedProperties.SecondaryColor = Rule->ColorMappings[TEXT("Secondary")];
        }
    }
    
    // Allow Blueprint implementations to modify the result
    OnAssetConverted(Properties, ConvertedProperties, FromGame, ToGame);
    
    return ConvertedProperties;
}

FInterverseConversionRule* UInterverseConversionSubsystem::FindConversionRule(
    const FString& FromGame,
    const FString& ToGame,
    EInterverseItemCategory Category)
{
    for (auto& Rule : ConversionRules)
    {
        if (Rule.FromGameType == FromGame &&
            Rule.ToGameType == ToGame &&
            Rule.ItemCategory == Category)
        {
            return &Rule;
        }
    }
    return nullptr;
}