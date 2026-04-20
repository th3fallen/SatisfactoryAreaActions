#include "AreaActionsModule.h"

#include "Patching/NativeHookManager.h"
#include "Equipment/FGBuildGun.h"
#include "AABlueprintFunctionLibrary.h"

DEFINE_LOG_CATEGORY(LogAreaActions);

void FAreaActionsModule::StartupModule() {
#if !WITH_EDITOR
	AFGBuildGun* BuildGunCDO = GetMutableDefault<AFGBuildGun>();
	SUBSCRIBE_METHOD_VIRTUAL_AFTER(AFGEquipment::BeginPlay, BuildGunCDO, [](AFGEquipment* Self)
	{
		if(!Self->IsA<AFGBuildGun>()) return;
		if(Self->FindComponentByClass<UAAAreaActionsComponent>()) return;
		UE_LOG(LogAreaActions, Display, TEXT("BuildGun BeginPlay"));
		UAAAreaActionsComponent* AreaActionsComponent = NewObject<UAAAreaActionsComponent>(Self);
		AreaActionsComponent->RegisterComponent();	 
		AreaActionsComponent->SetIsReplicated(true);
		Self->AddOwnedComponent(AreaActionsComponent);
	});
	SUBSCRIBE_METHOD(AFGCharacterPlayer::UnEquipAllEquipment, [](auto& Scope, AFGCharacterPlayer* Self)
	{
		if(Self)
		{
			if(AFGBuildGun* BuildGun = Self->GetBuildGun())
			{
				if(UAAAreaActionsComponent* Component = BuildGun->FindComponentByClass<UAAAreaActionsComponent>())
				{
					BuildGun->RemoveOwnedComponent(Component);
					Component->DestroyComponent();
				}
			}
		}
	});
#endif
}


IMPLEMENT_GAME_MODULE(FAreaActionsModule, AreaActions);
