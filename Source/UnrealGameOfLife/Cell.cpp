// Fill out your copyright notice in the Description page of Project Settings.


#include "Cell.h"

float ACell::PlaneUnitScale = 100.0f;

// Sets default values
ACell::ACell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane Mesh"));
	RootComponent = PlaneMesh;

	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text Renderer"));
	TextRenderComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);


	//For debug
	TextRenderComponent->SetRelativeLocation(FVector(0,0,1) * 50);
	TextRenderComponent->SetRelativeRotation(FRotator(90,180,0));
	TextRenderComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponent->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	TextRenderComponent->SetTextRenderColor(FColor::Red);
	SetTextVisibilityState(false);
	

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneObj(TEXT("StaticMesh'/Game/Plane.Plane'"));
	if (PlaneObj.Succeeded())
	{
		PlaneMesh->SetStaticMesh(PlaneObj.Object);  
	}

	//CreateDynamicMaterial();
	PlaneMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlaneMesh->SetGenerateOverlapEvents(false);

	CurrentState = 0;

	GridPosIndex = FIntPoint::ZeroValue;
}

// Called when the game starts or when spawned
void ACell::BeginPlay()
{
	Super::BeginPlay();	
}

void ACell::ChangeState(const uint8 NewState)
{
	CurrentState = NewState;
	ChangeStateColor(CurrentState);
}

void ACell::CreateDynamicMaterial()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialObj(TEXT("Material'/Game/M_Plane.M_Plane'"));
	if (MaterialObj.Succeeded())
	{
		//PlaneMat = MaterialObj.Object;
		UMaterialInterface* TmpMat = MaterialObj.Object;
		if (TmpMat)
		{
			PlaneMaterial = UMaterialInstanceDynamic::Create(TmpMat,PlaneMesh);
			if (PlaneMaterial)
			{
				PlaneMesh->SetMaterial(0,PlaneMaterial);
			} 
		}    	
	}
}

void ACell::CreateDynamicMaterial(UMaterialInterface* Material)
{
	if (Material)
	{
		PlaneMaterial = UMaterialInstanceDynamic::Create(Material,PlaneMesh);
		if (PlaneMaterial)
		{
			PlaneMesh->SetMaterial(0,PlaneMaterial);			
		} 
	}
}

void ACell::SetTextVisibilityState(const bool NewState)
{
	TextRenderComponent->SetVisibility(NewState);
}
