#include "InterverseMiningComponent.h"
#include "TimerManager.h"

UInterverseMiningComponent::UInterverseMiningComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    MiningPower = 1.0f;
    MiningInterval = 60.0f;
    CurrentDifficulty = 1.0f;
    bAutoStartMining = false;
}

void UInterverseMiningComponent::BeginPlay()
{
    Super::BeginPlay();
    InitializeMiningParameters();

    if (bAutoStartMining)
    {
        StartMining(TEXT("auto_miner"));
    }
}

void UInterverseMiningComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    StopMining();
}

void UInterverseMiningComponent::InitializeMiningParameters()
{
    // Initialize mining parameters based on current network state
    CurrentDifficulty = 1.0f;  // Default difficulty, should be fetched from network
}

void UInterverseMiningComponent::StartMining(const FString& MinerAddress)
{
    if (!GetWorld() || !MinerAddress.IsEmpty())
        return;

    CurrentMinerAddress = MinerAddress;
    GetWorld()->GetTimerManager().SetTimer(
        MiningTimerHandle,
        this,
        &UInterverseMiningComponent::OnMiningTick,
        MiningInterval,
        true
    );
}

void UInterverseMiningComponent::StopMining()
{
    if (!GetWorld())
        return;

    GetWorld()->GetTimerManager().ClearTimer(MiningTimerHandle);
    CurrentMinerAddress.Empty();
}

float UInterverseMiningComponent::CalculateReward()
{
    const float BaseRate = 0.1f;
    const float TimeMultiplier = FMath::Min(MiningInterval / 3600.0f, 2.0f);
    const float DifficultyMultiplier = 1.0f / CurrentDifficulty;
    const float RandomBonus = FMath::RandRange(0.0f, 0.05f);
    
    return BaseRate * MiningPower * TimeMultiplier * DifficultyMultiplier * (1.0f + RandomBonus);
}

bool UInterverseMiningComponent::IsMining() const
{
    return !CurrentMinerAddress.IsEmpty();
}

void UInterverseMiningComponent::SetMiningDifficulty(float NewDifficulty)
{
    CurrentDifficulty = FMath::Max(NewDifficulty, 0.1f);
}

void UInterverseMiningComponent::OnMiningTick()
{
    if (CurrentMinerAddress.IsEmpty())
        return;

    float Reward = CalculateReward();
    
    // Generate a unique block hash (this is just for demonstration)
    FString BlockHash = FGuid::NewGuid().ToString();
    
    OnMiningComplete.Broadcast(Reward, BlockHash);
}