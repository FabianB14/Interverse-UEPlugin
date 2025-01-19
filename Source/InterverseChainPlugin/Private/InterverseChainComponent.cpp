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

    if (NodeUrl.IsEmpty() || GameId.IsEmpty() || ApiKey.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Interverse: Missing configuration parameters"));
        return;
    }

    ConnectWebSocket();
}

void UInterverseChainComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    DisconnectWebSocket();
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
    if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
    {
        FModuleManager::Get().LoadModule("WebSockets");
    }

    FString WsUrl = NodeUrl;
    WsUrl.ReplaceInline(TEXT("http://"), TEXT("ws://"));
    WsUrl.ReplaceInline(TEXT("https://"), TEXT("wss://"));
    WsUrl.Append(TEXT("/ws"));

    WebSocket = FWebSocketsModule::Get().CreateWebSocket(WsUrl, TEXT("verse-protocol"));

    WebSocket->OnConnected().AddLambda([this]() {
        // Send initial handshake
        FString HandshakeMessage = FString::Printf(TEXT("{\"type\":\"handshake\",\"game_id\":\"%s\"}"), *GameId);
        WebSocket->Send(HandshakeMessage);
        
        AsyncTask(ENamedThreads::GameThread, [this]() {
            OnWebSocketConnected.Broadcast(true);
        });
    });

    WebSocket->OnMessage().AddLambda([this](const FString& MessageStr) {
        AsyncTask(ENamedThreads::GameThread, [this, MessageStr]() {
            OnWebSocketMessage.Broadcast(MessageStr);
            ProcessWebSocketMessage(MessageStr);
        });
    });

    WebSocket->Connect();
}

void UInterverseChainComponent::DisconnectWebSocket()
{
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
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
                    OnAssetMinted.Broadcast(Asset);
                });
            }
        }
        else if (URL.Contains(TEXT("assets/transfer")))
        {
            const FString AssetId = (*DataObject)->GetStringField("asset_id");
            const bool Success = JsonObject->GetBoolField("success");
            
            AsyncTask(ENamedThreads::GameThread, [this, AssetId, Success]() {
                OnTransferComplete.Broadcast(AssetId, Success);
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
                        OnAssetMinted.Broadcast(Asset);
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
                    OnTransferComplete.Broadcast(AssetId, Success);
                });
            }
        }
    }
}

void UInterverseChainComponent::SendWebSocketMessage(const FString& Message)
{
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        WebSocket->Send(Message);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("WebSocket not connected. Cannot send message."));
    }
}

bool UInterverseChainComponent::IsWebSocketConnected() const
{
    return WebSocket.IsValid() && WebSocket->IsConnected();
}

void UInterverseChainComponent::ReconnectWebSocket()
{
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