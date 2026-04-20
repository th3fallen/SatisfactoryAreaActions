// Fill out your copyright notice in the Description page of Project Settings.

#include "Actions/AAMassDismantle.h"

#include "Buildables/FGBuildable.h"
#include "AAAreaActionsComponent.h"
#include "FGCharacterPlayer.h"
#include "FGDismantleInterface.h"
#include "FGInventoryLibrary.h"


void AAAMassDismantle::Dismantle()
{
    for (AActor* Actor : this->Actors)
    {
        AFGBuildable* Buildable = Cast<AFGBuildable>(Actor);
        if (!Buildable) continue;

        TArray<FInventoryStack> BuildingRefunds;
        IFGDismantleInterface::Execute_GetDismantleRefund(Buildable, BuildingRefunds, false);
        IFGDismantleInterface::Execute_Dismantle(Buildable);

        this->Refunds.Append(BuildingRefunds);
    }
    UFGInventoryLibrary::ConsolidateInventoryItems(this->Refunds);

    ShowGiveRefundsWidget();
}

void AAAMassDismantle::GiveRefunds()
{
	AFGCharacterPlayer* Player = this->AreaActionsComponent->GetPlayerCharacter();
    AFGCrate* Crate = GetWorld()->SpawnActor<AFGCrate>(
        CrateClass,
        Player->GetActorLocation() + Player->GetActorForwardVector() * CrateDistance,
        FRotator(0, Player->GetActorRotation().Yaw + 90, 0));

    int CrateInventorySlots = 0;
    for (FInventoryStack& Stack : this->Refunds)
    {
        const int ItemStackSize = UFGItemDescriptor::GetStackSize(Stack.Item.ItemClass);
        CrateInventorySlots += Stack.NumItems / ItemStackSize + !!(Stack.NumItems % ItemStackSize);
    }
    Crate->GetInventory()->Resize(CrateInventorySlots);
    Crate->GetInventory()->AddStacks(this->Refunds);
    this->AreaActionsComponent->ClearSelection();
    this->Done();
}
