// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeBase.h"

#include "Kismet/GameplayStatics.h"
#include "Util/ColorConstants.h"

// Sets default values
AMazeBase::AMazeBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MazeWidth = 5;
	MazeHeight = 5;
	bUseMeshSizes = true;
	bGenerateInConstructionScript = false;
	bRegenerateMazeInConstructionScript = false;

	CenterSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Center Comp"));
	RootComponent = CenterSceneComp;

	FloorSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Floor Comp"));
	FloorSceneComp->SetupAttachment(CenterSceneComp);

	InnerWallSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Inner Wall Comp"));
	InnerWallSceneComp->SetupAttachment(CenterSceneComp);

	OuterWallSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Outer Wall Comp"));
	OuterWallSceneComp->SetupAttachment(CenterSceneComp);

	InnerCornerSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Inner Corner Comp"));
	InnerCornerSceneComp->SetupAttachment(CenterSceneComp);

	OuterCornerSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Outer Corner Comp"));
	OuterCornerSceneComp->SetupAttachment(CenterSceneComp);

}

// Called when the game starts or when spawned
void AMazeBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMazeBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (bRegenerateMazeInConstructionScript)
	{
		RegenerateMaze();
		
		bRegenerateMazeInConstructionScript = false;
		
	}
	
	
}

void AMazeBase::ClearMaze()
{

	MazeData.Empty();
	FloorContainer.Empty();
	InnerWallContainer.Empty();
	OuterWallContainer.Empty();
	InnerCornerContainer.Empty();
	OuterCornerContainer.Empty();
	RoomCenters.Empty();
	RemovedRoomDoorwayTransforms.Empty();
	
	if (!FloorSceneComp->GetAttachChildren().IsEmpty())
	{
		for (int32 i = FloorSceneComp->GetAttachChildren().Num(); i > 0; i--)
		{
			if (!FloorSceneComp->GetAttachChildren()[i - 1])
			{
				continue;
			}
			//FloorSceneComp->GetAttachChildren()[i - 1]->UnregisterComponent();
			FloorSceneComp->GetAttachChildren()[i - 1]->DestroyComponent();
		}
	}

	if (!InnerWallSceneComp->GetAttachChildren().IsEmpty())
	{
		for (int32 i = InnerWallSceneComp->GetAttachChildren().Num(); i > 0; i--)
		{
			if (!InnerWallSceneComp->GetAttachChildren()[i - 1])
			{
				continue;
			}
			//InnerWallSceneComp->GetAttachChildren()[i - 1]->UnregisterComponent();
			InnerWallSceneComp->GetAttachChildren()[i - 1]->DestroyComponent();
		}
	}

	if (!OuterWallSceneComp->GetAttachChildren().IsEmpty())
	{
		for (int32 i = OuterWallSceneComp->GetAttachChildren().Num(); i > 0; i--)
		{
			if (!OuterWallSceneComp->GetAttachChildren()[i - 1])
			{
				continue;
			}
			//OuterWallSceneComp->GetAttachChildren()[i - 1]->UnregisterComponent();
			OuterWallSceneComp->GetAttachChildren()[i - 1]->DestroyComponent();
		}
	}

	if (!InnerCornerSceneComp->GetAttachChildren().IsEmpty())
	{
		for (int32 i = InnerCornerSceneComp->GetAttachChildren().Num(); i > 0; i--)
		{
			if (!InnerCornerSceneComp->GetAttachChildren()[i - 1])
			{
				continue;
			}
			//InnerCornerSceneComp->GetAttachChildren()[i - 1]->UnregisterComponent();
			InnerCornerSceneComp->GetAttachChildren()[i - 1]->DestroyComponent();
		}
	}

	if (!OuterCornerSceneComp->GetAttachChildren().IsEmpty())
	{
		for (int32 i = OuterCornerSceneComp->GetAttachChildren().Num(); i > 0; i--)
		{
			if (!OuterCornerSceneComp->GetAttachChildren()[i - 1])
			{
				continue;
			}
			//OuterCornerSceneComp->GetAttachChildren()[i - 1]->UnregisterComponent();
			OuterCornerSceneComp->GetAttachChildren()[i - 1]->DestroyComponent();
		}
	}
}

void AMazeBase::GenerateFloors()
{
	if (bUseMeshSizes)
	{
		if (!FloorMeshSize)
		{
			UE_LOG(LogTemp, Error, TEXT("The maze is using the floor mesh size but does not have a floor mesh set"));
			return;
		}
		FloorSize = FloorMeshSize->GetBoundingBox().GetSize();
	}

	FTransform FloorTileTransform;
	
	for (int32 i = 0; i < MazeWidth; i++)
	{
		for (int32 j = 0; j < MazeHeight; j++)
		{
			// setup cell data
			FloorTileTransform.SetLocation(FVector(
				((i * FloorSize.X) - (FloorSize.X * (MazeWidth - 1)) / 2),
				((j * FloorSize.Y) - (FloorSize.Y * (MazeHeight - 1)) / 2),
				0));
			FMazeCellData TempCellData;
			TempCellData.MazeCellTransform = FloorTileTransform;
			TempCellData.bAlgorithmHasVisited = false;
			TempCellData.MazeCellCoordinates = FIntPoint(i, j);

			// create new child actor component and apply chosen class. Only works in instanced asset
			CreateChildActorInstance(FloorTileTransform,
				FloorActorClass,
				FloorSceneComp,
				FName(TEXT("Floor " + FString::FromInt(i) + ", " + FString::FromInt(j))),
				&FloorContainer);

			// Set cell position enum based on cell location
			if (i == 0 && j == 0)
			{
				TempCellData.CellPosition = (ECellPosition::ECP_BottomLeftCorner);
			}
			else if (i == 0 && j == (MazeHeight - 1))
			{
				TempCellData.CellPosition =(ECellPosition::ECP_BottomRightCorner);
			}
			else if (i == (MazeWidth - 1) && j == 0)
			{
				TempCellData.CellPosition =(ECellPosition::ECP_TopLeftCorner);
			}
			else if (i == (MazeWidth - 1) && j == (MazeHeight - 1))
			{
				TempCellData.CellPosition =(ECellPosition::ECP_TopRightCorner);
			}
			else if (j == 0)
			{
				TempCellData.CellPosition =(ECellPosition::ECP_LeftSide);
			}
			else if (j == (MazeHeight - 1))
			{
				TempCellData.CellPosition =(ECellPosition::ECP_RightSide);
			}
			else if (i == 0)
			{
				TempCellData.CellPosition =(ECellPosition::ECP_BottomSide);
			}
			else if (i == (MazeWidth - 1))
			{
				TempCellData.CellPosition =(ECellPosition::ECP_TopSide);
			}
			else
			{
				TempCellData.CellPosition =(ECellPosition::ECP_Normal);
			}
			
			MazeData.Add(FIntPoint(i, j), TempCellData);
		}
	}
}

void AMazeBase::GenerateWalls()
{
	if (bUseMeshSizes)
	{
		if (!InnerWallMeshSize || !OuterWallMeshSize)
		{
			UE_LOG(LogTemp, Error, TEXT("The maze is using the wall mesh size but does not have a wall mesh set"));
			return;
		}
		InnerWallSize = InnerWallMeshSize->GetBoundingBox().GetSize();
		OuterWallSize = OuterWallMeshSize->GetBoundingBox().GetSize();
	}

	FTransform WallTileTransform;

	for (int32 i = 0; i < MazeWidth; i++)
	{
		for (int32 j = 0; j < MazeHeight; j++)
		{
			// set inner walls going x direction
			if (i != MazeWidth - 1)
			{
				WallTileTransform.SetRotation(FRotator(0.f, 90.f, 0.f).Quaternion());
				WallTileTransform.SetLocation(FVector(
					((i * FloorSize.X) - (FloorSize.X * (MazeWidth - 1)) / 2) + (FloorSize.X) / 2,
					(InnerWallSize.X + InnerCornerSize.Y) * j  - (FloorSize.X * (MazeHeight - 1)) / 2,
					(InnerWallSize.Z + FloorSize.Z) / 2));

				CreateChildActorInstance(WallTileTransform,
					InnerWallActorClass,
					InnerWallSceneComp,
					FName(TEXT("Inner Wall X " + FString::FromInt(i) + ", " + FString::FromInt(j))),
					&InnerWallContainer);
			}

			// set inner walls going y direction
			if (j != MazeHeight - 1)
			{
				WallTileTransform.SetRotation(FRotator(0.f, 180.f, 0.f).Quaternion());
				WallTileTransform.SetLocation(FVector(
					(InnerWallSize.X + InnerCornerSize.X) * i - (FloorSize.X * (MazeWidth - 1)) / 2,
					((j * FloorSize.Y) - (FloorSize.Y * (MazeHeight - 1)) / 2) + (FloorSize.Y) / 2,
					(InnerWallSize.Z + FloorSize.Z) / 2));

				CreateChildActorInstance(WallTileTransform,
					InnerWallActorClass,
					InnerWallSceneComp,
					FName(TEXT("Inner Wall Y " + FString::FromInt(i) + ", " + FString::FromInt(j))),
					&InnerWallContainer);
			}
			
			// set outer walls -X 
			if (i == 0)
			{
				WallTileTransform.SetRotation(FRotator(0.f, -90.f, 0.f).Quaternion());
				WallTileTransform.SetLocation(FVector(
					(-((FloorSize.X) + (FloorSize.X * (MazeWidth - 1))) / 2),
					(OuterWallSize.X + OuterCornerSize.Y) * j  - (FloorSize.X * (MazeHeight - 1)) / 2,
					(OuterWallSize.Z + FloorSize.Z) / 2));

				CreateChildActorInstance(WallTileTransform,
					OuterWallActorClass,
					OuterWallSceneComp,
					FName(TEXT("Outer Wall -X " + FString::FromInt(i) + ", " + FString::FromInt(j))),
					&OuterWallContainer);
			}

			// set outer walls +X 
			if (i == (MazeWidth - 1))
			{
				WallTileTransform.SetRotation(FRotator(0.f, 90.f, 0.f).Quaternion());
				WallTileTransform.SetLocation(FVector(
					(((FloorSize.X) + (FloorSize.X * (MazeWidth - 1))) / 2),
					(OuterWallSize.X + OuterCornerSize.Y) * j  - (FloorSize.X * (MazeHeight- 1)) / 2,
					(OuterWallSize.Z + FloorSize.Z) / 2));

				CreateChildActorInstance(WallTileTransform,
					OuterWallActorClass,
					OuterWallSceneComp,
					FName(TEXT("Outer Wall +X " + FString::FromInt(i) + ", " + FString::FromInt(j))),
					&OuterWallContainer);
			}
			
			// Set Outer Walls -Y 
			if (j == 0)
			{
				WallTileTransform.SetRotation(FRotator(0.f, 0.f, 0.f).Quaternion());
				WallTileTransform.SetLocation(FVector(
					(OuterWallSize.X + OuterCornerSize.Y) * i  - (FloorSize.X * (MazeWidth - 1)) / 2,
					(-((FloorSize.X) + (FloorSize.X * (MazeHeight - 1))) / 2),
					(OuterWallSize.Z + FloorSize.Z) / 2));

				CreateChildActorInstance(WallTileTransform,
					OuterWallActorClass,
					OuterWallSceneComp,
					FName(TEXT("Outer Wall -Y " + FString::FromInt(i) + ", " + FString::FromInt(j))),
					&OuterWallContainer);
			}

			// Set Outer Walls +Y 
			if (j == (MazeHeight - 1))
			{
				WallTileTransform.SetRotation(FRotator(0.f, 180.f, 0.f).Quaternion());
				WallTileTransform.SetLocation(FVector(
					(OuterWallSize.X + OuterCornerSize.Y) * i  - (FloorSize.X * (MazeWidth - 1)) / 2,
					(((FloorSize.X) + (FloorSize.X * (MazeHeight - 1))) / 2),
					(OuterWallSize.Z + FloorSize.Z) / 2));

				CreateChildActorInstance(WallTileTransform,
					OuterWallActorClass,
					OuterWallSceneComp,
					FName(TEXT("Outer Wall +Y " + FString::FromInt(i) + ", " + FString::FromInt(j))),
					&OuterWallContainer);
			}
		}
	}
}

void AMazeBase::GenerateCorners()
{
	if (bUseMeshSizes)
	{
		if (!InnerCornerMeshSize || !OuterCornerMeshSize)
		{
			UE_LOG(LogTemp, Error, TEXT("The maze is using the corner mesh size but does not have a corner mesh set"));
			return;
		}
		InnerCornerSize = InnerCornerMeshSize->GetBoundingBox().GetSize();
		OuterCornerSize = OuterCornerMeshSize->GetBoundingBox().GetSize();
	}

	FTransform CornerTileTransform;

	for (int32 i = 0; i < MazeWidth + 1; i++)
	{
		for (int32 j = 0; j < MazeHeight + 1; j++)
		{
			if (i == 0 || j == 0 || i == MazeWidth || j == MazeHeight)
			{
				CornerTileTransform.SetLocation(FVector(
					(((i * FloorSize.X) - (FloorSize.X * (MazeWidth - 1)) / 2) - FloorSize.X / 2),
					(((j * FloorSize.Y) - (FloorSize.Y * (MazeHeight - 1)) / 2) - FloorSize.Y / 2),
					(OuterCornerSize.Z + FloorSize.Z) / 2));

				CreateChildActorInstance(CornerTileTransform,
					OuterCornerActorClass,
					OuterCornerSceneComp,
					FName(TEXT("Outer Corner " + FString::FromInt(i) + ", " + FString::FromInt(j))),
					&OuterCornerContainer);
			}
			else
			{
				CornerTileTransform.SetLocation(FVector(
					((i * FloorSize.X) - (FloorSize.X * (MazeWidth - 1)) / 2) - FloorSize.X / 2,
					((j * FloorSize.Y) - (FloorSize.Y * (MazeHeight - 1)) / 2) - FloorSize.Y / 2,
					(InnerCornerSize.Z + FloorSize.Z) / 2));
				
				CreateChildActorInstance(CornerTileTransform,
					InnerCornerActorClass,
					InnerCornerSceneComp,
					FName(TEXT("Inner Corner " + FString::FromInt(i) + ", " + FString::FromInt(j))),
					&InnerCornerContainer);
			}
		}
	}
}

void AMazeBase::CreateChildActorInstance(FTransform Transform, UClass* Class,
	TObjectPtr<USceneComponent> ParentSceneComponent, FName ComponentName,
	TArray<TObjectPtr<UChildActorComponent>>* Container)
{
	TObjectPtr<UChildActorComponent> TempComp = NewObject<UChildActorComponent>(
				this,
				UChildActorComponent::StaticClass(),
				ComponentName);
			
	if (TempComp)
	{
		TempComp->CreationMethod = EComponentCreationMethod::Instance;
		TempComp->SetupAttachment(ParentSceneComponent);
		TempComp->RegisterComponent();
		TempComp->SetChildActorClass(Class); // TODO: Make this an array and add random option
		TempComp->SetRelativeTransform(Transform);
		Container->Add(TempComp);
	}
}

void AMazeBase::InitializeRandomStreamSeeds()
{
	MazeRandomStream.Initialize(Seed);
	RoomRandomStream.Initialize(Seed);
}

void AMazeBase::GenerateAndSetRandomSeed()
{
	SetMazeSeed(FMath::Rand());
}

void AMazeBase::RegenerateMaze()
{
	if (!bUseCustomSeed && bGenerateRandomSeed)
	{
		GenerateAndSetRandomSeed();
	}

	ClearMaze();
	InitializeRandomStreamSeeds();
	GenerateFloors();
	GenerateCorners();
	GenerateWalls();
	GenerateRooms();
	ImplementMazeAlgorithm();
	CarveEntryAndExit();
}

void AMazeBase::GenerateRooms()
{
	if (!bCreateRooms || NumberOfRooms <= 0)
	{
		return;
	}
	
	if (RoomHeight >= MazeHeight || RoomWidth >= MazeWidth)
	{
		UE_LOG(LogTemp, Error, TEXT("Room height or room width is too large."));
		return;
	}

	int32 XRandMin = 1;
	int32 YRandMin = 1;
	int32 XRandMax = (MazeWidth - 1) - RoomWidth;
	int32 YRandMax = (MazeHeight - 1) - RoomHeight;

	for (int32 i = 0; i < NumberOfRooms; i++)
	{
		bool bRoomsSpawned = false;
		int32 LoopCounter = 0; // keeps track of times around loop, will cancel at 20

		LOOP: while (!bRoomsSpawned)
		{
			LoopCounter++;

			int32 RoomRandY = RoomRandomStream.RandRange(YRandMin, YRandMax);
			int32 RoomRandX = RoomRandomStream.RandRange(XRandMin, XRandMax);
			FIntPoint CellCoord(RoomRandX, RoomRandY);

			if(((NumberOfRooms * RoomWidth * RoomHeight) > ((MazeHeight * MazeWidth) - (2 * (MazeHeight - 1) + 2 * (MazeWidth - 1)))) || (LoopCounter > 20))
			{
				bRoomsSpawned = true;
			}
			
			FMazeCellData* CellFound = MazeData.Find(CellCoord);
			if (CellFound)
			{
				TArray<FVector> RoomCellLocations;
				TArray<FMazeCellData> TempCellData;
				FBox RoomBounds;
				//UE_LOG(LogTemp, Warning, TEXT("Cell Location: %d , %d"), CellFound->CellCoordinates.X, CellFound->CellCoordinates.Y);
				
				
				for (int32 j = CellFound->MazeCellCoordinates.X; j < CellFound->MazeCellCoordinates.X + RoomWidth; j++)
				{
					for (int32 k = CellFound->MazeCellCoordinates.Y; k < CellFound->MazeCellCoordinates.Y + RoomHeight; k++)
					{
						FMazeCellData* TempCell = MazeData.Find(FIntPoint(j,k));
						if (TempCell)
						{
							// if any of the cells found have been visited before, start process over
							if (TempCell->bAlgorithmHasVisited)
							{
								goto LOOP; // start while loop again
							}
							TempCellData.Add(*TempCell);
							RoomCellLocations.Add(TempCell->MazeCellTransform.GetLocation());
						}
					}
				}
				
				int32 ArrayBounds = RoomCellLocations.Num() - 1; // cap the array bounds
				FVector BoxHeight = FVector(0.f, 0.f, 2 * FloorSize.Z); // Vector to add to the room bounds to add height
				RoomBounds = FBox(RoomCellLocations[0] + BoxHeight + GetActorLocation(), RoomCellLocations[ArrayBounds] + BoxHeight + GetActorLocation()); // create box to delete wall and corner instances

				// Mark room cells as visited
				for (auto Cell : TempCellData)
				{
					FMazeCellData* TempCell = MazeData.Find(FIntPoint(Cell.MazeCellCoordinates.X, Cell.MazeCellCoordinates.Y));
					if (TempCell)
					{
						TempCell->bAlgorithmHasVisited = true;
					}
				}

				// delete components to make rooms
				TArray<TObjectPtr<USceneComponent>> ComponentsToDelete;
				
				for (auto Component : InnerWallContainer)
				{
					if (!Component || !Component->GetChildActor())
					{
						continue;
					}
					
					if (RoomBounds.Intersect(Component->GetChildActor()->GetComponentsBoundingBox()))
					{
						ComponentsToDelete.Add(Component);
					}
				}

				for (auto Component : InnerCornerContainer)
				{
					if (!Component || !Component->GetChildActor())
					{
						continue;
					}
					
					if (RoomBounds.Intersect(Component->GetChildActor()->GetComponentsBoundingBox()))
					{
						ComponentsToDelete.Add(Component);
					}
				}
				
				// carve room doors
				for (int32 RoomDoorCounter = 0; RoomDoorCounter < NumberOfRoomDoors; RoomDoorCounter++)
				{
					int32 RandPosX = RoomRandomStream.RandRange(0,RoomWidth - 1);
					int32 RandPosY = RoomRandomStream.RandRange(0,RoomHeight - 1);
					int32 RandSide = RoomRandomStream.RandRange(0,3);
					FBox DoorBounds;
					for (int32 j = TempCellData[0].MazeCellCoordinates.X; j <= TempCellData[TempCellData.Num() - 1].MazeCellCoordinates.X; j++)
					{
						for (int32 k = TempCellData[0].MazeCellCoordinates.Y; k <= TempCellData[TempCellData.Num() - 1].MazeCellCoordinates.Y; k++)
						{
							if (RandSide == 0)
							{
								if (j == TempCellData[0].MazeCellCoordinates.X)
								{
									if (k == TempCellData[0].MazeCellCoordinates.Y + RandPosY)
									{
										DoorBounds = FBox(MazeData.Find(FIntPoint(j - 1,k))->MazeCellTransform.GetLocation() + BoxHeight + GetActorLocation(),
											MazeData.Find(FIntPoint(j,k))->MazeCellTransform.GetLocation() + BoxHeight + GetActorLocation());
									}
								}
							}
							else if (RandSide == 1)
							{
								if (j == TempCellData[TempCellData.Num() - 1].MazeCellCoordinates.X)
								{
									if (k == TempCellData[0].MazeCellCoordinates.Y + RandPosY)
									{
										DoorBounds = FBox(MazeData.Find(FIntPoint(j,k))->MazeCellTransform.GetLocation() + BoxHeight + GetActorLocation(),
											MazeData.Find(FIntPoint(j + 1,k))->MazeCellTransform.GetLocation() + BoxHeight + GetActorLocation());
									}
								}
							}
							else if (RandSide == 2)
							{
								if (k == TempCellData[0].MazeCellCoordinates.Y)
								{
									if (j == TempCellData[0].MazeCellCoordinates.X + RandPosX)
									{
										DoorBounds = FBox(MazeData.Find(FIntPoint(j,k - 1))->MazeCellTransform.GetLocation() + BoxHeight + GetActorLocation(),
											MazeData.Find(FIntPoint(j,k))->MazeCellTransform.GetLocation() + BoxHeight + GetActorLocation());
									}
								}
							}
							else if (RandSide == 3)
							{
								if (k == TempCellData[TempCellData.Num() - 1].MazeCellCoordinates.Y)
								{
									if (j == TempCellData[0].MazeCellCoordinates.X + RandPosX)
									{
										DoorBounds = FBox(MazeData.Find(FIntPoint(j,k))->MazeCellTransform.GetLocation() + BoxHeight + GetActorLocation(),
											MazeData.Find(FIntPoint(j,k + 1))->MazeCellTransform.GetLocation() + BoxHeight + GetActorLocation());
									}
								}
							}
						}	
					}
			
					
					FTransform DoorwayTransform;

					for (auto Component : InnerWallContainer)
					{
						if (!Component || !Component->GetChildActor())
						{
							continue;
						}
					
						if (DoorBounds.Intersect(Component->GetChildActor()->GetComponentsBoundingBox()))
						{
							ComponentsToDelete.Add(Component);
							DoorwayTransform = Component->GetComponentTransform();
						}
					}

					RemovedRoomDoorwayTransforms.Add(DoorwayTransform);
				}

				// delete all room and door components
				if (!ComponentsToDelete.IsEmpty())
				{
					for (int32 k = ComponentsToDelete.Num(); k > 0; k--)
					{
						if (!ComponentsToDelete[k - 1])
						{
							continue;
						}
						//ComponentsToDelete[k - 1]->UnregisterComponent();
						ComponentsToDelete[k - 1]->DestroyComponent();
					}
				}
				
				RoomCenters.Add(RoomBounds.GetCenter());
				bRoomsSpawned = true;
			}
		}
	}
}

void AMazeBase::ImplementMazeAlgorithm()
{
	FIntPoint StartingCell; // cell used to start the algorithm
	if (MazeData.Find(MazeAlgorithmStartingCell))
	{
		if (MazeAlgorithmStartingCell.X > MazeWidth || MazeAlgorithmStartingCell.Y > MazeHeight || MazeData.Find(MazeAlgorithmStartingCell)->bAlgorithmHasVisited)
		{
			UE_LOG(LogTemp, Warning, TEXT("Maze Algorithm Starting Point is out of bounds, changing to (0, 0)"));
			StartingCell = FIntPoint(0,0);
		}
		else
		{
			StartingCell = MazeAlgorithmStartingCell;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Maze Algorithm Starting Point is out of bounds, changing to (0, 0)"));
		StartingCell = FIntPoint(0,0);
	}

	DeadEnds.Empty();
	TArray<FMazeCellData> CellQueue;
	int32 CurX = StartingCell.X;
	int32 CurY = StartingCell.Y;
	
	FMazeCellData* CurrentCell = MazeData.Find(FIntPoint(CurX, CurY));
	bool bDoneIterating = false;
	TArray<FMazeCellData> UnVisitedNearbyCells;

	bool bCheckPrevious = false;
	bool bEndCounter = false; // used to find all of the dead ends
	
	while (!bDoneIterating)
	{
		if (CurrentCell)
		{
			if (!bCheckPrevious)
			{
				CellQueue.Add(*CurrentCell);
			}
			
			CurrentCell->bAlgorithmHasVisited = true;
			ECellPosition CurrentCellPosition = CurrentCell->CellPosition;
			int32 RandDir;
			
			FMazeCellData* CheckCell = nullptr;
			switch (CurrentCellPosition)
			{
			case ECellPosition::ECP_Normal :
					
				CheckCell = MazeData.Find(FIntPoint(CurX + 1, CurY)); // cell up
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}
				
				CheckCell = MazeData.Find(FIntPoint(CurX, CurY + 1)); // cell right
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}

				CheckCell = MazeData.Find(FIntPoint(CurX - 1, CurY)); // cell down
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}

				CheckCell = MazeData.Find(FIntPoint(CurX, CurY - 1)); // cell left
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}
				break;
				
			case ECellPosition::ECP_LeftSide :
					
				CheckCell = MazeData.Find(FIntPoint(CurX + 1, CurY)); // cell up
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}
					
				CheckCell = MazeData.Find(FIntPoint(CurX, CurY + 1)); // cell right
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}

				CheckCell = MazeData.Find(FIntPoint(CurX - 1, CurY)); // cell down
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}
				break;
				
			case ECellPosition::ECP_RightSide :

				CheckCell = MazeData.Find(FIntPoint(CurX + 1, CurY)); // cell up
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}

				CheckCell = MazeData.Find(FIntPoint(CurX - 1, CurY)); // cell down
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}

				CheckCell = MazeData.Find(FIntPoint(CurX, CurY - 1)); // cell left
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}
				break;
				
			case ECellPosition::ECP_TopSide :
					
				CheckCell = MazeData.Find(FIntPoint(CurX, CurY + 1)); // cell right
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}

				CheckCell = MazeData.Find(FIntPoint(CurX - 1, CurY)); // cell down
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}

				CheckCell = MazeData.Find(FIntPoint(CurX, CurY - 1)); // cell left
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}
				break;
				
			case ECellPosition::ECP_BottomSide :

				CheckCell = MazeData.Find(FIntPoint(CurX + 1, CurY)); // cell up
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}
					
				CheckCell = MazeData.Find(FIntPoint(CurX, CurY + 1)); // cell right
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}
				
				CheckCell = MazeData.Find(FIntPoint(CurX, CurY - 1)); // cell left
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}
				break;
				
			case ECellPosition::ECP_BottomRightCorner :

				CheckCell = MazeData.Find(FIntPoint(CurX + 1, CurY)); // cell up
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}

				CheckCell = MazeData.Find(FIntPoint(CurX, CurY - 1)); // cell left
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}
				break;
				
			case ECellPosition::ECP_BottomLeftCorner :

				CheckCell = MazeData.Find(FIntPoint(CurX + 1, CurY)); // cell up
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}
				
				CheckCell = MazeData.Find(FIntPoint(CurX, CurY + 1)); // cell right
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}
				break;
				
			case ECellPosition::ECP_TopRightCorner :

				CheckCell = MazeData.Find(FIntPoint(CurX - 1, CurY)); // cell down
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}

				CheckCell = MazeData.Find(FIntPoint(CurX, CurY - 1)); // cell left
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}
				break;
				
			case ECellPosition::ECP_TopLeftCorner :

				CheckCell = MazeData.Find(FIntPoint(CurX - 1, CurY)); // cell down
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}

				CheckCell = MazeData.Find(FIntPoint(CurX, CurY + 1)); // cell right
				if (CheckCell)
				{
					if (!CheckCell->bAlgorithmHasVisited)
					{
						UnVisitedNearbyCells.Add(*CheckCell);
					}
				}
				break;
				
			default:
				break;
			}

			int32 ArrayLength = UnVisitedNearbyCells.Num();
			//UE_LOG(LogTemp, Warning, TEXT("%d"), ArrayLength);
			if (ArrayLength > 0)
			{
				bEndCounter = false;
				RandDir = MazeRandomStream.RandRange(0 , ArrayLength - 1);
				FMazeCellData CellToUse = UnVisitedNearbyCells[RandDir];
				FMazeCellData* NextCell = MazeData.Find(FIntPoint(CellToUse.MazeCellCoordinates.X, CellToUse.MazeCellCoordinates.Y));
				if (NextCell)
				{
					FBox Box;
					if (NextCell->MazeCellCoordinates.X < CurrentCell->MazeCellCoordinates.X || NextCell->MazeCellCoordinates.Y < CurrentCell->MazeCellCoordinates.Y)
					{
						Box = FBox(NextCell->MazeCellTransform.GetLocation()  + FVector(0.f, 0.f, 2*FloorSize.Z) + GetActorLocation(),
						           CurrentCell->MazeCellTransform.GetLocation() + FVector(0.f, 0.f, 2*FloorSize.Z) + GetActorLocation());
					}
					else
					{
						Box = FBox(CurrentCell->MazeCellTransform.GetLocation() + FVector(0.f, 0.f, 2*FloorSize.Z) + GetActorLocation(),
						           NextCell->MazeCellTransform.GetLocation() + FVector(0.f, 0.f, 2*FloorSize.Z) + GetActorLocation());
					}

					TArray<TObjectPtr<USceneComponent>> ComponentsToDelete;
					
					for (auto Component : InnerWallContainer)
					{
						if (!Component || !Component->GetChildActor())
						{
							continue;
						}
					
						if (Box.Intersect(Component->GetChildActor()->GetComponentsBoundingBox()))
						{
							ComponentsToDelete.Add(Component);
						}
					}

					if (!ComponentsToDelete.IsEmpty())
					{
						for (int32 k = ComponentsToDelete.Num(); k > 0; k--)
						{
							if (!ComponentsToDelete[k - 1])
							{
								continue;
							}
							//ComponentsToDelete[k - 1]->UnregisterComponent();
							ComponentsToDelete[k - 1]->DestroyComponent();
						}
					}
					
					CurrentCell = NextCell;
					CurX = CurrentCell->MazeCellCoordinates.X;
					CurY = CurrentCell->MazeCellCoordinates.Y;
					bCheckPrevious = false;
				}
			}
			else
			{
				if (!bEndCounter)
				{
					DeadEnds.Add(CellQueue[CellQueue.Num() - 1].MazeCellTransform.GetLocation());
					bEndCounter = true;
				}

				CellQueue.RemoveAt(CellQueue.Num() - 1); // remove last cell from the queue
				if (CellQueue.Num() > 0)
				{
					FMazeCellData CellToUse = CellQueue[CellQueue.Num() - 1];
					FMazeCellData* NextCell = MazeData.Find(FIntPoint(CellToUse.MazeCellCoordinates.X, CellToUse.MazeCellCoordinates.Y));
					if (NextCell)
					{
						bCheckPrevious = true;
						CurrentCell = NextCell;
						CurX = CurrentCell->MazeCellCoordinates.X;
						CurY = CurrentCell->MazeCellCoordinates.Y;
					}
				}
				else
				{
					bDoneIterating = true;
				}
				
			}
			UnVisitedNearbyCells.Empty();
		}
		else
		{
			bDoneIterating = true;
		}
		
	}
}

void AMazeBase::CarveEntryAndExit()
{

	TArray<TObjectPtr<USceneComponent>> ComponentsToDelete;
	
	if (bHasEntry)
	{
		if (bEntrySide1)
		{
			bEntrySide2 = false;
			bEntrySide3 = false;
			bEntrySide4 = false;
		}
		else if (bEntrySide2)
		{
			bEntrySide1 = false;
			bEntrySide3 = false;
			bEntrySide4 = false;
		}
		else if (bEntrySide3)
		{
			bEntrySide1 = false;
			bEntrySide2 = false;
			bEntrySide4 = false;
		}
		else if (bEntrySide4)
		{
			bEntrySide1 = false;
			bEntrySide2 = false;
			bEntrySide3 = false;
		}
		
		if (!bCustomEntry)
		{
			if (bRandomEntry)
			{
				if (bEntrySide1 || bEntrySide3)
				{
					EntryWallNumber = MazeRandomStream.RandRange(0, MazeWidth - 1);
				}
				else if (bEntrySide2 || bEntrySide4)
				{
					EntryWallNumber = MazeRandomStream.RandRange(0, MazeHeight - 1);
				}
				else
				{
					EntryWallNumber = 0;
				}
			}
		}

		if ((bEntrySide1 || bEntrySide3) && EntryWallNumber > MazeWidth - 1)
		{
			UE_LOG(LogTemp, Error, TEXT("EntryWallNumber is greater than MazeWidth"));
			return;
		}

		if ((bEntrySide2 || bEntrySide4) && EntryWallNumber > MazeHeight - 1)
		{
			UE_LOG(LogTemp, Error, TEXT("EntryWallNumber is greater than MazeHeight"));
			return;
		}
		
		FBox EntryBox;
		FVector BoxAddition = FVector(0.f, 0.f, 2*FloorSize.Z);
		if (bEntrySide1)
		{
			FMazeCellData* Cell = MazeData.Find(FIntPoint(EntryWallNumber, 0));
			if (Cell)
			{
				FVector CellLocation = Cell->MazeCellTransform.GetLocation() + GetActorLocation();
				EntryBox = FBox(CellLocation + FVector( 0.f, -(FloorSize.Y / 2 + OuterWallSize.Y), 0.f) + BoxAddition,
				CellLocation + BoxAddition);
			}
		}
		else if (bEntrySide2)
		{
			FMazeCellData* Cell = MazeData.Find(FIntPoint(0, EntryWallNumber));
			if (Cell)
			{
				FVector CellLocation = Cell->MazeCellTransform.GetLocation() + GetActorLocation();
				EntryBox = FBox(CellLocation + FVector( -(FloorSize.X / 2 + OuterWallSize.Y), 0.f, 0.f) + BoxAddition,
				CellLocation + BoxAddition);
			}
		}
		else if (bEntrySide3)
		{
			FMazeCellData* Cell = MazeData.Find(FIntPoint(EntryWallNumber, MazeHeight - 1));
			if (Cell)
			{
				FVector CellLocation = Cell->MazeCellTransform.GetLocation() + GetActorLocation();
				EntryBox = FBox(CellLocation + BoxAddition,
				CellLocation + FVector( 0.f, (FloorSize.Y / 2 + OuterWallSize.Y), 0.f) + BoxAddition);
			}
		}
		else if (bEntrySide4)
		{
			FMazeCellData* Cell = MazeData.Find(FIntPoint(MazeWidth - 1, EntryWallNumber));
			if (Cell)
			{
				FVector CellLocation = Cell->MazeCellTransform.GetLocation() + GetActorLocation();
				EntryBox = FBox(CellLocation + BoxAddition,
				CellLocation + FVector( (FloorSize.X / 2 + OuterWallSize.Y), 0.f, 0.f) + BoxAddition);
			}
		}

		FTransform EntryTransform;
		
		for (auto Component : OuterWallContainer)
		{
			if (!Component || !Component->GetChildActor())
			{
				continue;
			}
					
			if (EntryBox.Intersect(Component->GetChildActor()->GetComponentsBoundingBox()))
			{
				ComponentsToDelete.Add(Component);
				EntryTransform = Component->GetComponentTransform();
			}
		}
		
		EntryWallTransform = EntryTransform;
		
	}
	
	if (bHasExit)
	{

		if (bExitSide1)
		{
			bExitSide2 = false;
			bExitSide3 = false;
			bExitSide4 = false;
		}
		else if (bExitSide2)
		{
			bExitSide1 = false;
			bExitSide3 = false;
			bExitSide4 = false;
		}
		else if (bExitSide3)
		{
			bExitSide1 = false;
			bExitSide2 = false;
			bExitSide4 = false;
		}
		else if (bEntrySide4)
		{
			bExitSide1 = false;
			bExitSide2 = false;
			bExitSide3 = false;
		}
		
		if (!bCustomExit)
		{
			if (bRandomExit)
			{
				if (bExitSide1 || bExitSide3)
				{
					ExitWallNumber = MazeRandomStream.RandRange(0, MazeWidth - 1);
				}
				else if (bExitSide2 || bExitSide4)
				{
					ExitWallNumber = MazeRandomStream.RandRange(0, MazeHeight - 1);
				}
				else
				{
					ExitWallNumber = 0;
				}
			}
		}

		if ((bExitSide1 || bExitSide3) && ExitWallNumber > MazeWidth - 1)
		{
			UE_LOG(LogTemp, Error, TEXT("ExitWallNumber is greater than MazeWidth"));
			return;
		}

		if ((bExitSide2 || bExitSide4) && ExitWallNumber > MazeHeight - 1)
		{
			UE_LOG(LogTemp, Error, TEXT("ExitWallNumber is greater than MazeHeight"));
			return;
		}
		
		FBox ExitBox;
		FVector BoxAddition = FVector(0.f, 0.f, 2*FloorSize.Z);
		if (bExitSide1)
		{
			FMazeCellData* Cell = MazeData.Find(FIntPoint(ExitWallNumber, 0));
			if (Cell)
			{
				FVector CellLocation = Cell->MazeCellTransform.GetLocation() + GetActorLocation();
				ExitBox = FBox(CellLocation + FVector( 0.f, -(FloorSize.Y / 2 + OuterWallSize.Y), 0.f) + BoxAddition,
				CellLocation + BoxAddition);
			}
		}
		else if (bExitSide2)
		{
			FMazeCellData* Cell = MazeData.Find(FIntPoint(0, ExitWallNumber));
			if (Cell)
			{
				FVector CellLocation = Cell->MazeCellTransform.GetLocation() + GetActorLocation();
				ExitBox = FBox(CellLocation + FVector( -(FloorSize.X / 2 + OuterWallSize.Y), 0.f, 0.f) + BoxAddition,
				CellLocation + BoxAddition);
			}
		}
		else if (bExitSide3)
		{
			FMazeCellData* Cell = MazeData.Find(FIntPoint(ExitWallNumber, MazeHeight - 1));
			if (Cell)
			{
				FVector CellLocation = Cell->MazeCellTransform.GetLocation() + GetActorLocation();
				ExitBox = FBox(CellLocation + BoxAddition,
				CellLocation + FVector( 0.f, (FloorSize.Y / 2 + OuterWallSize.Y), 0.f) + BoxAddition);
			}
		}
		else if (bExitSide4)
		{
			FMazeCellData* Cell = MazeData.Find(FIntPoint(MazeWidth - 1, ExitWallNumber));
			if (Cell)
			{
				FVector CellLocation = Cell->MazeCellTransform.GetLocation() + GetActorLocation();
				ExitBox = FBox(CellLocation + BoxAddition,
				CellLocation + FVector( (FloorSize.X / 2 + OuterWallSize.Y), 0.f, 0.f) + BoxAddition);
			}
		}

		FTransform ExitTransform;
		
		for (auto Component : OuterWallContainer)
		{
			if (!Component || !Component->GetChildActor())
			{
				continue;
			}
					
			if (ExitBox.Intersect(Component->GetChildActor()->GetComponentsBoundingBox()))
			{
				ComponentsToDelete.Add(Component);
				ExitTransform = Component->GetComponentTransform();
			}
		}
		
		ExitWallTransform = ExitTransform;
	}

	if (!ComponentsToDelete.IsEmpty())
	{
		for (int32 k = ComponentsToDelete.Num(); k > 0; k--)
		{
			if (!ComponentsToDelete[k - 1])
			{
				continue;
			}
			//ComponentsToDelete[k - 1]->UnregisterComponent();
			ComponentsToDelete[k - 1]->DestroyComponent();
		}
	}
}


// Called every frame
void AMazeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

