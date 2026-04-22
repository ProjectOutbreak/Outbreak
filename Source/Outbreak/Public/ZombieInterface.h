// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ZombieInterface.generated.h"

struct FZombieData;
// This class does not need to be modified.
UINTERFACE()
class UZombieInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * TODO : CharacterZombie와 ZombieEntityBase를 사용하기 위한 임시 인터페이스
 */
class OUTBREAK_API IZombieInterface
{
	GENERATED_BODY()

public:
	
	virtual FZombieData* GetZombieData() = 0;
	virtual void SetIsAttacking(const bool bInIsAttacking) = 0;
	virtual void SetIsAlert(const bool bInIsAlert) = 0;
	virtual ACharacter* GetCharacter() = 0;
	virtual bool IsAttacking() const = 0;
	virtual void SetEnableAttackCollision(const bool bEnable) = 0;
	
};
