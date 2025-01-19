# Interverse Item Conversion System

## Overview
The Item Conversion System enables meaningful translation of assets between different game universes. It handles property mapping, value scaling, and effect conversion.

## Conversion Types

### 1. Direct Conversion
Properties that map directly between games:
```
Fantasy Sword → Sci-Fi Energy Blade
- Level → Level (1:1)
- Rarity → Rarity (1:1)
- Owner → Owner (1:1)
```

### 2. Scaled Conversion
Properties that require value scaling:
```
Fantasy → Sci-Fi Damage Conversion
- Physical Damage × 10 = Energy Damage
Example: 100 Physical → 1000 Energy
```

### 3. Effect Conversion
Special effects and abilities:
```
Fantasy → Sci-Fi Effects
- Fire → Plasma
- Ice → Cryo
- Lightning → Electric
- Poison → Radiation
```

## Implementation

### Blueprint Example
```blueprint
Convert Fantasy to Sci-Fi Weapon:
1. Get source properties
2. Map direct properties
3. Scale numeric values
4. Convert effects
5. Update visuals
6. Validate result
```

### C++ Example
```cpp
FInterverseBaseProperties ConvertWeapon(
    const FInterverseBaseProperties& Source,
    const FString& TargetGameType)
{
    FInterverseBaseProperties Target;
    
    // Direct properties
    Target.Level = Source.Level;
    Target.Rarity = Source.Rarity;
    
    // Scaled properties
    float ScaledDamage = ScaleValue(
        Source.NumericProperties["damage"],
        "fantasy",
        "scifi"
    );
    Target.NumericProperties.Add("energy_damage", ScaledDamage);
    
    // Effect conversion
    FString NewEffect = ConvertEffect(
        Source.StringProperties["effect"],
        "fantasy",
        "scifi"
    );
    Target.StringProperties.Add("effect", NewEffect);
    
    return Target;
}
```

## Conversion Rules

### Rule Definition
```json
{
    "property_mapping": {
        "damage": "energy_damage",
        "fire_effect": "plasma_effect"
    },
    "scale_factors": {
        "damage": 10.0,
        "speed": 1.5
    },
    "effect_mapping": {
        "fire": "plasma",
        "ice": "cryo"
    }
}
```

### Rule Application
```cpp
void ApplyConversionRules(
    FInterverseBaseProperties& Properties,
    const FConversionRules& Rules)
{
    // Apply property mapping
    for (auto& Mapping : Rules.PropertyMapping)
    {
        MapProperty(Properties, Mapping);
    }
    
    // Apply scaling
    for (auto& Scale : Rules.ScaleFactors)
    {
        ScaleProperty(Properties, Scale);
    }
    
    // Convert effects
    for (auto& Effect : Rules.EffectMapping)
    {
        ConvertEffect(Properties, Effect);
    }
}
```

## Visual Conversion

### Model Mapping
```cpp
FString MapModel(const FString& SourceModel, 
                const FString& TargetStyle)
{
    // Example: "fantasy_sword_01" → "scifi_blade_01"
    TMap<FString, FString> ModelMap;
    ModelMap.Add("fantasy_sword", "scifi_blade");
    // ... add more mappings
    
    return ConvertModelId(SourceModel, ModelMap);
}
```

### Effect Visuals
```cpp
void ConvertEffectVisuals(
    UParticleSystem* SourceEffect,
    const FString& TargetStyle)
{
    // Convert particle effects
    // Convert sound effects
    // Convert animations
}
```

## Best Practices

1. **Value Scaling**
   - Document scaling factors
   - Maintain relative power levels
   - Handle edge cases

2. **Effect Conversion**
   - Preserve effect purpose
   - Match visual themes
   - Maintain gameplay balance

3. **Validation**
   - Check property bounds
   - Verify conversions
   - Handle missing mappings

4. **Performance**
   - Cache conversion rules
   - Batch conversions
   - Optimize frequent paths

## Example Conversions

### Weapon Example
```
Fantasy Sword → Sci-Fi Blade
Source:
- Category: Weapon
- Type: Sword
- Damage: 100
- Effect: Fire
- Color: Red

Target:
- Category: Weapon
- Type: Energy Blade
- Energy Damage: 1000
- Effect: Plasma
- Color: Red Energy
```

### Mount Example
```
Fantasy Dragon → Sci-Fi Mech
Source:
- Speed: 500
- Can_Fly: True
- Attack: Fire Breath
- Effect: Flame Aura

Target:
- Speed: 750
- Can_Fly: True
- Attack: Plasma Cannon
- Effect: Energy Shield
```