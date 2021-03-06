/*
* Copyright (c) <2018> Side Effects Software Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. The name of Side Effects Software may not be used to endorse or
*    promote products derived from this software without specific prior
*    written permission.
*
* THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
* NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "CoreMinimal.h"

#include "HoudiniAssetComponent.h"
#include "Engine/Blueprint.h"

#if WITH_EDITOR
	#include "Subsystems/AssetEditorSubsystem.h"
#endif

#include "HoudiniAssetBlueprintComponent.generated.h"

class USCS_Node;

UCLASS(NotBlueprintType, Experimental, meta=(BlueprintSpawnableComponent, DisplayName="Houdini Asset"))
class HOUDINIENGINERUNTIME_API UHoudiniAssetBlueprintComponent : public UHoudiniAssetComponent
{
	GENERATED_BODY()

public:
	UHoudiniAssetBlueprintComponent(const FObjectInitializer & ObjectInitializer);

#if WITH_EDITOR
	// Sync certain variables of this HoudiniAssetComponent to the blueprint generated class.
	// This is typically used when the Blueprint definition is being edited and the HAC cook
	// took place in a transient HoudiniAssetComponent. Certain properties needs to be copied
	// from the transient component back to the Blueprint generated class in order to be retained
	// as part of the Client MeetingBlueprint definition.
	
	void CopyStateToTemplateComponent();

	void CopyStateFromTemplateComponent(UHoudiniAssetBlueprintComponent* FromComponent);

	void CopyDetailsFromComponent(
		UHoudiniAssetBlueprintComponent* FromComponent, 
		bool CopyInputs, 
		bool bClearChanged, 
		bool bInCanDeleteHoudiniNodes, 
		EObjectFlags SetFlags=RF_NoFlags, 
		EObjectFlags ClearFlags=RF_NoFlags);

	virtual bool HasOpenEditor() const override;
	IAssetEditorInstance* FindEditorInstance() const;
	AActor* GetPreviewActor() const; 
#endif

	virtual UHoudiniAssetComponent* GetCachedTemplate() const override;

	//------------------------------------------------------------------------------------------------
	// Supported Features
	//------------------------------------------------------------------------------------------------

	// Some features may be unavaible depending on the context in which the Houdini Asset Component
	// has been instantiated.

	virtual bool CanDeleteHoudiniNodes() const override;

	void SetCanDeleteHoudiniNodes(bool bInCanDeleteNodes);

	virtual bool IsValidComponent() const override;

	virtual bool IsInputTypeSupported(EHoudiniInputType InType) const override;
	virtual bool IsOutputTypeSupported(EHoudiniOutputType InType) const override;

	virtual bool IsProxyStaticMeshEnabled() const override;

	//------------------------------------------------------------------------------------------------
	// Notifications
	//------------------------------------------------------------------------------------------------

	//virtual void BroadcastPreAssetCook() override;
	virtual void OnPrePreCook() override;
	virtual void OnPostPreCook() override;
	virtual void OnPreOutputProcessing() override;
	virtual void OnPostOutputProcessing() override;
	virtual void NotifyHoudiniRegisterCompleted() override;
	virtual void NotifyHoudiniPreUnregister() override;
	virtual void NotifyHoudiniPostUnregister() override;

	//------------------------------------------------------------------------------------------------
	// UActorComponent overrides
	//------------------------------------------------------------------------------------------------
#if WITH_EDITOR
	virtual void OnComponentCreated() override;
#endif

	virtual void OnRegister() override;

	virtual void BeginDestroy() override;

	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	// Refer USplineComponent for a decent reference on how to use component instance data.
	virtual TStructOnScope<FActorComponentInstanceData> GetComponentInstanceData() const override;
	void ApplyComponentInstanceData(struct FHoudiniAssetBlueprintInstanceData* ComponentInstanceData, const bool bPostUCS);

	//------------------------------------------------------------------------------------------------
	// UHoudiniAssetComponent overrides
	//------------------------------------------------------------------------------------------------
	
	FHoudiniAssetComponentEvent OnParametersChangedEvent;
	FHoudiniAssetComponentEvent OnHoudiniAssetChangedEvent;

	virtual void HoudiniEngineTick() override;
	virtual void OnFullyLoaded() override;
	virtual void OnTemplateParametersChanged() override;
	virtual void OnHoudiniAssetChanged() override;
	virtual void RegisterHoudiniComponent(UHoudiniAssetComponent* InComponent) override;
	

	//------------------------------------------------------------------------------------------------
	// Blueprint functions
	//------------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category="Houdini Asset Component")
	bool HasParameter(FString Name);

	UFUNCTION(BlueprintCallable, Category="Houdini Asset Component")
	void SetFloatParameter(FString Name, float Value, int Index=0);
	
	UFUNCTION(BlueprintCallable, Category="Houdini Asset Component")
	void SetToggleValueAt(FString Name, bool Value, int Index=0);

protected:

	template<typename ParamT, typename ValueT>
	void SetTypedValueAt(const FString& Name, ValueT& Value, int Index=0);
	
	void OnTemplateParametersChangedHandler(UHoudiniAssetComponent* ComponentTemplate);
	void InvalidateData();


	USceneComponent* FindOwnerComponentByName(FName ComponentName) const;
	USceneComponent* FindActorComponentByName(AActor * InActor, FName ComponentName) const;

	USCS_Node* FindSCSNodeForTemplateComponent(USimpleConstructionScript* SCS, const UActorComponent* InComponent) const;
	USCS_Node* FindSCSNodeForTemplateComponentInClassHierarchy(const UActorComponent* InComponent) const;
#if WITH_EDITOR
	USCS_Node* FindSCSNodeForInstanceComponent(USimpleConstructionScript* SCS, const UActorComponent* InComponent) const;
#endif // WITH_EDITOR

	void CachePreviewState();
	void CacheBlueprintData();

	USimpleConstructionScript* GetSCS() const;

	//// The output translation has finished.
	//void OnOutputProcessingCompletedHandler(UHoudiniAssetComponent * InComponent);

#if WITH_EDITOR
	//void ReceivedAssetEditorRequestCloseEvent(UObject* Asset, EAssetEditorCloseReason CloseReason);
	TWeakObjectPtr<UAssetEditorSubsystem> CachedAssetEditorSubsystem;
#endif

	TWeakObjectPtr<UBlueprint> CachedBlueprint;
	TWeakObjectPtr<AActor> CachedActorCDO;
	TWeakObjectPtr<UHoudiniAssetBlueprintComponent> CachedTemplateComponent;

	/*UPROPERTY(DuplicateTransient)
	bool bOutputsRequireUpdate;*/
	UPROPERTY()
	bool FauxBPProperty;

	UPROPERTY()
	bool bHoudiniAssetChanged;

	UPROPERTY()
	bool bIsInBlueprintEditor;

	UPROPERTY(Transient, DuplicateTransient)
	bool bCanDeleteHoudiniNodes;

	UPROPERTY(Transient, DuplicateTransient)
	bool bHasRegisteredComponentTemplate;
	
	// This is used to keep track of which SCS variable names correspond to which
	// output objects.
	// This seems like it will cause issues in the map.
	UPROPERTY()
	TMap<FHoudiniOutputObjectIdentifier, FName> CachedOutputNodes;
};


///** Used to keep track of output data and mappings during reconstruction  */
USTRUCT()
struct FHoudiniAssetBlueprintOutput
{
	GENERATED_BODY()

	UPROPERTY()
	int32 OutputIndex;

	UPROPERTY()
	FHoudiniOutputObject OutputObject;

	FHoudiniAssetBlueprintOutput()
		: OutputIndex(INDEX_NONE)
	{

	}
};


/** Used to store HoudiniAssetComponent data during recompile of BP */
USTRUCT()
struct FHoudiniAssetBlueprintInstanceData : public FActorComponentInstanceData
{
	GENERATED_BODY()
public:
	FHoudiniAssetBlueprintInstanceData();
	FHoudiniAssetBlueprintInstanceData(const UHoudiniAssetBlueprintComponent* SourceComponent);

	virtual ~FHoudiniAssetBlueprintInstanceData() = default;

	/*virtual bool ContainsData() const override
	{
		return (HAC != nullptr) || Super::ContainsData();
	}*/

	virtual void ApplyToComponent(UActorComponent* Component, const ECacheApplyPhase CacheApplyPhase) override
	{
		Super::ApplyToComponent(Component, CacheApplyPhase);
		CastChecked<UHoudiniAssetBlueprintComponent>(Component)->ApplyComponentInstanceData(this, (CacheApplyPhase == ECacheApplyPhase::PostUserConstructionScript));
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	// Persist all the required properties for being able to recook the HoudiniAsset from its existing state.
	UPROPERTY()
	UHoudiniAsset* HoudiniAsset;

	UPROPERTY()
	int32 AssetId;

	UPROPERTY()
	EHoudiniAssetState AssetState;

	// Subasset index
	UPROPERTY()
	uint32 SubAssetIndex;

	UPROPERTY()
	uint32 AssetCookCount;

	UPROPERTY()
	bool bHasBeenLoaded;

	UPROPERTY()
	bool bHasBeenDuplicated;

	UPROPERTY()
	bool bPendingDelete;

	UPROPERTY()
	bool bRecookRequested;

	UPROPERTY()
	bool bRebuildRequested;

	UPROPERTY()
	bool bEnableCooking;

	UPROPERTY()
	bool bForceNeedUpdate;

	UPROPERTY()
	bool bLastCookSuccess;

	/*UPROPERTY(DuplicateTransient)
	TSet<UHoudiniAssetComponent*> DownstreamHoudiniAssets;*/

	UPROPERTY()
	FGuid ComponentGUID;

	UPROPERTY()
	FGuid HapiGUID;

	UPROPERTY()
	bool bRegisteredComponentTemplate;

	// Name of the component from which this 
	// data was copied. Used for debugging purposes.
	UPROPERTY()
	FString SourceName;

	UPROPERTY()
	TMap<FHoudiniOutputObjectIdentifier, FHoudiniAssetBlueprintOutput> Outputs;

	UPROPERTY()
	TArray<UHoudiniInput*> Inputs;
};

