// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "SBAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_SB_API USBAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	virtual void StartInitialLoading() override; // Attempting to setup GAS on bootup - Kimic


};
