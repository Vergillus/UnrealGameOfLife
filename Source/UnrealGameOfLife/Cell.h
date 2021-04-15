// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "Cell.generated.h"


UENUM()
enum class ECellState : uint8
{
	State_DEAD,
	State_ALIVE,
	NONE
};


UCLASS()
class UNREALGAMEOFLIFE_API ACell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	

	UPROPERTY()
	UStaticMeshComponent* PlaneMesh;

	UPROPERTY(BlueprintReadOnly)
	UMaterialInstanceDynamic* PlaneMaterial;

	uint8 CurrentState:1;	//The 1 in the end indicates that it only uses 1 bit

	UPROPERTY(BlueprintReadOnly)
	FIntPoint GridPosIndex;

	UPROPERTY(BlueprintReadOnly)
	UTextRenderComponent* TextRenderComponent;

public:	
	void ChangeState(const uint8 NewState);

	static float PlaneUnitScale;
	
	FORCEINLINE FIntPoint GetGridPosIndex() const { return GridPosIndex; }
	FORCEINLINE void SetGridPosIndex(const uint32 X, const uint32 Y) { GridPosIndex.X = X; GridPosIndex.Y = Y; TextRenderComponent->Text = FText::FromString(FString::FromInt(GridPosIndex.X) + "__" + FString::FromInt(GridPosIndex.Y));}

	FORCEINLINE void ChangeStateColor(const float NewColorVal) const { PlaneMaterial->SetScalarParameterValue("ColorLerpVal",NewColorVal);}

	FORCEINLINE uint8 GetCurrentState() const {return CurrentState;}
	
	void CreateDynamicMaterial();

	void CreateDynamicMaterial(UMaterialInterface* Material);

	void SetTextVisibilityState(const bool NewState);
	
};
