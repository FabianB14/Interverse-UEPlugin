#include "InterverseChainComponent.h"
#include "InterverseCompatibility.h"
#include "JsonObjectConverter.h"
#include "WebSocketsModule.h"

UInterverseChainComponent::UInterverseChainComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    Http = &FHttpModule::Get();
}

void UInterverseChainComponent::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("InterverseChainComponent BeginPlay"));

    if (NodeUrl.IsEmpty() || GameId.IsEmpty() || ApiKey.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Missing configuration - NodeUrl: %s, GameId: %s, ApiKey is %s"), 
            *NodeUrl, *GameId, ApiKey.IsEmpty() ? TEXT("empty") : TEXT("set"));
        return;
    }

    ConnectWebSocket();
}

void UInterverseChainComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UE_LOG(LogTemp, Log, TEXT("InterverseChainComponent EndPlay"));
    DisconnectWebSocket();
    Super::EndPlay(EndPlayReason);
}

void UInterverseChainComponent::CreateWallet()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &UInterverseChainComponent::OnHttpResponseReceived);
    
    // Use compatibility layer for endpoint
    FString Endpoint = InterverseCompat::GetEndpointPath("wallet/create");
    Request->SetURL(FString::Printf(TEXT("%s/%s"), *NodeUrl, *Endpoint));
    
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "application/json");
    Request->SetHeader("X-API-Key", ApiKey);
    Request->ProcessRequest();
}

void UInterverseChainComponent::GetBalance(const FString& Address)
{
    if (Address.IsEmpty()) return;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &UInterverseChainComponent::OnHttpResponseReceived);
    
    FString Endpoint = InterverseCompat::GetEndpointPath(FString::Printf(TEXT("wallet/%s/balance"), *Address));
    Request->SetURL(FString::Printf(TEXT("%s/%s"), *NodeUrl, *Endpoint));
    
    Request->SetVerb("GET");
    Request->SetHeader("X-API-Key", ApiKey);
    Request->ProcessRequest();
}

void UInterverseChainComponent::MintGameAsset(
    const FString& OwnerAddress,
    const FInterverseBaseProperties& Properties,
    const TMap<FString, FString>& CustomProperties)
{
    if (!Properties.IsValid() || OwnerAddress.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid asset properties or owner address"));
        return;
    }

    // Use compatibility layer to convert properties
    TSharedPtr<FJsonObject> AssetJson = InterverseCompat::ConvertAssetToJson(Properties, CustomProperties);
    AssetJson->SetStringField("owner", OwnerAddress);
    AssetJson->SetStringField("game_id", GameId);
    
    // Convert to string
    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(AssetJson.ToSharedRef(), Writer);
    
    // Use compatibility layer for endpoint
    FString Endpoint = InterverseCompat::GetEndpointPath("assets/mint");
    
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &UInterverseChainComponent::OnHttpResponseReceived);
    Request->SetURL(FString::Printf(TEXT("%s/%s"), *NodeUrl, *Endpoint));
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "application/json");
    Request->SetHeader("X-API-Key", ApiKey);
    Request->SetContentAsString(RequestBody);
    Request->ProcessRequest();
}

void UInterverseChainComponent::TransferAsset(
    const FString& AssetId,
    const FString& FromAddress,
    const FString& ToAddress)
{
    if (AssetId.IsEmpty() || FromAddress.IsEmpty() || ToAddress.IsEmpty()) return;

    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    JsonObject->SetStringField("asset_id", AssetId);
    JsonObject->SetStringField("from_address", FromAddress);
    JsonObject->SetStringField("to_address", ToAddress);
    
    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    
    FString Endpoint = InterverseCompat::GetEndpointPath("assets/transfer");
    
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &UInterverseChainComponent::OnHttpResponseReceived);
    Request->SetURL(FString::Printf(TEXT("%s/%s"), *NodeUrl, *Endpoint));
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "application/json");
    Request->SetHeader("X-API-Key", ApiKey);
    Request->SetContentAsString(RequestBody);
    Request->ProcessRequest();
}

void UInterverseChainComponent::GetPlayerAssets(const FString& PlayerAddress)
{
    if (PlayerAddress.IsEmpty()) return;

    FString Endpoint = InterverseCompat::GetEndpointPath(FString::Printf(TEXT("assets/player/%s"), *PlayerAddress));
    
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &UInterverseChainComponent::OnHttpResponseReceived);
    Request->SetURL(FString::Printf(TEXT("%s/%s"), *NodeUrl, *Endpoint));
    Request->SetVerb("GET");
    Request->SetHeader("X-API-Key", ApiKey);
    Request->ProcessRequest();
}

void UInterverseChainComponent::ConnectWebSocket()
{
    // Load WebSocket module for UE5
    const FName WebSocketModuleName = TEXT("WebSockets");
    if (!FModuleManager::Get().IsModuleLoaded(WebSocketModuleName))
    {
        FModuleManager::Get().LoadModule(WebSocketModuleName);
        if (!FModuleManager::Get().IsModuleLoaded(WebSocketModuleName))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to load WebSockets module"));
            return;
        }
    }

    // Validate configuration
    if (NodeUrl.IsEmpty() || ApiKey.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("NodeUrl or ApiKey is empty"));
        return;
    }

    // Construct WebSocket URL for UE5
    FString WsUrl = NodeUrl;
    WsUrl.ReplaceInline(TEXT("http://"), TEXT("ws://"));
    WsUrl.ReplaceInline(TEXT("https://"), TEXT("wss://"));
    
    // Remove any trailing slashes for UE5
    while (WsUrl.EndsWith(TEXT("/")))
    {
        WsUrl.LeftChopInline(1);
    }
    
    // Add /ws endpoint and API key
    WsUrl = FString::Printf(TEXT("%s/ws?api_key=%s"), *WsUrl, *ApiKey);
    
    UE_LOG(LogTemp, Log, TEXT("UE5 Connecting to WebSocket URL: %s"), *WsUrl);

    // UE5 specific headers
    TMap<FString, FString> Headers;
    Headers.Add(TEXT("Sec-WebSocket-Protocol"), TEXT("verse-protocol"));
    Headers.Add(TEXT("Upgrade"), TEXT("websocket"));
    Headers.Add(TEXT("Connection"), TEXT("Upgrade"));
    Headers.Add(TEXT("Sec-WebSocket-Version"), TEXT("13"));

    // Create WebSocket with UE5's implementation
    WebSocket = FWebSocketsModule::Get().CreateWebSocket(WsUrl, TEXT("verse-protocol"), Headers);
    
    if (!WebSocket.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create WebSocket in UE5"));
        return;
    }

    // UE5 connection handler with improved lambda capture
    WebSocket->OnConnected().AddLambda([this, WsUrl]() {
        UE_LOG(LogTemp, Log, TEXT("UE5 WebSocket Connected to: %s"), *WsUrl);
        
        // Use UE5's task graph for game thread execution
        FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
            FSimpleDelegateGraphTask::FDelegate::CreateLambda([this]()
            {
                OnWebSocketConnected.Broadcast(true);
            }),
            TStatId(),
            nullptr,
            ENamedThreads::GameThread
        );
        
        // Send initial handshake
        if (!GameId.IsEmpty())
        {
            const FString HandshakeMessage = FString::Printf(TEXT("{\"type\":\"handshake\",\"game_id\":\"%s\"}"), *GameId);
            WebSocket->Send(HandshakeMessage);
            UE_LOG(LogTemp, Log, TEXT("Sent UE5 handshake: %s"), *HandshakeMessage);
        }
    });

    // UE5 error handler
    WebSocket->OnConnectionError().AddLambda([this](const FString& Error) {
        UE_LOG(LogTemp, Error, TEXT("UE5 WebSocket Connection Error: %s"), *Error);
        
        FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
            FSimpleDelegateGraphTask::FDelegate::CreateLambda([this]()
            {
                OnWebSocketConnected.Broadcast(false);
            }),
            TStatId(),
            nullptr,
            ENamedThreads::GameThread
        );
    });

    // UE5 message handler with improved type safety
    WebSocket->OnMessage().AddLambda([this](const FString& MessageStr) {
        UE_LOG(LogTemp, Verbose, TEXT("UE5 Received WebSocket message: %s"), *MessageStr);
        
        FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
            FSimpleDelegateGraphTask::FDelegate::CreateLambda([this, MessageStr]()
            {
                OnWebSocketMessage.Broadcast(MessageStr);
                ProcessWebSocketMessage(MessageStr);
            }),
            TStatId(),
            nullptr,
            ENamedThreads::GameThread
        );
    });

    // UE5 close handler with status code
    WebSocket->OnClosed().AddLambda([this](int32 StatusCode, const FString& Reason, bool bWasClean) {
        UE_LOG(LogTemp, Warning, TEXT("UE5 WebSocket Closed - Status: %d, Reason: %s, Clean: %d"), 
            StatusCode, *Reason, bWasClean);
            
        // Optionally attempt reconnection if wasn't a clean close
        if (!bWasClean)
        {
            UE_LOG(LogTemp, Warning, TEXT("UE5 WebSocket connection was not clean, scheduling reconnect"));
            // Schedule reconnection attempt after delay
            FTimerHandle ReconnectTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
                ReconnectTimerHandle,
                this,
                &UInterverseChainComponent::ReconnectWebSocket,
                5.0f,  // 5 second delay
                false
            );
        }
    });

    UE_LOG(LogTemp, Log, TEXT("UE5 Initiating WebSocket connection"));
    WebSocket->Connect();
}

void UInterverseChainComponent::DisconnectWebSocket()
{
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        UE_LOG(LogTemp, Log, TEXT("Disconnecting WebSocket"));
        WebSocket->Close();
    }
}

void UInterverseChainComponent::OnHttpResponseReceived(
    FHttpRequestPtr Request,
    FHttpResponsePtr Response,
    bool bSuccess)
{
    if (!bSuccess || !Response.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to receive response"));
        return;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

    if (FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        FString URL = Request->GetURL();
        
        // Get the data object which contains the actual response
        const TSharedPtr<FJsonObject>* DataObject;
        if (!JsonObject->TryGetObjectField("data", DataObject))
        {
            UE_LOG(LogTemp, Warning, TEXT("Response missing data field"));
            return;
        }

        if (URL.Contains(TEXT("assets/mint")))
        {
            FInterverseAsset Asset;
            if (InterverseCompat::ConvertJsonToAsset(*DataObject, Asset))
            {
                AsyncTask(ENamedThreads::GameThread, [this, Asset]() {
                    OnAssetMinted.Broadcast(Asset, TEXT(""));
                });
            }
        }
        else if (URL.Contains(TEXT("assets/transfer")))
        {
            const FString AssetId = (*DataObject)->GetStringField("asset_id");
            const bool Success = JsonObject->GetBoolField("success");
            
            AsyncTask(ENamedThreads::GameThread, [this, AssetId, Success]() {
            OnTransferComplete.Broadcast(AssetId, TEXT(""), Success);  
    });
        }
        else if (URL.Contains(TEXT("wallet/balance")))
        {
            const float Balance = (*DataObject)->GetNumberField("balance");
            
            AsyncTask(ENamedThreads::GameThread, [this, Balance]() {
                OnBalanceUpdated.Broadcast(Balance);
            });
        }
    }
}

void UInterverseChainComponent::ProcessWebSocketMessage(const FString& Message)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);

    if (FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        const FString MessageType = JsonObject->GetStringField("type");

        if (MessageType == "asset_update")
        {
            const TSharedPtr<FJsonObject>* AssetObject;
            if (JsonObject->TryGetObjectField("asset", AssetObject))
            {
                FInterverseAsset Asset;
                if (InterverseCompat::ConvertJsonToAsset(*AssetObject, Asset))
                {
                    AsyncTask(ENamedThreads::GameThread, [this, Asset]() {
                        OnAssetMinted.Broadcast(Asset, TEXT(""));
                    });
                }
            }
        }
        else if (MessageType == "balance_update")
        {
            const TSharedPtr<FJsonObject>* DataObject;
            if (JsonObject->TryGetObjectField("data", DataObject))
            {
                const float NewBalance = (*DataObject)->GetNumberField("balance");
                AsyncTask(ENamedThreads::GameThread, [this, NewBalance]() {
                    OnBalanceUpdated.Broadcast(NewBalance);
                });
            }
        }
        else if (MessageType == "transfer_complete")
        {
            const TSharedPtr<FJsonObject>* DataObject;
            if (JsonObject->TryGetObjectField("data", DataObject))
            {
                const FString AssetId = (*DataObject)->GetStringField("asset_id");
                const bool Success = (*DataObject)->GetBoolField("success");
                AsyncTask(ENamedThreads::GameThread, [this, AssetId, Success]() {
                     OnTransferComplete.Broadcast(AssetId, TEXT(""), Success);
                });
            }
        }
    }
}

void UInterverseChainComponent::RecordTransaction(const FString& TransactionData)
{
    if (!TransactionData.IsEmpty())
    {
        // Send transaction data to blockchain
        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
        Request->OnProcessRequestComplete().BindUObject(this, &UInterverseChainComponent::OnHttpResponseReceived);
        
        FString Endpoint = InterverseCompat::GetEndpointPath(TEXT("transactions/record"));
        Request->SetURL(FString::Printf(TEXT("%s/%s"), *NodeUrl, *Endpoint));
        Request->SetVerb(TEXT("POST"));
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        Request->SetHeader(TEXT("X-API-Key"), ApiKey);
        Request->SetContentAsString(TransactionData);
        Request->ProcessRequest();
    }
}

void UInterverseChainComponent::GetLedgerState(FString& OutLedgerState)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindLambda([&OutLedgerState](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
    {
        if (bSuccess && Response.IsValid())
        {
            OutLedgerState = Response->GetContentAsString();
        }
    });
    
    Request->SetURL(FString::Printf(TEXT("%s/chain"), *NodeUrl));
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("X-API-Key"), ApiKey);
    Request->ProcessRequest();
}

void UInterverseChainComponent::GetTransactionHistory(const FString& Address, TArray<FString>& OutTransactions)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindLambda([&OutTransactions](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
    {
        if (bSuccess && Response.IsValid())
        {
            TSharedPtr<FJsonObject> JsonObject;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
            
            if (FJsonSerializer::Deserialize(Reader, JsonObject))
            {
                const TArray<TSharedPtr<FJsonValue>>* TransactionsArray;
                if (JsonObject->TryGetArrayField(TEXT("transactions"), TransactionsArray))
                {
                    for (const auto& TransactionValue : *TransactionsArray)
                    {
                        FString TransactionString;
                        if (TransactionValue->TryGetString(TransactionString))
                        {
                            OutTransactions.Add(TransactionString);
                        }
                    }
                }
            }
        }
    });
    
    Request->SetURL(FString::Printf(TEXT("%s/transactions/%s"), *NodeUrl, *Address));
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("X-API-Key"), ApiKey);
    Request->ProcessRequest();
}

void UInterverseChainComponent::SendWebSocketMessage(const FString& Message)
{
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        UE_LOG(LogTemp, Verbose, TEXT("Sending WebSocket message: %s"), *Message);
        WebSocket->Send(Message);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot send message - WebSocket not connected"));
    }
}

bool UInterverseChainComponent::IsWebSocketConnected() const
{
    return WebSocket.IsValid() && WebSocket->IsConnected();
}

void UInterverseChainComponent::ReconnectWebSocket()
{
    UE_LOG(LogTemp, Log, TEXT("Attempting to reconnect WebSocket"));
    DisconnectWebSocket();
    ConnectWebSocket();
}

FString UInterverseChainComponent::GetConnectionStatus() const
{
    if (!WebSocket.IsValid())
    {
        return TEXT("Not Initialized");
    }
    else if (WebSocket->IsConnected())
    {
        return TEXT("Connected");
    }
    else
    {
        return TEXT("Disconnected");
    }
}