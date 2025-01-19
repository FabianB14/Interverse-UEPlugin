# Interverse Standard Properties System

## Overview
The Interverse Standard Properties System provides a unified framework for handling game assets across different game universes. This standardization ensures consistent behavior and meaningful translation of assets between games.

## Property Categories

### Universal Properties
These properties are mandatory for all assets:

1. **Base Properties**
   - Category (EInterverseItemCategory)
   - Rarity (EInterverseRarity)
   - Level (Integer)
   - Model Identifier (String)

2. **Visual Properties**
   - Primary Color (Linear Color)
   - Secondary Color (Linear Color)
   - Scale (Vector)
   - Custom Material Parameters (Map)

3. **Numeric Properties**
   Custom numerical values stored as key-value pairs:
   ```cpp
   TMap<FString, float> NumericProperties;
   // Example:
   // "damage": 100.0f
   // "durability": 1000.0f
   // "weight": 5.0f
   ```

4. **String Properties**
   Custom string values stored as key-value pairs:
   ```cpp
   TMap<FString, FString> StringProperties;
   // Example:
   // "effect": "fire"
   // "description": "A legendary sword"
   // "creator": "Master Smith"
   ```

5. **Tags**
   Array of strings for additional properties:
   ```cpp
   TArray<FString> Tags;
   // Example:
   // ["fire", "melee", "two_handed"]
   ```

## Property Implementation

### Blueprint Example
```blueprint
Create Interverse Base Properties:
1. Set Category = Weapon
2. Set Rarity = Legendary
3. Set Level = 50
4. Set ModelIdentifier = "sword_01"
5. Set Primary Color = (R=1.0, G=0.0, B=0.0, A=1.0)
6. Add Numeric Property "damage" = 100.0
7. Add String Property "effect" = "fire"
8. Add Tags = ["fire", "melee"]
```

### C++ Example
```cpp
FInterverseBaseProperties CreateWeaponProperties()
{
    FInterverseBaseProperties Props;
    Props.Category = EInterverseItemCategory::Weapon;
    Props.Rarity = EInterverseRarity::Legendary;
    Props.Level = 50;
    Props.ModelIdentifier = TEXT("sword_01");
    Props.PrimaryColor = FLinearColor::Red;
    Props.NumericProperties.Add(TEXT("damage"), 100.0f);
    Props.StringProperties.Add(TEXT("effect"), TEXT("fire"));
    Props.Tags.Add(TEXT("fire"));
    Props.Tags.Add(TEXT("melee"));
    return Props;
}
```

## Property Validation

### Required Properties
All assets must have:
- Valid Category
- Valid Rarity
- Level ≥ 0
- Non-empty ModelIdentifier

### Validation Example
```cpp
bool ValidateProperties(const FInterverseBaseProperties& Props)
{
    return !Props.ModelIdentifier.IsEmpty() && 
           Props.Level >= 0 &&
           Props.Category != EInterverseItemCategory::None;
}
```

## Best Practices

1. **Property Naming**
   - Use clear, descriptive names
   - Follow camelCase convention
   - Use standard prefixes for similar properties

2. **Value Ranges**
   - Document acceptable ranges
   - Implement range validation
   - Handle out-of-range values gracefully

3. **Metadata**
   - Include creation timestamp
   - Track modification history
   - Add game-specific tags

4. **Optimization**
   - Cache frequently accessed properties
   - Batch property updates
   - Use appropriate data types

## Common Property Sets

### Weapons
```cpp
Required Properties:
- damage
- attack_speed
- range
- durability
- weapon_type
```

### Armor
```cpp
Required Properties:
- defense
- weight
- armor_type
- resistance_types
- mobility_factor
```

### Mounts
```cpp
Required Properties:
- speed
- stamina
- can_fly
- carry_capacity
- mount_type
```

## Error Handling

Handle common property errors:
1. Missing required properties
2. Invalid property types
3. Out-of-range values
4. Incompatible combinations

Example:
```cpp
try
{
    ValidateProperties(Props);
    ProcessProperties(Props);
}
catch (const FInterversePropertyException& e)
{
    HandlePropertyError(e);
}
```