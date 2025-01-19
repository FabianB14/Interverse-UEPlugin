#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InterverseChainComponent.h"
#include "InterverseChainDelegates.h"
#include "InterverseMiningComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INTERVERSECHAINPLUGIN_API UInterverseMiningComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    UInterverseMiningComponent();
    
    UPROPERTY(BlueprintAssignable, Category = "Interverse|Mining")
    FOnMiningComplete OnMiningComplete;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse|Mining", meta=(ClampMin="0.0"))
    float MiningPower;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse|Mining", meta=(ClampMin="1.0"))
    float MiningInterval;

    UPROPERTY(BlueprintReadWrite, Category = "Interverse|Mining")
    bool bAutoStartMining;

    UFUNCTION(BlueprintCallable, Category = "Interverse|Mining")
    void StartMining(const FString& MinerAddress);

    UFUNCTION(BlueprintCallable, Category = "Interverse|Mining")
    void StopMining();

    UFUNCTION(BlueprintCallable, Category = "Interverse|Mining")
    float CalculateReward();

    UFUNCTION(BlueprintPure, Category = "Interverse|Mining")
    bool IsMining() const;

    UFUNCTION(BlueprintCallable, Category = "Interverse|Mining")
    void SetMiningDifficulty(float NewDifficulty);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    FTimerHandle MiningTimerHandle;
    FString CurrentMinerAddress;
    float CurrentDifficulty;
    
    void OnMiningTick();
    void InitializeMiningParameters();
};