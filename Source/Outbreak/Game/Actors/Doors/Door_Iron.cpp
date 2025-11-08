// Fill out your copyright notice in the Description page of Project Settings.


#include "Door_Iron.h"
ADoor_Iron::ADoor_Iron()
{
	// 메시, 타임라인 컴포넌트 할당
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorFrameAsset(
		TEXT("/Script/Engine.StaticMesh'/Game/Art/Components/Door/SM_Doorframe_Wooden.SM_Doorframe_Wooden'")
	);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorMeshAsset(
		TEXT("/Script/Engine.StaticMesh'/Game/Art/Components/Door/SM_Door_Wooden.SM_Door_Wooden'")
	);


	if (DoorFrameAsset.Succeeded())
	{
		DoorFrame->SetStaticMesh(DoorFrameAsset.Object);
	}

	if (DoorMeshAsset.Succeeded())
	{
		DoorMesh->SetStaticMesh(DoorMeshAsset.Object);
	}



	// 문 관련 변수 설정
	bIsDestroyable = false;
	HP = 100; 
	CurrentStatus = EDoorStatus::Close;
}

