#include "InterversePlayerComponent.h"
#include "InterverseChainComponent.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"

UInterversePlayerComponent::UInterversePlayerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInterversePlayerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UInterversePlayerComponent::InitializePlayer(const FString& GameSpecificID, const FString& DisplayName)
{
    // Generate or retrieve global player ID
    FString GlobalID = GenerateGlobalPlayerID(GameSpecificID);
    
    // Set up player ID struct
    CurrentPlayerID.GlobalPlayerID = GlobalID;
    CurrentPlayerID.CurrentGameID = GameSpecificID;
    CurrentPlayerID.PlayerName = DisplayName;
    CurrentPlayerID.LastKnownGameID = GetWorld()->GetGameInstance()->GetName();
    CurrentPlayerID.LastActiveTime = FDateTime::UtcNow();

    // Register with blockchain
    RegisterPlayerWithChain();

    // Broadcast event
    OnPlayerIdentified.Broadcast(CurrentPlayerID);
}

FString UInterversePlayerComponent::GenerateGlobalPlayerID(const FString& GameSpecificID)
{
    // Generate a unique ID that combines:
    // - Hash of game-specific ID
    // - Timestamp
    // - Random salt
    FString TimeStamp = FDateTime::UtcNow().ToString();
    FString Salt = FGuid::NewGuid().ToString();
    
    FString CombinedString = GameSpecificID + TimeStamp + Salt;
    return FMD5::HashAnsiString(*CombinedString);
}

void UInterversePlayerComponent::RegisterPlayerWithChain()
{
    if (UInterverseChainComponent* ChainComponent = GetOwner()->FindComponentByClass<UInterverseChainComponent>())
    {
        // Create player registration record
        TSharedPtr<FJsonObject> PlayerRecord = MakeShared<FJsonObject>();
        PlayerRecord->SetStringField(TEXT("type"), TEXT("player_registration"));
        PlayerRecord->SetStringField(TEXT("global_id"), CurrentPlayerID.GlobalPlayerID);
        PlayerRecord->SetStringField(TEXT("game_id"), CurrentPlayerID.CurrentGameID);
        PlayerRecord->SetStringField(TEXT("player_name"), CurrentPlayerID.PlayerName);
        PlayerRecord->SetStringField(TEXT("current_game"), CurrentPlayerID.LastKnownGameID);
        
        // Convert to string
        FString SerializedRecord;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&SerializedRecord);
        FJsonSerializer::Serialize(PlayerRecord.ToSharedRef(), Writer);

        // Send to blockchain
        // Implementation depends on your blockchain integration
    }
}

void UInterversePlayerComponent::UpdatePlayerActivity()
{
    CurrentPlayerID.LastActiveTime = FDateTime::UtcNow();
    CurrentPlayerID.LastKnownGameID = GetWorld()->GetGameInstance()->GetName();

    // Update on blockchain if needed
    RegisterPlayerWithChain();
}