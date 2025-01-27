#include "InterverseGameLinkComponent.h"
#include "InterversePlayerComponent.h" // Include the player component for player ID handling
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Serialization/JsonSerializer.h"
#include "InterverseChainComponent.h"

UInterverseGameLinkComponent::UInterverseGameLinkComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInterverseGameLinkComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UInterverseGameLinkComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

bool UInterverseGameLinkComponent::RegisterGameLink(const FGameLinkConfig& LinkConfig)
{
    if (LinkConfig.TargetGameId.IsEmpty())
    {
        return false;
    }

    // Store or update the link configuration
    GameLinks.Add(LinkConfig.TargetGameId, LinkConfig);

    // Create blockchain record
    TSharedPtr<FJsonObject> LinkRecord = MakeShared<FJsonObject>();
    LinkRecord->SetStringField("source_game", GetOwner()->GetWorld()->GetGameInstance()->GetName());
    LinkRecord->SetStringField("target_game", LinkConfig.TargetGameId);
    LinkRecord->SetBoolField("direct_transfer", LinkConfig.bAllowDirectObjectTransfer);

    // Add class mappings to record
    TArray<TSharedPtr<FJsonValue>> Mappings;
    for (const auto& Mapping : LinkConfig.ClassMappings)
    {
        TSharedPtr<FJsonObject> MappingObj = MakeShared<FJsonObject>();
        MappingObj->SetStringField("source_class", Mapping.Key.ToString());
        MappingObj->SetStringField("target_class", Mapping.Value.ToString());
        Mappings.Add(MakeShared<FJsonValueObject>(MappingObj));
    }
    LinkRecord->SetArrayField("class_mappings", Mappings);

    // Record on blockchain using the chain component
    if (UInterverseChainComponent* ChainComponent = GetOwner()->FindComponentByClass<UInterverseChainComponent>())
    {
        // Convert record to string
        FString RecordString;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RecordString);
        FJsonSerializer::Serialize(LinkRecord.ToSharedRef(), Writer);

        // Send to blockchain
        // Implementation depends on your blockchain integration
    }

    // Broadcast success
    OnGameLinkEstablished.Broadcast(LinkConfig.TargetGameId);
    return true;
}

bool UInterverseGameLinkComponent::TransferGameObject(AActor* Actor, const FString& TargetGameId, const FString& TargetPlayerID)
{
    if (!Actor || !GameLinks.Contains(TargetGameId))
    {
        return false;
    }
    // Get player component for source player ID
    UInterversePlayerComponent* PlayerComp = GetOwner()->FindComponentByClass<UInterversePlayerComponent>();
    if (!PlayerComp)
    {
        return false;
    }

    const FGameLinkConfig& LinkConfig = GameLinks[TargetGameId];
    if (!LinkConfig.bAllowDirectObjectTransfer)
    {
        return false;
    }

    // Create transfer data
    FTransferredObjectData TransferData;
    if (!SerializeActor(Actor, TransferData))
    {
        return false;
    }

    TransferData.SourcePlayerID = PlayerComp->GetPlayerID().GlobalPlayerID;
    TransferData.TargetPlayerID = TargetPlayerID;

    // Record transfer on blockchain
    RecordTransferOnChain(
        TransferData.SourceGameId,
        TargetGameId,
        TransferData.ObjectId,
        TransferData.SourcePlayerID,
        TransferData.TargetPlayerID
    );

    return TransferObjectData(TransferData, TargetGameId);
}

bool UInterverseGameLinkComponent::SerializeActor(AActor* Actor, FTransferredObjectData& OutData)
{
    if (!Actor)
        return false;

    // Generate transfer ID
    OutData.ObjectId = FGuid::NewGuid().ToString();
    OutData.ObjectClass = Actor->GetClass()->GetPathName();
    
    // Serialize properties
    for (TFieldIterator<FProperty> PropIt(Actor->GetClass()); PropIt; ++PropIt)
    {
        FProperty* Property = *PropIt;
        if (Property->HasAnyPropertyFlags(CPF_SaveGame))
        {
            void* PropertyValue = Property->ContainerPtrToValuePtr<void>(Actor);
            FString ValueString;
            Property->ExportTextItem_Direct(ValueString, PropertyValue, PropertyValue, Actor, PPF_None);
            OutData.ObjectData.Add(Property->GetName(), ValueString);
        }
    }

    OutData.bIsValid = true;
    return true;
}

AActor* UInterverseGameLinkComponent::SpawnReceivedObject(const FTransferredObjectData& ObjectData)
{
    if (!ObjectData.bIsValid)
    {
        return nullptr;
    }

    // Find the mapped class for this object
    UClass* SourceClass = FindObject<UClass>(ANY_PACKAGE, *ObjectData.ObjectClass);
    if (!SourceClass)
    {
        return nullptr;
    }

    UClass* TargetClass = FindMappedClass(SourceClass, ObjectData.SourceGameId);
    if (!TargetClass)
    {
        return nullptr;
    }

    // Spawn the actor
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    AActor* NewActor = GetWorld()->SpawnActor<AActor>(TargetClass, FTransform::Identity, SpawnParams);
    if (!NewActor)
    {
        return nullptr;
    }

    // Apply the deserialized data
    if (DeserializeToActor(ObjectData, NewActor))
    {
        OnObjectReceived.Broadcast(NewActor, ObjectData.SourceGameId);
        return NewActor;
    }

    // Clean up if deserialization failed
    NewActor->Destroy();
    return nullptr;
}

bool UInterverseGameLinkComponent::DeserializeToActor(const FTransferredObjectData& Data, AActor* OutActor)
{
    if (!OutActor)
    {
        return false;
    }

    // Apply properties from the transferred data
     for (const auto& Pair : Data.ObjectData)
    {
        FProperty* Property = FindFProperty<FProperty>(OutActor->GetClass(), *Pair.Key);
        if (Property && Property->HasAnyPropertyFlags(CPF_SaveGame))
        {
            void* PropertyValue = Property->ContainerPtrToValuePtr<void>(OutActor);
            const TCHAR* ImportText = *Pair.Value;
            Property->ImportText_Direct(ImportText, PropertyValue, OutActor, PPF_None);
        }
    }
    
    return true;
}

UClass* UInterverseGameLinkComponent::FindMappedClass(UClass* SourceClass, const FString& TargetGameId) const
{
    const FGameLinkConfig* Config = GameLinks.Find(TargetGameId);
    if (!Config)
    {
        return nullptr;
    }

    // Look up the class mapping
    for (const auto& Mapping : Config->ClassMappings)
    {
        if (Mapping.Key.Get() == SourceClass)
        {
            return Mapping.Value.Get();
        }
    }

    // Return the original class if no mapping is found
    return SourceClass;
}

void UInterverseGameLinkComponent::RecordTransferOnChain(
    const FString& SourceGameId,
    const FString& TargetGameId,
    const FString& ObjectId,
    const FString& SourcePlayerID,
    const FString& TargetPlayerID)
{
    TSharedPtr<FJsonObject> TransferRecord = MakeShared<FJsonObject>();
    TransferRecord->SetStringField("type", "game_object_transfer");
    TransferRecord->SetStringField("source_game", SourceGameId);
    TransferRecord->SetStringField("target_game", TargetGameId);
    TransferRecord->SetStringField("object_id", ObjectId);
    TransferRecord->SetStringField("source_player", SourcePlayerID);
    TransferRecord->SetStringField("target_player", TargetPlayerID);
    TransferRecord->SetNumberField("timestamp", FDateTime::UtcNow().ToUnixTimestamp());

    FString SerializedRecord;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&SerializedRecord);
    FJsonSerializer::Serialize(TransferRecord.ToSharedRef(), Writer);

    if (UInterverseChainComponent* ChainComp = GetOwner()->FindComponentByClass<UInterverseChainComponent>())
    {
        // Send to blockchain
        ChainComp->RecordTransaction(SerializedRecord);
    }

    OnObjectTransferred.Broadcast(ObjectId, TargetPlayerID, true);
}

bool UInterverseGameLinkComponent::IsGameLinked(const FString& GameId) const
{
    return GameLinks.Contains(GameId);
}

TArray<FString> UInterverseGameLinkComponent::GetLinkedGames() const
{
    TArray<FString> LinkedGames;
    GameLinks.GetKeys(LinkedGames);
    return LinkedGames;
}

bool UInterverseGameLinkComponent::GetGameLinkConfig(const FString& GameId, FGameLinkConfig& OutConfig) const
{
    const FGameLinkConfig* Config = GameLinks.Find(GameId);
    if (Config)
    {
        OutConfig = *Config;
        return true;
    }
    return false;
}

bool UInterverseGameLinkComponent::TransferObjectData(const FTransferredObjectData& ObjectData, const FString& TargetGameId)
{
    if (!GameLinks.Contains(TargetGameId))
    {
        return false;
    }

    const FGameLinkConfig& LinkConfig = GameLinks[TargetGameId];
    if (!LinkConfig.bAllowDirectObjectTransfer)
    {
        return false;
    }

    // Record transfer on blockchain
    RecordTransferOnChain(
        ObjectData.SourceGameId,
        TargetGameId,
        ObjectData.ObjectId,
        ObjectData.SourcePlayerID,
        ObjectData.TargetPlayerID
    );

    OnObjectTransferred.Broadcast(ObjectData.ObjectId, ObjectData.TargetPlayerID, true);
    return true;
}