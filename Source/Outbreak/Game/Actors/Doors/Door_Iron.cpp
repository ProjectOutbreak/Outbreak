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
	static ConstructorHelpers::FObjectFinder<UCurveFloat> DoorTimeLineCompAsset(
		TEXT("/Script/Engine.CurveFloat'/Game/Blueprints/Components/Door/DoorCurveFloat.DoorCurveFloat'")
	);

	if (DoorFrameAsset.Succeeded())
	{
		DoorFrame->SetStaticMesh(DoorFrameAsset.Object);
	}

	if (DoorMeshAsset.Succeeded())
	{
		DoorMesh->SetStaticMesh(DoorMeshAsset.Object);
	}

	// 로드 성공 여부 확인 후 커브 변수(부모 클래스의)에 할당
	if (DoorTimeLineCompAsset.Succeeded())
	{
		// ADoorBase에 선언된 DoorTimelineFloatCurve 변수에 할당
		DoorTimelineFloatCurve = DoorTimeLineCompAsset.Object;
	}

	// 문 관련 변수 설정
	bIsDestroyable = false;
	HP = 100; 
	CurrentStatus = EDoorStatus::Close;
}

