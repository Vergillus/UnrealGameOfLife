// Fill out your copyright notice in the Description page of Project Settings.


#include "GOFManager.h"

#include "Cell.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGOFManager::AGOFManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootComp;

	MapWidth = 25;
	MapDepth = 25;

	CellScale = 1.0f;

	RndChosenTileNum = 8;
	
	bCanStartGame = false;
	bShowGridCoords = false;
}

// Called when the game starts or when spawned
void AGOFManager::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(),0);
	PlayerController->EnableInput(PlayerController);

	TileSize = ACell::PlaneUnitScale * CellScale;

	ChooseRandomTile(RndChosenTileNum);
}

void AGOFManager::OnConstruction(const FTransform& Transform)
{
	GenerateGridCells();
	ManageCamera();	
}

// Called every frame
void AGOFManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerController->WasInputKeyJustReleased(EKeys::SpaceBar))
	{
		bCanStartGame = true;
	}

	if (bCanStartGame)
	{
		GameOfLife();	
	}
	
	FVector MouseWorldLocation;
	FVector MouseDir;	
	
	PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation,MouseDir);

	//UE_LOG(LogTemp,Warning,TEXT("MOuse Pos: %s"), *MouseWorldLocation.ToString());

	int32 X;
	int32 Y;	

	if(PlayerController->WasInputKeyJustReleased(EKeys::LeftMouseButton))
	{
		if (LocationToGrid(MouseWorldLocation,X,Y))
		{
			int32 Index = GetIndex(X,Y);
			CellGrid[Index]->ChangeState(static_cast<uint8>(ECellState::State_ALIVE));			
		}	
	}
	else if (PlayerController->WasInputKeyJustReleased(EKeys::RightMouseButton))
	{
		if (LocationToGrid(MouseWorldLocation,X,Y))
		{
			int32 Index = GetIndex(X,Y);
			CellGrid[Index]->ChangeState(static_cast<uint8>(ECellState::State_DEAD));			
		}	
	}	
}

void AGOFManager::GenerateGridCells()
{
	if(MapWidth <= 0 || MapDepth <= 0) return;

	for(ACell* Cell : CellGrid)
	{
		Cell->Destroy();
	}
	CellGrid.Empty();

	for (auto ChildComp : ChildActors)
	{
		if (ChildComp)
		{
			ChildComp->DestroyComponent();			
		}		
	}
	ChildActors.Empty();	

	//RegisterAllComponents();
	
	for(int x = 0; x < MapDepth; x++)
	{
		for(int y = 0; y < MapWidth; y++)
		{
			const FVector Position = FVector(x * CellScale * ACell::PlaneUnitScale, y * CellScale * ACell::PlaneUnitScale,0.0f);
			
			FString Str = "Cell_" + FString::FromInt(x) + "_" + FString::FromInt(y); 
			FName Name = (*Str);
			
			UChildActorComponent* Child = NewObject<UChildActorComponent>(this);
			if (Child)
			{	
				Child->RegisterComponent();
				Child->SetChildActorClass(ACell::StaticClass());
				Child->SetWorldLocation(Position);
				Child->SetWorldScale3D(FVector(CellScale));
				Child->GetChildActor()->Rename(*Name.ToString());
				
				ChildActors.Add(Child);
				
				auto ChildCell = Cast<ACell>(Child->GetChildActor());				
				if (ChildCell)
				{
					ChildCell->SetGridPosIndex(x,y);
					
					ChildCell->CreateDynamicMaterial(PlaneMaterial);
					ChildCell->SetTextVisibilityState(bShowGridCoords);					
					
					CellGrid.Add(ChildCell);					
				}				
			}			
			
			/*ACell* Cell = GetWorld()->SpawnActor<ACell>(CellToSpawn,Position,FRotator::ZeroRotator,SpawnParameters);
			if (Cell)
			{
			Cell->SetGridPosIndex(x,y);
			CellGrid.Add(Cell);
			}*/
			
		}
	}	
}

void AGOFManager::GameOfLife()
{
	
	for (int i = 0; i < CellGrid.Num(); ++i)
	{
		const int32 X = CellGrid[i]->GetGridPosIndex().X;
		const int32 Y = CellGrid[i]->GetGridPosIndex().Y;		

		// skip the edges
		if((X == 0 || X == MapDepth - 1)) continue;
		if((Y == 0 || Y == MapWidth - 1)) continue;
		
		uint8 AliveNeighbourCnt = 0;
		
		for (int k = -1; k<= 1;k++)
		{
			for (int j = -1; j<= 1; j++)
			{
				if(i == 0 && j == 0) continue;
			
				const int Index = GetIndex(X + k,Y + j);

				AliveNeighbourCnt += CellGrid[Index]->GetCurrentState();				
			}			
		}


		// Game of Life Rules
		if (CellGrid[i]->GetCurrentState() == static_cast<uint8>(ECellState::State_ALIVE) && AliveNeighbourCnt < 2) //Loneliness
		{
			CellGrid[i]->ChangeState(static_cast<uint8>(ECellState::State_DEAD));
		}
		else if (CellGrid[i]->GetCurrentState() == static_cast<uint8>(ECellState::State_ALIVE) && AliveNeighbourCnt > 3) //Overpopulation
		{
			CellGrid[i]->ChangeState(static_cast<uint8>(ECellState::State_DEAD));
		}
		else if (CellGrid[i]->GetCurrentState() == static_cast<uint8>(ECellState::State_DEAD) && AliveNeighbourCnt == 3) // Birth
		{
			CellGrid[i]->ChangeState(static_cast<uint8>(ECellState::State_ALIVE));
		}
		else // Statis
		{
			CellGrid[i]->ChangeState(CellGrid[i]->GetCurrentState());
		}
	}	
}

int AGOFManager::GetIndex(const int32 X, const int32 Y)
{
	return X * MapWidth + Y ;
}

void AGOFManager::ManageCamera()
{	
	if (CameraActor)
	{
		FVector NewCameraPos = CellGrid[CellGrid.Num() - 1]->GetActorLocation();
		NewCameraPos = NewCameraPos * 0.5;
		NewCameraPos.Z = 1000;
		
		CameraActor->SetActorLocation(NewCameraPos);

		CameraActor->GetCameraComponent()->OrthoWidth = (GetWidth() + GetHeight()) * 0.7f; //FMath::Min(MapWidth, MapDepth) * 2.0f  / CellScale  ;		
	}
}

bool AGOFManager::LocationToGrid(const FVector& Location, int32& X, int32& Y)
{
	const FVector ActorLoc = GetActorLocation();

	const float TileOffset = TileSize * 0.5f;
	
	X = FMath::Floor((Location.X - ActorLoc.X + TileOffset ) / TileSize);	
	Y = FMath::Floor((Location.Y - ActorLoc.Y + TileOffset ) / TileSize);	

	return ValidTile(X,Y);
}

void AGOFManager::ChooseRandomTile(const int32 Val)
{
	TArray<ACell*> Tmp = CellGrid;

	//ShuffleArray(Tmp);

	int32 Counter = Val;
	
	while(Counter >= 0)
	{
		int32 X = FMath::RandRange(1, MapDepth - 1);
		int32 Y = FMath::RandRange(1, MapWidth - 1);

		int32 Index = GetIndex(X,Y);

		int32 CoordX = Tmp[Index]->GetGridPosIndex().X;
		int32 CoordY = Tmp[Index]->GetGridPosIndex().Y;

		if(CoordX == 0 || CoordX == MapDepth - 1) continue;
		if(CoordY == 0 || CoordY == MapWidth - 1) continue;

		Tmp[Index]->ChangeState(static_cast<uint8>(ECellState::State_ALIVE));

		Counter--;
	}

}

/*void AGOFManager::ShuffleArray(TArray<ACell*>& InArr)
{
	if (InArr.Num() > 0)
	{
		int32 LastIndex = InArr.Num() - 1;
		for (int i = 0; i <= LastIndex; ++i)
		{
			int32 Index = FMath::RandRange(i,LastIndex);
			if (i != Index) 
			{
				InArr.Swap(i,Index);
			}
		}
	}	
}*/




