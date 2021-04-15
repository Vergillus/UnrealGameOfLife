// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Cell.h"
#include "GameFramework/Actor.h"
#include "GOFManager.generated.h"

UCLASS()
class UNREALGAMEOFLIFE_API AGOFManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGOFManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(BlueprintReadOnly)
	USceneComponent* RootComp;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= "Map Settings | Map")
	int32 MapWidth;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= "Map Settings | Map")
	int32 MapDepth;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= "Map Settings | Map")
	float CellScale;

	UPROPERTY(BlueprintReadOnly)
	float TileSize;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= "Map Settings | Random Tile")
	int32 RndChosenTileNum;

	// Reference to camera in the scene.
	// Assigned in the editor
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= "Map Settings | Camera")
	ACameraActor* CameraActor;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= "Map Settings | Material")
	UMaterialInterface* PlaneMaterial;

	// Converts World Location to Grid Location
	bool LocationToGrid(const FVector& Location, int32& X, int32& Y);

	FORCEINLINE bool ValidTile(const int32 X, const int32 Y) const {return (X >= 0 && X< MapDepth) && (Y >= 0 && Y < MapWidth);}

	FORCEINLINE float GetWidth() const  {return MapWidth * ACell::PlaneUnitScale * CellScale;}
	FORCEINLINE float GetHeight() const  {return MapDepth * ACell::PlaneUnitScale * CellScale;}

	UPROPERTY(BlueprintReadWrite)
	bool bCanStartGame;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Map Settings | Debug")
	bool bShowGridCoords;

private:

	UPROPERTY()
	TArray<class ACell*> CellGrid;

	UPROPERTY()
	TArray<UChildActorComponent*> ChildActors;
	
	UFUNCTION(BlueprintCallable)
    void GenerateGridCells();	

	UFUNCTION(BlueprintCallable)
	void GameOfLife();

	UFUNCTION(BlueprintCallable)
	int GetIndex(const int32 X, const int32 Y);

	void ManageCamera();

	UPROPERTY()
	APlayerController* PlayerController;

	void ChooseRandomTile(const int32 Val);

	//void ShuffleArray(TArray<ACell*>& InArr);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
