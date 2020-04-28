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

#include "HoudiniInputObject.h"

#include "HoudiniEngineRuntime.h"
#include "HoudiniAssetComponent.h"
#include "HoudiniSplineComponent.h"
#include "HoudiniInput.h"

#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Landscape.h"
#include "Engine/Brush.h"
#include "GameFramework/Volume.h"

#include "Model.h"
#include "Engine/Polys.h"
#include "Engine/Brush.h"

#include "HoudiniEngineRuntimeUtils.h"

//-----------------------------------------------------------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------------------------------------------------------

//
UHoudiniInputObject::UHoudiniInputObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Transform(FTransform::Identity)
	, Type(EHoudiniInputObjectType::Invalid)
	, InputNodeId(-1)
	, InputObjectNodeId(-1)
	, bHasChanged(false)
	, bNeedsToTriggerUpdate(false)
	, bTransformChanged(false)
{
}

//
UHoudiniInputStaticMesh::UHoudiniInputStaticMesh(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//
UHoudiniInputSkeletalMesh::UHoudiniInputSkeletalMesh(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//
UHoudiniInputSceneComponent::UHoudiniInputSceneComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//
UHoudiniInputMeshComponent::UHoudiniInputMeshComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//
UHoudiniInputInstancedMeshComponent::UHoudiniInputInstancedMeshComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//
UHoudiniInputSplineComponent::UHoudiniInputSplineComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, NumberOfSplineControlPoints(-1)
	, SplineLength(-1.0f)
	, SplineResolution(-1.0f)
	, SplineClosed(false)
{

}

// Returns true if the attached actor's (parent) transform has been modified
bool 
UHoudiniInputSplineComponent::HasActorTransformChanged() const
{
	return false;
}

// Returns true if the attached component's transform has been modified
bool 
UHoudiniInputSplineComponent::HasComponentTransformChanged() const
{
	return false;
}

// Return true if the component itself has been modified
bool 
UHoudiniInputSplineComponent::HasComponentChanged() const 
{
	USplineComponent* SplineComponent = Cast<USplineComponent>(InputObject.LoadSynchronous());

	if (!SplineComponent)
		return false;

	if (SplineClosed != SplineComponent->IsClosedLoop()) 
		return true;

	if (SplineComponent->GetSplineLength() != SplineLength)
		return true;

	if (SplineComponent->GetNumberOfSplinePoints() != NumberOfSplineControlPoints)
		return true;

	for (int32 n = 0; n < SplineComponent->GetNumberOfSplinePoints(); ++n) 
	{
		const FTransform& NextPointTransform = SplineControlPoints[n];

		if (NextPointTransform.GetRotation() != SplineComponent->GetQuaternionAtSplinePoint(n, ESplineCoordinateSpace::World))
			return true;

		if (NextPointTransform.GetScale3D() != SplineComponent->GetScaleAtSplinePoint(n))
			return true;

	}

	return false;
}

bool 
UHoudiniInputSplineComponent::HasSplineComponentChanged(float fCurrentSplineResolution) const
{
	return false;
}

//
UHoudiniInputHoudiniSplineComponent::UHoudiniInputHoudiniSplineComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CurveType(EHoudiniCurveType::Linear)
	, CurveMethod(EHoudiniCurveMethod::CVs)
	, Reversed(false)
{

}

//
UHoudiniInputHoudiniAsset::UHoudiniInputHoudiniAsset(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, AssetOutputIndex(-1)
{

}

//
UHoudiniInputActor::UHoudiniInputActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//
UHoudiniInputLandscape::UHoudiniInputLandscape(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//
UHoudiniInputBrush::UHoudiniInputBrush()
	: CombinedModel(nullptr)
	, bIgnoreInputObject(false)
{

}

//-----------------------------------------------------------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------------------------------------------------------

UObject* 
UHoudiniInputObject::GetObject()
{
	//return InputObject.Get();
	return InputObject.LoadSynchronous();
}

UStaticMesh*
UHoudiniInputStaticMesh::GetStaticMesh() 
{
	return Cast<UStaticMesh>(InputObject.LoadSynchronous());
	//return Cast<UStaticMesh>(InputObject.Get()); 
}

USkeletalMesh*
UHoudiniInputSkeletalMesh::GetSkeletalMesh()
{
	//return Cast<USkeletalMesh>(InputObject.Get()); 
	return Cast<USkeletalMesh>(InputObject.LoadSynchronous());
}

USceneComponent*
UHoudiniInputSceneComponent::GetSceneComponent()
{
	//return Cast<USceneComponent>(InputObject.Get());
	return Cast<USceneComponent>(InputObject.LoadSynchronous());
}

UStaticMeshComponent*
UHoudiniInputMeshComponent::GetStaticMeshComponent()
{
	//return Cast<UStaticMeshComponent>(InputObject.Get()); 
	return Cast<UStaticMeshComponent>(InputObject.LoadSynchronous());
}
UStaticMesh*
UHoudiniInputMeshComponent::GetStaticMesh() 
{ 
	return StaticMesh.Get(); 
}

UInstancedStaticMeshComponent*
UHoudiniInputInstancedMeshComponent::GetInstancedStaticMeshComponent() 
{
	//return Cast<UInstancedStaticMeshComponent>(InputObject.Get()); 
	return Cast<UInstancedStaticMeshComponent>(InputObject.LoadSynchronous());
}

USplineComponent*
UHoudiniInputSplineComponent::GetSplineComponent()
{
	//return Cast<USplineComponent>(InputObject.Get()); 
	return Cast<USplineComponent>(InputObject.LoadSynchronous());
}

UHoudiniSplineComponent*
UHoudiniInputHoudiniSplineComponent::GetCurveComponent()
{
	//return Cast<UHoudiniSplineComponent>(InputObject.Get()); 
	//return Cast<UHoudiniSplineComponent>(InputObject.LoadSynchronous());
	return MyHoudiniSplineComponent;
}

UHoudiniAssetComponent*
UHoudiniInputHoudiniAsset::GetHoudiniAssetComponent()
{ 
	//return Cast<UHoudiniAssetComponent>(InputObject.Get());
	return Cast<UHoudiniAssetComponent>(InputObject.LoadSynchronous());
}

AActor*
UHoudiniInputActor::GetActor()
{ 
	//return Cast<AActor>(InputObject.Get()); 
	return Cast<AActor>(InputObject.LoadSynchronous());
}

ALandscapeProxy*
UHoudiniInputLandscape::GetLandscapeProxy() 
{
	//return Cast<ALandscapeProxy>(InputObject.Get()); 
	return Cast<ALandscapeProxy>(InputObject.LoadSynchronous());
}

void 
UHoudiniInputLandscape::SetLandscapeProxy(UObject* InLandscapeProxy) 
{
	UObject* LandscapeProxy = Cast<UObject>(InLandscapeProxy);
	if (LandscapeProxy)
		InputObject = LandscapeProxy;
}

ABrush*
UHoudiniInputBrush::GetBrush() const
{ 
	return Cast<ABrush>(InputObject.LoadSynchronous());
}


//-----------------------------------------------------------------------------------------------------------------------------
// CREATE METHODS
//-----------------------------------------------------------------------------------------------------------------------------

UHoudiniInputObject *
UHoudiniInputObject::CreateTypedInputObject(UObject * InObject, UObject* InOuter, const FString& InName)
{
	if (!InObject)
		return nullptr;

	UHoudiniInputObject* HoudiniInputObject = nullptr;
	
	EHoudiniInputObjectType InputObjectType = GetInputObjectTypeFromObject(InObject);
	switch (InputObjectType)
	{
		case EHoudiniInputObjectType::Object:
			HoudiniInputObject = UHoudiniInputObject::Create(InObject, InOuter, InName);
			break;

		case EHoudiniInputObjectType::StaticMesh:
			HoudiniInputObject = UHoudiniInputStaticMesh::Create(InObject, InOuter, InName);
			break;

		case EHoudiniInputObjectType::SkeletalMesh:
			HoudiniInputObject = UHoudiniInputSkeletalMesh::Create(InObject, InOuter, InName);
			break;
		case EHoudiniInputObjectType::SceneComponent:
			// Do not create input objects for unknown scene component!
			//HoudiniInputObject = UHoudiniInputSceneComponent::Create(InObject, InOuter, InName);
			break;

		case EHoudiniInputObjectType::StaticMeshComponent:
			HoudiniInputObject = UHoudiniInputMeshComponent::Create(InObject, InOuter, InName);
			break;

		case EHoudiniInputObjectType::InstancedStaticMeshComponent:
			HoudiniInputObject = UHoudiniInputInstancedMeshComponent::Create(InObject, InOuter, InName);
			break;
		case EHoudiniInputObjectType::SplineComponent:
			HoudiniInputObject = UHoudiniInputSplineComponent::Create(InObject, InOuter, InName);
			break;

		case EHoudiniInputObjectType::HoudiniSplineComponent:
			HoudiniInputObject = UHoudiniInputHoudiniSplineComponent::Create(InObject, InOuter, InName);
			break;
		case EHoudiniInputObjectType::HoudiniAssetComponent:
			HoudiniInputObject = UHoudiniInputHoudiniAsset::Create(InObject, InOuter, InName);
			break;
		case EHoudiniInputObjectType::Actor:
			HoudiniInputObject = UHoudiniInputActor::Create(InObject, InOuter, InName);
			break;

		case EHoudiniInputObjectType::Landscape:
			HoudiniInputObject = UHoudiniInputLandscape::Create(InObject, InOuter, InName);
			break;

		case EHoudiniInputObjectType::Brush:
			HoudiniInputObject = UHoudiniInputBrush::Create(InObject, InOuter, InName);
			break;

		case EHoudiniInputObjectType::Invalid:
		default:		
			break;
	}

	return HoudiniInputObject;
}


UHoudiniInputObject *
UHoudiniInputInstancedMeshComponent::Create(UObject * InObject, UObject* InOuter, const FString& InName)
{
	FString InputObjectNameStr = "HoudiniInputObject_ISMC_" + InName;
	FName InputObjectName = MakeUniqueObjectName(InOuter, UHoudiniInputInstancedMeshComponent::StaticClass(), *InputObjectNameStr);

	// We need to create a new object
	UHoudiniInputInstancedMeshComponent * HoudiniInputObject = NewObject<UHoudiniInputInstancedMeshComponent>(
		InOuter, UHoudiniInputInstancedMeshComponent::StaticClass(), InputObjectName, RF_Public | RF_Transactional);

	HoudiniInputObject->Type = EHoudiniInputObjectType::InstancedStaticMeshComponent;
	HoudiniInputObject->Update(InObject);
	HoudiniInputObject->bHasChanged = true;

	return HoudiniInputObject;
}

UHoudiniInputObject *
UHoudiniInputMeshComponent::Create(UObject * InObject, UObject* InOuter, const FString& InName)
{
	FString InputObjectNameStr = "HoudiniInputObject_SMC_" + InName;
	FName InputObjectName = MakeUniqueObjectName(InOuter, UHoudiniInputMeshComponent::StaticClass(), *InputObjectNameStr);

	// We need to create a new object
	UHoudiniInputMeshComponent * HoudiniInputObject = NewObject<UHoudiniInputMeshComponent>(
		InOuter, UHoudiniInputMeshComponent::StaticClass(), InputObjectName, RF_Public | RF_Transactional);

	HoudiniInputObject->Type = EHoudiniInputObjectType::StaticMeshComponent;
	HoudiniInputObject->Update(InObject);
	HoudiniInputObject->bHasChanged = true;

	return HoudiniInputObject;
}

UHoudiniInputObject *
UHoudiniInputSplineComponent::Create(UObject * InObject, UObject* InOuter, const FString& InName)
{
	FString InputObjectNameStr = "HoudiniInputObject_Spline_" + InName;
	FName InputObjectName = MakeUniqueObjectName(InOuter, UHoudiniInputSplineComponent::StaticClass(), *InputObjectNameStr);

	// We need to create a new object
	UHoudiniInputSplineComponent * HoudiniInputObject = NewObject<UHoudiniInputSplineComponent>(
		InOuter, UHoudiniInputSplineComponent::StaticClass(), InputObjectName, RF_Public | RF_Transactional);

	HoudiniInputObject->Type = EHoudiniInputObjectType::SplineComponent;
	HoudiniInputObject->Update(InObject);
	HoudiniInputObject->bHasChanged = true;

	return HoudiniInputObject;
}

UHoudiniInputObject *
UHoudiniInputHoudiniSplineComponent::Create(UObject * InObject, UObject* InOuter, const FString& InName)
{
	FString InputObjectNameStr = "HoudiniInputObject_HoudiniSpline_" + InName;
	FName InputObjectName = MakeUniqueObjectName(InOuter, UHoudiniInputHoudiniSplineComponent::StaticClass(), *InputObjectNameStr);

	// We need to create a new object
	UHoudiniInputHoudiniSplineComponent * HoudiniInputObject = NewObject<UHoudiniInputHoudiniSplineComponent>(
		InOuter, UHoudiniInputHoudiniSplineComponent::StaticClass(), InputObjectName, RF_Public | RF_Transactional);

	HoudiniInputObject->Type = EHoudiniInputObjectType::HoudiniSplineComponent;
	HoudiniInputObject->Update(InObject);
	HoudiniInputObject->bHasChanged = true;

	return HoudiniInputObject;
}

UHoudiniInputObject *
UHoudiniInputHoudiniAsset::Create(UObject * InObject, UObject* InOuter, const FString& InName)
{
	FString InputObjectNameStr = "HoudiniInputObject_HAC_" + InName;
	FName InputObjectName = MakeUniqueObjectName(InOuter, UHoudiniInputHoudiniAsset::StaticClass(), *InputObjectNameStr);

	// We need to create a new object
	UHoudiniInputHoudiniAsset * HoudiniInputObject = NewObject<UHoudiniInputHoudiniAsset>(
		InOuter, UHoudiniInputHoudiniAsset::StaticClass(), InputObjectName, RF_Public | RF_Transactional);

	HoudiniInputObject->Type = EHoudiniInputObjectType::HoudiniAssetComponent;
	HoudiniInputObject->Update(InObject);
	HoudiniInputObject->bHasChanged = true;

	return HoudiniInputObject;
}

UHoudiniInputObject *
UHoudiniInputSceneComponent::Create(UObject * InObject, UObject* InOuter, const FString& InName)
{
	FString InputObjectNameStr = "HoudiniInputObject_SceneComp_" + InName;
	FName InputObjectName = MakeUniqueObjectName(InOuter, UHoudiniInputSceneComponent::StaticClass(), *InputObjectNameStr);

	// We need to create a new object
	UHoudiniInputSceneComponent * HoudiniInputObject = NewObject<UHoudiniInputSceneComponent>(
		InOuter, UHoudiniInputSceneComponent::StaticClass(), InputObjectName, RF_Public | RF_Transactional);

	HoudiniInputObject->Type = EHoudiniInputObjectType::SceneComponent;
	HoudiniInputObject->Update(InObject);
	HoudiniInputObject->bHasChanged = true;

	return HoudiniInputObject;
}

UHoudiniInputObject *
UHoudiniInputLandscape::Create(UObject * InObject, UObject* InOuter, const FString& InName)
{
	FString InputObjectNameStr = "HoudiniInputObject_Landscape_" + InName;
	FName InputObjectName = MakeUniqueObjectName(InOuter, UHoudiniInputLandscape::StaticClass(), *InputObjectNameStr);

	// We need to create a new object
	UHoudiniInputLandscape * HoudiniInputObject = NewObject<UHoudiniInputLandscape>(
		InOuter, UHoudiniInputLandscape::StaticClass(), InputObjectName, RF_Public | RF_Transactional);

	HoudiniInputObject->Type = EHoudiniInputObjectType::Landscape;
	HoudiniInputObject->Update(InObject);
	HoudiniInputObject->bHasChanged = true;

	return HoudiniInputObject;
}

UHoudiniInputBrush *
UHoudiniInputBrush::Create(UObject * InObject, UObject* InOuter, const FString& InName)
{
	FString InputObjectNameStr = "HoudiniInputObject_Brush_" + InName;
	FName InputObjectName = MakeUniqueObjectName(InOuter, UHoudiniInputBrush::StaticClass(), *InputObjectNameStr);

	// We need to create a new object
	UHoudiniInputBrush * HoudiniInputObject = NewObject<UHoudiniInputBrush>(
		InOuter, UHoudiniInputBrush::StaticClass(), InputObjectName, RF_Public | RF_Transactional);

	HoudiniInputObject->Type = EHoudiniInputObjectType::Brush;
	HoudiniInputObject->Update(InObject);
	HoudiniInputObject->bHasChanged = true;

	return HoudiniInputObject;
}

UHoudiniInputObject *
UHoudiniInputActor::Create(UObject * InObject, UObject* InOuter, const FString& InName)
{
	FString InputObjectNameStr = "HoudiniInputObject_Actor_" + InName;
	FName InputObjectName = MakeUniqueObjectName(InOuter, UHoudiniInputActor::StaticClass(), *InputObjectNameStr);

	// We need to create a new object
	UHoudiniInputActor * HoudiniInputObject = NewObject<UHoudiniInputActor>(
		InOuter, UHoudiniInputActor::StaticClass(), InputObjectName, RF_Public | RF_Transactional);

	HoudiniInputObject->Type = EHoudiniInputObjectType::Actor;
	HoudiniInputObject->Update(InObject);
	HoudiniInputObject->bHasChanged = true;

	return HoudiniInputObject;
}

UHoudiniInputObject *
UHoudiniInputStaticMesh::Create(UObject * InObject, UObject* InOuter, const FString& InName)
{
	FString InputObjectNameStr = "HoudiniInputObject_SM_" + InName;
	FName InputObjectName = MakeUniqueObjectName(InOuter, UHoudiniInputStaticMesh::StaticClass(), *InputObjectNameStr);

	// We need to create a new object
	UHoudiniInputStaticMesh * HoudiniInputObject = NewObject<UHoudiniInputStaticMesh>(
		InOuter, UHoudiniInputStaticMesh::StaticClass(), InputObjectName, RF_Public | RF_Transactional);

	HoudiniInputObject->Type = EHoudiniInputObjectType::StaticMesh;
	HoudiniInputObject->Update(InObject);
	HoudiniInputObject->bHasChanged = true;

	return HoudiniInputObject;
}

UHoudiniInputObject *
UHoudiniInputSkeletalMesh::Create(UObject * InObject, UObject* InOuter, const FString& InName)
{
	FString InputObjectNameStr = "HoudiniInputObject_SkelMesh_" + InName;
	FName InputObjectName = MakeUniqueObjectName(InOuter, UHoudiniInputSkeletalMesh::StaticClass(), *InputObjectNameStr);

	// We need to create a new object
	UHoudiniInputSkeletalMesh * HoudiniInputObject = NewObject<UHoudiniInputSkeletalMesh>(
		InOuter, UHoudiniInputSkeletalMesh::StaticClass(), InputObjectName, RF_Public | RF_Transactional);

	HoudiniInputObject->Type = EHoudiniInputObjectType::SkeletalMesh;
	HoudiniInputObject->Update(InObject);
	HoudiniInputObject->bHasChanged = true;

	return HoudiniInputObject;
}

UHoudiniInputObject *
UHoudiniInputObject::Create(UObject * InObject, UObject* InOuter, const FString& InName)
{
	FString InputObjectNameStr = "HoudiniInputObject_" + InName;
	FName InputObjectName = MakeUniqueObjectName(InOuter, UHoudiniInputObject::StaticClass(), *InputObjectNameStr);

	// We need to create a new object
	UHoudiniInputObject * HoudiniInputObject = NewObject<UHoudiniInputObject>(
		InOuter, UHoudiniInputObject::StaticClass(), InputObjectName, RF_Public | RF_Transactional);

	HoudiniInputObject->Type = EHoudiniInputObjectType::Object;
	HoudiniInputObject->Update(InObject);
	HoudiniInputObject->bHasChanged = true;

	return HoudiniInputObject;
}

//-----------------------------------------------------------------------------------------------------------------------------
// DELETE METHODS
//-----------------------------------------------------------------------------------------------------------------------------

void
UHoudiniInputObject::MarkInputNodesForDeletion()
{
	// If valid, mark our input nodes for deletion..	
	if (this->IsA<UHoudiniInputHoudiniAsset>())
	{
		// Unless if we're a HoudiniAssetInput! we don't want to delete the other HDA's node!
		// just invalidate the node IDs!
		InputNodeId = -1;
		InputObjectNodeId = -1;
		return;
	}

	if (InputNodeId >= 0)
	{
		FHoudiniEngineRuntime::Get().MarkNodeIdAsPendingDelete(InputNodeId);
		InputNodeId = -1;
	}

	// ... and the parent OBJ as well to clean up
	if (InputObjectNodeId >= 0)
	{
		FHoudiniEngineRuntime::Get().MarkNodeIdAsPendingDelete(InputObjectNodeId);
		InputObjectNodeId = -1;
	}
}

void
UHoudiniInputObject::BeginDestroy()
{
	// Invalidate and mark our input node for deletion
	MarkInputNodesForDeletion();

	Super::BeginDestroy();
}

//-----------------------------------------------------------------------------------------------------------------------------
// UPDATE METHODS
//-----------------------------------------------------------------------------------------------------------------------------

void
UHoudiniInputObject::Update(UObject * InObject)
{
	InputObject = InObject;
}

void
UHoudiniInputStaticMesh::Update(UObject * InObject)
{
	// Nothing to do
	Super::Update(InObject);

	UStaticMesh* SM = Cast<UStaticMesh>(InObject);
	ensure(SM);
}

void
UHoudiniInputSkeletalMesh::Update(UObject * InObject)
{
	// Nothing to do
	Super::Update(InObject);

	USkeletalMesh* SkelMesh = Cast<USkeletalMesh>(InObject);
	ensure(SkelMesh);
}

void
UHoudiniInputSceneComponent::Update(UObject * InObject)
{	
	Super::Update(InObject);

	USceneComponent* USC = Cast<USceneComponent>(InObject);	
	ensure(USC);
	if (USC)
	{
		Transform = USC->GetComponentTransform();
	}
}


bool 
UHoudiniInputSceneComponent::HasActorTransformChanged() const
{
	// Returns true if the attached actor's (parent) transform has been modified
	USceneComponent* MyComp = Cast<USceneComponent>(InputObject.LoadSynchronous());
	if (!MyComp || MyComp->IsPendingKill())
		return false;

	AActor* MyActor = MyComp->GetOwner();
	if (!MyActor)
		return false;

	return (!ActorTransform.Equals(MyActor->GetTransform()));
}


bool
UHoudiniInputSceneComponent::HasComponentTransformChanged() const
{
	// Returns true if the attached actor's (parent) transform has been modified
	USceneComponent* MyComp = Cast<USceneComponent>(InputObject.LoadSynchronous());
	if (!MyComp || MyComp->IsPendingKill())
		return false;

	return !Transform.Equals(MyComp->GetComponentTransform());
}


bool 
UHoudiniInputSceneComponent::HasComponentChanged() const
{
	// Should return true if the component itself has been modified
	// Should be overriden in child classes
	return false;
}


bool
UHoudiniInputMeshComponent::HasComponentChanged() const
{
	UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(InputObject.LoadSynchronous());
	UStaticMesh* MySM = StaticMesh.Get();

	// Return true if SMC's static mesh has been modified
	return (MySM != SMC->GetStaticMesh());
}


void
UHoudiniInputMeshComponent::Update(UObject * InObject)
{
	Super::Update(InObject);

	UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(InObject);
	
	ensure(SMC);

	if (SMC)
	{
		StaticMesh = TSoftObjectPtr<UStaticMesh>(SMC->GetStaticMesh());

		TArray<UMaterialInterface*> Materials = SMC->GetMaterials();
		for (auto CurrentMat : Materials)
		{
			// TODO: Update material ref here
			FString MatRef;
			MeshComponentsMaterials.Add(MatRef);
		}
	}
}

void
UHoudiniInputInstancedMeshComponent::Update(UObject * InObject)
{
	Super::Update(InObject);

	UInstancedStaticMeshComponent* ISMC = Cast<UInstancedStaticMeshComponent>(InObject);

	ensure(ISMC);

	if (ISMC)
	{
		uint32 InstanceCount = ISMC->GetInstanceCount();
		InstanceTransforms.SetNum(InstanceCount);

		// Copy the instances' transforms		
		for (uint32 InstIdx = 0; InstIdx < InstanceCount; InstIdx++)
		{
			FTransform CurTransform = FTransform::Identity;
			ISMC->GetInstanceTransform(InstIdx, CurTransform);
			InstanceTransforms[InstIdx] = CurTransform;			
		}
	}
}

bool
UHoudiniInputInstancedMeshComponent::HasInstancesChanged() const
{	
	UInstancedStaticMeshComponent* ISMC = Cast<UInstancedStaticMeshComponent>(InputObject.LoadSynchronous());
	if (!ISMC)
		return false;

	uint32 InstanceCount = ISMC->GetInstanceCount();
	if (InstanceTransforms.Num() != InstanceCount)
		return true;

	// Copy the instances' transforms		
	for (uint32 InstIdx = 0; InstIdx < InstanceCount; InstIdx++)
	{
		FTransform CurTransform = FTransform::Identity;
		ISMC->GetInstanceTransform(InstIdx, CurTransform);

		if(!InstanceTransforms[InstIdx].Equals(CurTransform))
			return true;
	}

	return false;
}


bool
UHoudiniInputInstancedMeshComponent::HasComponentTransformChanged() const
{
	if (Super::HasComponentTransformChanged())
		return true;

	return HasInstancesChanged();
}

void
UHoudiniInputSplineComponent::Update(UObject * InObject)
{
	Super::Update(InObject);

	USplineComponent* Spline = Cast<USplineComponent>(InObject);

	ensure(Spline);

	if (Spline)
	{
		NumberOfSplineControlPoints = Spline->GetNumberOfSplinePoints();
		SplineLength = Spline->GetSplineLength();		
		SplineClosed = Spline->IsClosedLoop();

		//SplineResolution = -1.0f;

		SplineControlPoints.SetNumZeroed(NumberOfSplineControlPoints);
		for (int32 Idx = 0; Idx < NumberOfSplineControlPoints; Idx++)
		{
			SplineControlPoints[Idx] = Spline->GetTransformAtSplinePoint(Idx, ESplineCoordinateSpace::Local);
		}		
	}
}

void
UHoudiniInputHoudiniSplineComponent::Update(UObject * InObject)
{
	Super::Update(InObject);

	// We need a strong ref to the spline component to prevent it from being GCed
	MyHoudiniSplineComponent = Cast<UHoudiniSplineComponent>(InObject);

	if (!MyHoudiniSplineComponent || MyHoudiniSplineComponent->IsPendingKill())
	{
		// Use default values
		CurveType = EHoudiniCurveType::Linear;
		CurveMethod = EHoudiniCurveMethod::CVs;
		Reversed = false;
	}
	else
	{
		CurveType = MyHoudiniSplineComponent->GetCurveType();
		CurveMethod = MyHoudiniSplineComponent->GetCurveMethod();
		Reversed = false;//Spline->IsReversed();
	}
}

void
UHoudiniInputHoudiniAsset::Update(UObject * InObject)
{
	Super::Update(InObject);

	UHoudiniAssetComponent* HAC = Cast<UHoudiniAssetComponent>(InObject);

	ensure(HAC);

	if (HAC)
	{
		// TODO: Notify HAC that we're a downstream?

		// TODO: Allow selection of the asset output
		AssetOutputIndex = 0;
	}
}


void
UHoudiniInputActor::Update(UObject * InObject)
{
	Super::Update(InObject);

	AActor* Actor = Cast<AActor>(InObject);
	ensure(Actor);

	if (Actor)
	{
		Transform = Actor->GetTransform();

		// TODO:
		// Iterate on all our scene component, creating child inputs for them if necessary
		//
		// The actor's components that can be sent as inputs
		ActorComponents.Empty();

		TArray<USceneComponent*> AllComponents;
		Actor->GetComponents<USceneComponent>(AllComponents, true);

		int32 CompIdx = 0;
		ActorComponents.SetNum(AllComponents.Num());
		for (USceneComponent * SceneComponent : AllComponents)
		{
			if (!SceneComponent || SceneComponent->IsPendingKill())
				continue;

			UHoudiniInputObject* InputObj = UHoudiniInputObject::CreateTypedInputObject(
				SceneComponent, GetOuter(), Actor->GetName());
			if (!InputObj)
				continue;

			UHoudiniInputSceneComponent* SceneInput = Cast<UHoudiniInputSceneComponent>(InputObj);
			if (!SceneInput)
				continue;

			ActorComponents[CompIdx++] = SceneInput;
		}
		ActorComponents.SetNum(CompIdx);
	}
}

bool 
UHoudiniInputActor::HasActorTransformChanged()
{
	if (!GetActor())
		return false;

	if (!Transform.Equals(GetActor()->GetTransform()))
		return true;

	return false;
}

bool 
UHoudiniInputActor::HasContentChanged() const
{
	return false;
}

bool
UHoudiniInputLandscape::HasActorTransformChanged()
{
	return Super::HasActorTransformChanged();
	//return false;
}

void
UHoudiniInputLandscape::Update(UObject * InObject)
{
	Super::Update(InObject);

	ALandscapeProxy* Landscape = Cast<ALandscapeProxy>(InObject);

	//ensure(Landscape);

	if (Landscape)
	{
		// Nothing to do for landscapes?
	}
}

EHoudiniInputObjectType
UHoudiniInputObject::GetInputObjectTypeFromObject(UObject* InObject)
{
	if (InObject->IsA(USceneComponent::StaticClass()))
	{
		// Handle component inputs
		// UISMC derived from USMC, so always test instances before static meshes
		if (InObject->IsA(UInstancedStaticMeshComponent::StaticClass()))
		{
			return EHoudiniInputObjectType::InstancedStaticMeshComponent;
		}
		else if (InObject->IsA(UStaticMeshComponent::StaticClass()))
		{
			return EHoudiniInputObjectType::StaticMeshComponent;
		}
		else if (InObject->IsA(USplineComponent::StaticClass()))
		{
			return EHoudiniInputObjectType::SplineComponent;
		}
		else if (InObject->IsA(UHoudiniSplineComponent::StaticClass()))
		{
			return EHoudiniInputObjectType::HoudiniSplineComponent;
		}
		else if (InObject->IsA(UHoudiniAssetComponent::StaticClass()))
		{
			return EHoudiniInputObjectType::HoudiniAssetComponent;
		}
		else
		{
			return EHoudiniInputObjectType::SceneComponent;
		}
	}
	else if (InObject->IsA(AActor::StaticClass()))
	{
		// Handle actors
		if (InObject->IsA(ALandscapeProxy::StaticClass()))
		{
			return EHoudiniInputObjectType::Landscape;
		}
		else if (InObject->IsA(ABrush::StaticClass()))
		{
			return EHoudiniInputObjectType::Brush;
		}
		else
		{
			return EHoudiniInputObjectType::Actor;
		}
	}
	else
	{
		if (InObject->IsA(UStaticMesh::StaticClass()))
		{
			return EHoudiniInputObjectType::StaticMesh;
		}
		else if (InObject->IsA(USkeletalMesh::StaticClass()))
		{
			return EHoudiniInputObjectType::SkeletalMesh;
		}
		else
		{
			return EHoudiniInputObjectType::Object;
		}
	}

	return EHoudiniInputObjectType::Invalid;
}



//-----------------------------------------------------------------------------------------------------------------------------
// UHoudiniInputBrush
//-----------------------------------------------------------------------------------------------------------------------------

FHoudiniBrushInfo::FHoudiniBrushInfo(ABrush* InBrushActor)
{
	if (!InBrushActor)
		return;

	BrushActor = InBrushActor;
	CachedTransform = BrushActor->GetActorTransform();
	BrushActor->GetActorBounds(false, CachedOrigin, CachedExtent);
	CachedBrushType = BrushActor->BrushType;

	UModel* Model = BrushActor->Brush;

	// Cache the hash of the surface properties
	if (IsValid(Model) && IsValid(Model->Polys))
	{
		int32 NumPolys = Model->Polys->Element.Num();
		CachedSurfaceHash = 0;
		for(int32 iPoly = 0; iPoly < NumPolys; ++iPoly)
		{
			const FPoly& Poly = Model->Polys->Element[iPoly]; 
			CombinePolyHash(CachedSurfaceHash, Poly);
		}
	}
	else
	{
		CachedSurfaceHash = 0;
	}
	
}

bool FHoudiniBrushInfo::HasChanged() const
{
	if (!BrushActor.IsValid())
		return false;

	// Has the transform changed?
	if (!BrushActor->GetActorTransform().Equals(CachedTransform))
		return true;

	if (BrushActor->BrushType != CachedBrushType)
		return true;

	// Has the actor bounds changed?
	FVector TmpOrigin, TmpExtent;
	BrushActor->GetActorBounds(false, TmpOrigin, TmpExtent);

	if (!(TmpOrigin.Equals(CachedOrigin) && TmpExtent.Equals(CachedExtent) ))
		return true;

	// Is there a tracked surface property that changed?
	UModel* Model = BrushActor->Brush;
	if (IsValid(Model) && IsValid(Model->Polys))
	{
		// Hash the incoming surface properties and compared it against the cached hash.
		int32 NumPolys = Model->Polys->Element.Num();
		uint64 SurfaceHash = 0;
		for (int32 iPoly = 0; iPoly < NumPolys; ++iPoly)
		{
			const FPoly& Poly = Model->Polys->Element[iPoly];
			CombinePolyHash(SurfaceHash, Poly);
		}
		if (SurfaceHash != CachedSurfaceHash)
			return true;
	}
	else
	{
		if (CachedSurfaceHash != 0)
			return true;
	}

	return false;
}

int32 FHoudiniBrushInfo::GetNumVertexIndicesFromModel(const UModel* Model)
{
	const TArray<FBspNode>& Nodes = Model->Nodes;		
	int32 NumIndices = 0;
	// Build the face counts buffer by iterating over the BSP nodes.
	for(const FBspNode& Node : Nodes)
	{
		NumIndices += Node.NumVertices;
	}
	return NumIndices;
}

UModel* UHoudiniInputBrush::GetCachedModel() const
{
	return CombinedModel;
}

bool UHoudiniInputBrush::HasBrushesChanged(const TArray<ABrush*>& InBrushes) const
{
	if (InBrushes.Num() != BrushesInfo.Num())
		return true;

	int32 NumBrushes = BrushesInfo.Num();

	for (int32 InfoIndex = 0; InfoIndex < NumBrushes; ++InfoIndex)
	{
		const FHoudiniBrushInfo& BrushInfo = BrushesInfo[InfoIndex];
		// Has the cached brush actor invalid?
		if (!BrushInfo.BrushActor.IsValid())
			return true;

		// Has there been an order change in the actors list?
		if (InBrushes[InfoIndex] != BrushInfo.BrushActor.Get())
			return true;

		// Has there been any other changes to the brush?
		if (BrushInfo.HasChanged())
			return true;
	}

	// Nothing has changed.
	return false;
}

void UHoudiniInputBrush::UpdateCachedData(UModel* InCombinedModel, const TArray<ABrush*>& InBrushes)
{
	ABrush* InputBrush = GetBrush();
	if (IsValid(InputBrush))
	{
		CachedInputBrushType = InputBrush->BrushType;
	}

	// Cache the combined model aswell as the brushes used to generate this model.
	CombinedModel = InCombinedModel;

	BrushesInfo.SetNumUninitialized(InBrushes.Num());
	for (int i = 0; i < InBrushes.Num(); ++i)
	{
		if (!InBrushes[i])
			continue;
		BrushesInfo[i] = FHoudiniBrushInfo(InBrushes[i]);
	}
}


void
UHoudiniInputBrush::Update(UObject * InObject)
{
	Super::Update(InObject);

	ABrush* BrushActor = GetBrush();
	if (!IsValid(BrushActor))
	{
		bIgnoreInputObject = true;
		return;
	}

	CachedInputBrushType = BrushActor->BrushType;

	bIgnoreInputObject = ShouldIgnoreThisInput();
}

bool
UHoudiniInputBrush::ShouldIgnoreThisInput()
{
	// Invalid brush, should be ignored
	ABrush* BrushActor = GetBrush();
	ensure(BrushActor);
	if (!BrushActor)
		return true;

	// If the BrushType has changed since caching this object, this object cannot be ignored.
	if (CachedInputBrushType != BrushActor->BrushType)
		return false;

	// If it's not an additive brush, we want to ignore it
	bool bShouldBeIgnored = BrushActor->BrushType != EBrushType::Brush_Add;

	// If this is not a static brush (e.g., AVolume), ignore it.
	if (!bShouldBeIgnored)
		bShouldBeIgnored = !BrushActor->IsStaticBrush();

	return bShouldBeIgnored;
}

bool UHoudiniInputBrush::HasContentChanged() const
{
	ABrush* BrushActor = GetBrush();
	ensure(BrushActor);
	
	if (!BrushActor)
		return false;

	if (BrushActor->BrushType != CachedInputBrushType)
		return true;
	
	if (bIgnoreInputObject)
		return false;

	// Find intersecting actors and capture their properties so that
	// we can determine whether something has changed.
	TArray<ABrush*> IntersectingBrushes;
	FindIntersectingSubtractiveBrushes(this, IntersectingBrushes);
	
	if (HasBrushesChanged(IntersectingBrushes))
	{
		return true;
	}

	return false;
}

bool
UHoudiniInputBrush::HasActorTransformChanged()
{
	if (bIgnoreInputObject)
		return false;

	return Super::HasActorTransformChanged();
}


bool UHoudiniInputBrush::FindIntersectingSubtractiveBrushes(const UHoudiniInputBrush* InputBrush, TArray<ABrush*>& OutBrushes)
{
	TArray<AActor*> IntersectingActors;	
	TArray<FBox> Bounds;


	if (!IsValid(InputBrush))
		return false;

	ABrush* BrushActor = InputBrush->GetBrush();
	if (!IsValid(BrushActor))
		return false;


	OutBrushes.Empty();

	Bounds.Add( BrushActor->GetComponentsBoundingBox(true) );

	FHoudiniEngineRuntimeUtils::FindActorsOfClassInBounds(BrushActor->GetWorld(), ABrush::StaticClass(), Bounds, nullptr, IntersectingActors);

	//--------------------------------------------------------------------------------------------------
	// Filter the actors to only keep intersecting subtractive brushes.
	//--------------------------------------------------------------------------------------------------
	for (AActor* Actor : IntersectingActors)
	{
		// Filter out anything that is not a static brush (typically volume actors).
		ABrush* Brush = Cast<ABrush>(Actor);

		// NOTE: The brush actor needs to be added in the correct map/level order
		// together with the subtractive brushes otherwise the CSG operations 
		// will not match the BSP in the level.
		if (Actor == BrushActor)
			OutBrushes.Add(Brush);

		if (!(Brush && Brush->IsStaticBrush()))
			continue;

		if (Brush->BrushType == Brush_Subtract)
			OutBrushes.Add(Brush);
	}

	return true;	
}
