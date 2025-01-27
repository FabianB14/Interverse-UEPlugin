#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Http.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "InterverseStandardTypes.h"
#include "InterverseChainDelegates.h"
#include "InterverseChainComponent.generated.h"

// Declare WebSocket delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWebSocketConnected, bool, Success);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWebSocketMessage, const FString&, Message);

UCLASS(ClassGroup=(Blockchain), meta=(BlueprintSpawnableComponent))
class INTERVERSECHAINPLUGIN_API UInterverseChainComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    UInterverseChainComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintCallable, Category = "Interverse|Chain")
    void RecordTransaction(const FString& TransactionData);

    UFUNCTION(BlueprintCallable, Category = "Interverse|Chain")
    void GetLedgerState(FString& OutLedgerState);

    UFUNCTION(BlueprintCallable, Category = "Interverse|Chain")
    void GetTransactionHistory(const FString& Address, TArray<FString>& OutTransactions);

    UPROPERTY(BlueprintAssignable, Category = "Interverse|Events")
    FOnAssetMinted OnAssetMinted;

    UPROPERTY(BlueprintAssignable, Category = "Interverse|Events")
    FOnTransferComplete OnTransferComplete;

    UPROPERTY(BlueprintAssignable, Category = "Interverse|Events")
    FOnBalanceUpdated OnBalanceUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Interverse|Events")
    FOnWebSocketConnected OnWebSocketConnected;

    UPROPERTY(BlueprintAssignable, Category = "Interverse|Events")
    FOnWebSocketMessage OnWebSocketMessage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interverse|Configuration")
    FString NodeUrl;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interverse|Configuration")
    FString GameId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interverse|Configuration")
    FString ApiKey;

    UFUNCTION(BlueprintCallable, Category = "Interverse|Wallet")
    void CreateWallet();

    UFUNCTION(BlueprintCallable, Category = "Interverse|Wallet")
    void GetBalance(const FString& Address);

    UFUNCTION(BlueprintCallable, Category = "Interverse|Assets")
    void MintGameAsset(const FString& OwnerAddress, 
                      const FInterverseBaseProperties& Properties,
                      const TMap<FString, FString>& CustomProperties);

    UFUNCTION(BlueprintCallable, Category = "Interverse|Assets")
    void TransferAsset(const FString& AssetId, 
                      const FString& FromAddress, 
                      const FString& ToAddress);

    UFUNCTION(BlueprintCallable, Category = "Interverse|Assets")
    void GetPlayerAssets(const FString& PlayerAddress);

    UFUNCTION(BlueprintCallable, Category = "Interverse|Network")
    void ConnectWebSocket();

    UFUNCTION(BlueprintCallable, Category = "Interverse|Network")
    void DisconnectWebSocket();

    UFUNCTION(BlueprintCallable, Category = "Interverse|Network")
    void SendWebSocketMessage(const FString& Message);

    UFUNCTION(BlueprintPure, Category = "Interverse|Network")
    bool IsWebSocketConnected() const;

    UFUNCTION(BlueprintCallable, Category = "Interverse|Network")
    void ReconnectWebSocket();

    UFUNCTION(BlueprintPure, Category = "Interverse|Network")
    FString GetConnectionStatus() const;

private:
    FHttpModule* Http;
    TSharedPtr<IWebSocket> WebSocket;

    void OnHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
    void ProcessWebSocketMessage(const FString& Message);
};