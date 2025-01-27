#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InterverseStandardTypes.h"
#include "GameFramework/Actor.h"
#include "Json.h"
#include "InterverseGameLinkComponent.generated.h"

// Declare delegates first, before the component class
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameLinkEstablished, const FString&, TargetGameId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAssetTransferredToPlayer, const FString&, AssetID, const FString&, TargetPlayerID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAssetReceivedFromPlayer, UObject*, ReceivedObject, const FString&, SourcePlayerID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnObjectTransferred, const FString&, ObjectId, const FString&, TargetPlayerID, bool, Success);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectReceived, UObject*, ReceivedObject, const FString&, SourceId);


USTRUCT(BlueprintType)
struct FGameLinkConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Link")
    FString TargetGameId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Link")
    FString TargetGameEndpoint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Link")
    bool bAllowDirectObjectTransfer;

    // Blueprint-exposed object class mappings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Link")
    TMap<TSoftClassPtr<UObject>, TSoftClassPtr<UObject>> ClassMappings;

    // Additional metadata for the link
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Link")
    TMap<FString, FString> LinkMetadata;

    FGameLinkConfig()
    {
        bAllowDirectObjectTransfer = true;
    }
};

USTRUCT(BlueprintType)
struct FTransferredObjectData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Game Link")
    FString ObjectId;

    UPROPERTY(BlueprintReadWrite, Category = "Game Link")
    FString SourcePlayerID;

    UPROPERTY(BlueprintReadWrite, Category = "Game Link")
    FString TargetPlayerID;

    UPROPERTY(BlueprintReadWrite, Category = "Game Link")
    FString SourceGameId;

    UPROPERTY(BlueprintReadWrite, Category = "Game Link")
    FString ObjectClass;

    UPROPERTY(BlueprintReadWrite, Category = "Game Link")
    TMap<FString, FString> ObjectData;

    UPROPERTY(BlueprintReadWrite, Category = "Game Link")
    bool bIsValid;

    FTransferredObjectData()
    {
        bIsValid = false;
    }
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INTERVERSECHAINPLUGIN_API UInterverseGameLinkComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    UInterverseGameLinkComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // Blueprint-callable functions
    UFUNCTION(BlueprintCallable, Category = "Interverse|Game Link")
    bool RegisterGameLink(const FGameLinkConfig& LinkConfig);

    UFUNCTION(BlueprintCallable, Category = "Interverse|Game Link")
    bool TransferGameObject(AActor* Actor, const FString& TargetGameId, const FString& TargetPlayerID);

    UFUNCTION(BlueprintCallable, Category = "Interverse|Game Link")
    bool TransferObjectData(const FTransferredObjectData& ObjectData, const FString& TargetGameId);

    UFUNCTION(BlueprintCallable, Category = "Interverse|Game Link")
    AActor* SpawnReceivedObject(const FTransferredObjectData& ObjectData);

    // Blueprint-accessible getters
    UFUNCTION(BlueprintPure, Category = "Interverse|Game Link")
    bool IsGameLinked(const FString& GameId) const;

    UFUNCTION(BlueprintPure, Category = "Interverse|Game Link")
    TArray<FString> GetLinkedGames() const;

    UFUNCTION(BlueprintCallable, Category = "Interverse|Game Link")
    bool GetGameLinkConfig(const FString& GameId, FGameLinkConfig& OutConfig) const;

    // Blueprint events
    UPROPERTY(BlueprintAssignable, Category = "Interverse|Game Link")
    FOnGameLinkEstablished OnGameLinkEstablished;

    UPROPERTY(BlueprintAssignable, Category = "Interverse|Game Link")
    FOnAssetTransferredToPlayer OnAssetTransferredToPlayer;

    UPROPERTY(BlueprintAssignable, Category = "Interverse|Game Link")
    FOnAssetReceivedFromPlayer OnAssetReceivedFromPlayer;

    UPROPERTY(BlueprintAssignable, Category = "Interverse|Events")
    FOnObjectTransferred OnObjectTransferred;

    UPROPERTY(BlueprintAssignable, Category = "Interverse|Events")
    FOnObjectReceived OnObjectReceived;

protected:
    // Store game link configurations
    UPROPERTY()
    TMap<FString, FGameLinkConfig> GameLinks;

    // Helper functions
    bool SerializeActor(AActor* Actor, FTransferredObjectData& OutData);
    bool DeserializeToActor(const FTransferredObjectData& Data, AActor* OutActor);
    void RecordTransferOnChain(const FString& SourceGameId, const FString& TargetGameId, const FString& ObjectId, const FString& SourcePlayerID, const FString& TargetPlayerID);
    UClass* FindMappedClass(UClass* SourceClass, const FString& TargetGameId) const;
};