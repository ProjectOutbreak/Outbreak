#include "Pawn/OutbreakSpectatorPawn.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"

AOutbreakSpectatorPawn::AOutbreakSpectatorPawn()
{
    bReplicates = true;
}

void AOutbreakSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindKey(EKeys::Q, IE_Pressed, this, &AOutbreakSpectatorPawn::ServerPrevPlayer);
    PlayerInputComponent->BindKey(EKeys::E, IE_Pressed, this, &AOutbreakSpectatorPawn::ServerNextPlayer);
    PlayerInputComponent->BindKey(EKeys::R, IE_Pressed, this, &AOutbreakSpectatorPawn::ServerToggleFreeCam);
}

void AOutbreakSpectatorPawn::ServerNextPlayer_Implementation() { UpdateSpectateTarget(1); }
void AOutbreakSpectatorPawn::ServerPrevPlayer_Implementation() { UpdateSpectateTarget(-1); }

void AOutbreakSpectatorPawn::ServerToggleFreeCam_Implementation()
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->SetViewTargetWithBlend(this, 0.2f);
        CurrentTargetIndex = -1;
    }
}

void AOutbreakSpectatorPawn::UpdateSpectateTarget(int32 Direction)
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    const AGameStateBase* GS = GetWorld()->GetGameState();
    if (!PC || !GS) return;

    TArray<APlayerState*> Players = GS->PlayerArray;
    TArray<APlayerState*> ValidTargets;
    
    for (APlayerState* PS : Players)
    {
        if (PS && PS->GetPawn() && PS->GetPawn() != this)
        {
            ValidTargets.Add(PS);
        }
    }

    if (ValidTargets.Num() > 0)
    {
        CurrentTargetIndex = (CurrentTargetIndex + Direction + ValidTargets.Num()) % ValidTargets.Num();
        AActor* TargetActor = ValidTargets[CurrentTargetIndex]->GetPawn();
        
        PC->SetViewTargetWithBlend(TargetActor, 0.3f);
    }
}