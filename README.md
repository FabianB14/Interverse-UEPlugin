# Interverse Chain Plugin for Unreal Engine

## Overview
Interverse Chain Plugin enables seamless cross-game asset transfer and transformation in Unreal Engine games. This plugin provides a standardized system for handling game items, allowing them to be meaningfully transferred between different game universes while maintaining their core properties and value.

## Features
- Cross-game asset transfer system
- Standardized item property framework
- Automatic conversion between game styles
- Full Blueprint support
- Real-time WebSocket updates
- Comprehensive asset management
- Reward system integration
- Full Unreal Engine 5.5+ integration

## Installation
1. Copy the plugin to your project's `Plugins` directory
2. Enable the plugin in Edit > Plugins
3. Rebuild your project
4. Add Interverse components to your actors

## Standard Properties System
Interverse uses a comprehensive property system that ensures consistent item handling across different games:

### Item Categories
- Weapon
- Armor
- Accessory
- Consumable
- Currency
- Cosmetic
- Mount
- Pet

### Rarity Levels
- Common
- Uncommon
- Rare
- Epic
- Legendary
- Mythic

### Base Properties
Each item includes:
- Universal Properties (Category, Rarity, Level)
- Visual Properties (Model ID, Colors)
- Game-Specific Properties (Stats, Effects)
- Tags and Metadata

## Asset Conversion Examples

### Weapon Conversion (Fantasy to Sci-Fi)
```
Fantasy: Flaming Sword
- Damage: 100
- Effect: Flame Trail
- Color: Red

Converts to:
Sci-Fi: Plasma Blade
- Energy Damage: 1000
- Effect: Energy Trail
- Blade Color: Red Plasma
```

### Mount Conversion
```
Fantasy: Dragon Mount
- Speed: 500
- Flight: True
- Attack: Fire Breath
- Effects: Flame Aura

Converts to:
Sci-Fi: Mech Dragon
- Speed: 500
- Flight: Anti-gravity
- Weapon: Plasma Cannon
- Effects: Energy Field
```

## Implementation Guide

### Basic Setup
```cpp
// Add Interverse Chain Component
UPROPERTY()
class UInterverseChainComponent* InterverseComponent;

// Initialize in BeginPlay
void AMyActor::BeginPlay()
{
    Super::BeginPlay();
    InterverseComponent = NewObject<UInterverseChainComponent>(this);
    InterverseComponent->RegisterComponent();
}
```

### Blueprint Usage
1. Add InterverseChainComponent to your actor
2. Configure connection settings:
   - Node URL
   - Game ID
   - API Key
3. Use provided events for asset handling:
   - OnAssetMinted
   - OnTransferComplete
   - OnBalanceUpdated
   - OnWebSocketConnected
   - OnWebSocketMessage

## Core Components

### InterverseChainComponent
Handles core blockchain interactions:
- Asset minting and transfers
- Balance management
- Real-time updates via WebSocket
- Transaction processing

### InterverseInventoryComponent
Manages in-game inventory:
- Asset storage and retrieval
- Equipment system
- Item filtering and sorting
- Inventory persistence

### InterverseMiningComponent
Handles mining and rewards:
- Mining operations
- Reward calculations
- Mining difficulty adjustment
- Performance optimization

## File Structure

```
InterverseChainPlugin/
├── Source/
│   ├── Public/
│   │   ├── InterverseChainPlugin.h
│   │   ├── InterverseChainComponent.h
│   │   ├── InterverseInventoryComponent.h
│   │   ├── InterverseMiningComponent.h
│   │   ├── InterverseStandardTypes.h
│   │   ├── InterverseCompatibility.h
│   │   └── InterverseChainDelegates.h
│   ├── Private/
│   │   ├── InterverseChainPlugin.cpp
│   │   ├── InterverseChainComponent.cpp
│   │   ├── InterverseInventoryComponent.cpp
│   │   └── InterverseMiningComponent.cpp
│   └── InterverseChainPlugin.Build.cs
├── Resources/
│   └── Icon128.png
├── Content/
│   ├── Blueprints/
│   │   ├── Examples/
│   │   │   ├── BP_InterverseTest.uasset
│   │   │   ├── BP_WeaponConverter.uasset
│   │   │   └── BP_EffectConverter.uasset
│   │   └── Templates/
│   │       ├── BP_GenericWeapon.uasset
│   │       └── BP_GenericArmor.uasset
│   └── Documentation/
│       └── Images/
├── Docs/
│   ├── StandardProperties.md
│   ├── ItemConversion.md
│   ├── NetworkSetup.md
│   └── Examples.md
├── README.md
└── InterverseChainPlugin.uplugin
```

## Quick Start
1. Add InterverseChainComponent to your actor/pawn
2. Configure the connection settings
3. Bind to relevant events
4. Use provided Blueprint nodes or C++ functions
5. Test with included example Blueprints

## Testing
The plugin includes several test Blueprints:
- BP_InterverseTest: Basic functionality testing
- BP_WeaponConverter: Asset conversion testing
- BP_EffectConverter: Effect system testing

## Documentation
- [Standard Properties System](Docs/StandardProperties.md)
- [Item Conversion Guide](Docs/ItemConversion.md)
- [Network Setup](Docs/NetworkSetup.md)
- [Implementation Examples](Docs/Examples.md)

## Support
For support, please visit our [issues page](https://github.com/yourusername/InterverseChainPlugin/issues)

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing
1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request