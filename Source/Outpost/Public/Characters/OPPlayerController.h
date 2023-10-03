// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_API AOPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Sets default values for this player controller's properties
	AOPPlayerController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*
	 A custom version of UWidgetBlueprintLibrary::SetInputMode_GameOnly(), that doesn't change the viewport mouse capture mode.
	 @param	bInConsumeCaptureMouseDown	Leave this "false" to avoid issues with mouse clicks.
	 */
	UFUNCTION(BlueprintCallable, Category = "OPPlayerController")
		void CustomGameOnlyInputMode(bool bInConsumeCaptureMouseDown);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};