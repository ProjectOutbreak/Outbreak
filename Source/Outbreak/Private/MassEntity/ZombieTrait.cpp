// Fill out your copyright notice in the Description page of Project Settings.

#include "MassEntity/ZombieTrait.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntity/ZombieHealthFragment.h"
#include "MassEntity/ZombieStateFragment.h"

void UZombieTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FZombieHealthFragment>();
	BuildContext.AddFragment<FZombieStateFragment>();
}
