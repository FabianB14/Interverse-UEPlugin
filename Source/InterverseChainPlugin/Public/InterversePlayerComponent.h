#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InterverseStandardTypes.h"
#include "InterversePlayerComponent.generated.h"

USTRUCT(BlueprintType)
struct FInterversePlayerID
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Interverse|Player")
    FString GlobalPlayerID;

    UPROPERTY(BlueprintReadOnly, Category = "Interverse|Player")
    FString CurrentGameID;

    UPROPERTY(BlueprintReadOnly, Category = "Interverse|Player")
    FString PlayerName;

    UPROPERTY(BlueprintReadOnly, Category = "Interverse|Player")
    FString LastKnownGameID;

    UPROPERTY(BlueprintReadOnly, Category = "Interverse|Player")
    FDateTime LastActiveTime;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerIdentified, const FInterversePlayerID&, PlayerID);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INTERVERSECHAINPLUGIN_API UInterversePlayerComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    UInterversePlayerComponent();

    // Blueprint functions
    UFUNCTION(BlueprintCallable, Category = "Interverse|Player")
    void InitializePlayer(const FString& GameSpecificID, const FString& DisplayName);

    UFUNCTION(BlueprintCallable, Category = "Interverse|Player")
    FInterversePlayerID GetPlayerID() const { return CurrentPlayerID; }

    UFUNCTION(BlueprintPure, Category = "Interverse|Player")
    static FString GenerateGlobalPlayerID(const FString& GameSpecificID);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Interverse|Player")
    FOnPlayerIdentified OnPlayerIdentified;

protected:
    UPROPERTY()
    FInterversePlayerID CurrentPlayerID;

    virtual void BeginPlay() override;
    
    // Helper functions
    void RegisterPlayerWithChain();
    void UpdatePlayerActivity();
};