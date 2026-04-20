#include "AASelectionActor.h"

#include "AABlueprintFunctionLibrary.h"
#include "AAAreaActionsComponent.h"
#include "Buildables/FGBuildable.h"
#include "AAAreaActionsComponent.h"

AAASelectionActor::AAASelectionActor() : Super()
{
}

void AAASelectionActor::EnableInput(APlayerController* PlayerController)
{
	Super::EnableInput(PlayerController);
	
	InputComponent->BindAction(TEXT("PrimaryFire"), IE_Pressed, this, &AAASelectionActor::PrimaryFire);	
	InputComponent->BindAction(TEXT("SecondaryFire"), IE_Pressed, this, &AAASelectionActor::SecondaryFire);
}

void AAASelectionActor::PrimaryFire() {
	FHitResult HitResult;
	switch (this->SelectionMode) {
	case EAASelectionMode::SM_Corner:
		if (AreaActionsComponent->RaycastMouseWithRange(HitResult, false, true, true)) {
			AActor* HitActor = HitResult.GetActor();
			if (HitActor && HitActor->IsA<AAACornerIndicator>()) {
				AAACornerIndicator* HitCorner = static_cast<AAACornerIndicator*>(HitActor);
				const int CornerIdx = AreaActionsComponent->CornerIndicators.Find(HitCorner);
				AreaActionsComponent->RemoveCorner(CornerIdx);
			}
			else {
				AreaActionsComponent->AddCorner(FVector2D(HitResult.Location.X, HitResult.Location.Y));
			}
		}
		break;
	case EAASelectionMode::SM_Bottom:
		if (AreaActionsComponent->RaycastMouseWithRange(HitResult, false, true, false)) {
			AActor* HitActor = HitResult.GetActor();
			if (HitActor == AreaActionsComponent->BottomIndicator) {
				AreaActionsComponent->AreaMinZ = AreaActionsComponent->MinZ;
			}
			else {
				AreaActionsComponent->AreaMinZ = HitResult.Location.Z;
				if (AreaActionsComponent->AreaMaxZ < AreaActionsComponent->AreaMinZ) {
					const float Tmp = AreaActionsComponent->AreaMinZ;
					AreaActionsComponent->AreaMinZ = AreaActionsComponent->AreaMaxZ;
					AreaActionsComponent->AreaMaxZ = Tmp;
				}
			}
			AreaActionsComponent->UpdateHeight();
		}
		break;
	case EAASelectionMode::SM_Top:
		if (AreaActionsComponent->RaycastMouseWithRange(HitResult, false, true, false)) {
			AActor* HitActor = HitResult.GetActor();
			if (HitActor == AreaActionsComponent->TopIndicator) {
				AreaActionsComponent->AreaMaxZ = AreaActionsComponent->MaxZ;
			}
			else {
				AreaActionsComponent->AreaMaxZ = HitResult.Location.Z;
				if (AreaActionsComponent->AreaMaxZ < AreaActionsComponent->AreaMinZ) {
					const float Tmp = AreaActionsComponent->AreaMinZ;
					AreaActionsComponent->AreaMinZ = AreaActionsComponent->AreaMaxZ;
					AreaActionsComponent->AreaMaxZ = Tmp;
				}
			}
			AreaActionsComponent->UpdateHeight();
		}
		break;
	case EAASelectionMode::SM_Building:
		if (AreaActionsComponent->RaycastMouseWithRange(HitResult, true, true, true)) {
			AActor* HitActor = HitResult.GetActor();
			if (HitActor && HitActor->IsA<AFGBuildable>()) {
				if (AreaActionsComponent->ExtraActors.Contains(HitActor)) {
					AreaActionsComponent->ExtraActors.Remove(HitActor);
				}
				else {
					AreaActionsComponent->ExtraActors.Add(HitActor);
				}
				AreaActionsComponent->UpdateExtraActors();
			}
		}
		break;
	default:
		break;
	}
}

void AAASelectionActor::SecondaryFire() {
	AreaActionsComponent->ToggleBuildMenu();
	this->Destroy();
}