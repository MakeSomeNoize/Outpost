// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OPWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_API UOPWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// Sets default values for this subsystem's properties
	UOPWorldSubsystem();

	// USubsystem implementation Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

protected:
	
};