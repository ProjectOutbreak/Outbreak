// Fill out your copyright notice in the Description page of Project Settings.

#include "MassEntity/ZombieChaseTrait.h"
#include "MassEntityTemplateRegistry.h"
#include "Data/ZombieMassFragments.h"

void UZombieChaseTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	const FZombieChaseTargetSharedFragment TargetShared;
	BuildContext.AddSharedFragment(FSharedStruct::Make(TargetShared));
	
	BuildContext.AddFragment<FZombieChaseTargetFragment>();
}
