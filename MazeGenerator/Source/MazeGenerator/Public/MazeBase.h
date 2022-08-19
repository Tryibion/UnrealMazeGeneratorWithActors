// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMazeConstructionCompleted);

UENUM()
enum class ECellPosition : uint8
{
	ECP_Normal,
	ECP_LeftSide,
	ECP_RightSide,
	ECP_TopSide,
	ECP_BottomSide,
	ECP_BottomRightCorner,
	ECP_BottomLeftCorner,
	ECP_TopRightCorner,
	ECP_TopLeftCorner,

	ECP_MAX
};

USTRUCT(BlueprintType)
struct FMazeCellData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FTransform MazeCellTransform;

	UPROPERTY(BlueprintReadOnly)
	FIntPoint MazeCellCoordinates;

	UPROPERTY(BlueprintReadOnly)
	ECellPosition CellPosition;

	bool bAlgorithmHasVisited;
	
};

// Forward declaring
class UStaticMeshComponent;
class UStaticMesh;
class USceneComponent;

UCLASS()
class MAZEGENERATOR_API AMazeBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMazeBase();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Maze Components")
	TObjectPtr<USceneComponent> CenterSceneComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Maze Components")
	TObjectPtr<USceneComponent> FloorSceneComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Maze Components")
	TObjectPtr<USceneComponent> InnerWallSceneComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Maze Components")
	TObjectPtr<USceneComponent> OuterWallSceneComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Maze Components")
	TObjectPtr<USceneComponent> InnerCornerSceneComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Maze Components")
	TObjectPtr<USceneComponent> OuterCornerSceneComp;
	
	UPROPERTY(BlueprintReadOnly, Category="Maze Components")
	TArray<TObjectPtr<UChildActorComponent>> FloorContainer;

	UPROPERTY(BlueprintReadOnly, Category="Maze Components")
	TArray<TObjectPtr<UChildActorComponent>> InnerWallContainer;

	UPROPERTY(BlueprintReadOnly, Category="Maze Components")
	TArray<TObjectPtr<UChildActorComponent>> OuterWallContainer;

	UPROPERTY(BlueprintReadOnly, Category="Maze Components")
	TArray<TObjectPtr<UChildActorComponent>> InnerCornerContainer;

	UPROPERTY(BlueprintReadOnly, Category="Maze Components")
	TArray<TObjectPtr<UChildActorComponent>> OuterCornerContainer;

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void ClearMaze();

	UFUNCTION()
	void GenerateFloors();

	UFUNCTION()
	void GenerateWalls();

	UFUNCTION()
	void GenerateCorners();

	void CreateChildActorInstance(FTransform Transform,
		UClass* Class,
		TObjectPtr<USceneComponent> ParentSceneComponent,
		FName ComponentName,
		TArray<TObjectPtr<UChildActorComponent>>* Container);

	UFUNCTION()
	void InitializeRandomStreamSeeds();

	UFUNCTION()
	void GenerateAndSetRandomSeed();

	UFUNCTION()
	void RegenerateMaze();

	UFUNCTION()
	void GenerateRooms();

	UFUNCTION()
	void ImplementMazeAlgorithm();

	UFUNCTION()
	void CarveEntryAndExit();
	
public:

	UPROPERTY(BlueprintAssignable, Category="Maze")
	FOnMazeConstructionCompleted OnMazeConstructionCompleted;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties",
		meta = (ExposeOnSpawn="true", ToolTip="This sets how many cells the maze will have width wise."))
	int32 MazeWidth;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties",
		meta = (ExposeOnSpawn="true", ToolTip="This sets how many cells the maze will have height wise."))
	int32 MazeHeight;

	/// <summary>
	/// The maze can be generated in the construction script if this is true.
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category="Maze|Properties")
	bool bGenerateInConstructionScript;

	/// <summary>
	/// When clicked, this will regenerate the maze in the editor.
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category="Maze|Properties",
		meta = (EditCondition="bGenerateInConstructionScript", ToolTip="Click to regenerate the maze in the construction script"))
	bool bRegenerateMazeInConstructionScript;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Maze|Properties|Seed",
		meta = (ExposeOnSpawn="true"))
	bool bUseCustomSeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Maze|Properties|Seed",
		meta = (EditCondition="!bUseCustomSeed", ToolTip="Click to generate a random seed", ExposeOnSpawn="true"))
	bool bGenerateRandomSeed;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Maze|Properties|Seed",
		meta = (EditCondition="bUseCustomSeed", ExposeOnSpawn="true"))
	int32 Seed;

	UFUNCTION(BlueprintCallable, Category="Maze|Seed")
	FORCEINLINE int32 GetMazeSeed() { return Seed; }

	UFUNCTION(BlueprintCallable, Category="Maze|Seed")
	FORCEINLINE void SetMazeSeed(int32 DesiredSeed) { Seed = DesiredSeed; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Seed",
		meta = (ExposeOnSpawn="true"))
	FIntPoint MazeAlgorithmStartingCell;


	// make these arrays in the future
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Actors",
			meta = (ExposeOnSpawn="true"))
	TSubclassOf<AActor> FloorActorClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Actors",
			meta = (ExposeOnSpawn="true"))
	TSubclassOf<AActor> InnerWallActorClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Actors",
			meta = (ExposeOnSpawn="true"))
	TSubclassOf<AActor> OuterWallActorClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Actors",
			meta = (ExposeOnSpawn="true"))
	TSubclassOf<AActor> InnerCornerActorClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Actors",
			meta = (ExposeOnSpawn="true"))
	TSubclassOf<AActor> OuterCornerActorClass;

	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Rooms",
		meta = (ExposeOnSpawn="true"))
	bool bCreateRooms;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Rooms",
		meta = (EditCondition="bCreateRooms", ExposeOnSpawn="true"))
	int32 NumberOfRooms;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Rooms",
		meta = (EditCondition="bCreateRooms", ExposeOnSpawn="true"))
	int32 RoomWidth;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Rooms",
		meta = (EditCondition="bCreateRooms", ExposeOnSpawn="true"))
	int32 RoomHeight;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Rooms",
		meta = (EditCondition="bCreateRooms", ExposeOnSpawn="true"))
	int32 NumberOfRoomDoors;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|EntryPoint",
		meta = (ExposeOnSpawn="true"))
	bool bHasEntry;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|EntryPoint",
		meta = (EditCondition="bHasEntry",ExposeOnSpawn="true"))
	bool bCustomEntry;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|EntryPoint",
		meta = (EditCondition="bHasEntry && !bCustomEntry", ExposeOnSpawn="true"))
	bool bRandomEntry;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|EntryPoint",
		meta = (EditCondition="bHasEntry && bCustomEntry", ExposeOnSpawn="true"))
	int32 EntryWallNumber;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|EntryPoint",
		meta = (EditCondition="bHasEntry && !bEntrySide2 && !bEntrySide3 && !bEntrySide4", ExposeOnSpawn="true"))
	bool bEntrySide1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|EntryPoint",
		meta = (EditCondition="bHasEntry && !bEntrySide1 && !bEntrySide3 && !bEntrySide4", ExposeOnSpawn="true"))
	bool bEntrySide2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|EntryPoint",
		meta = (EditCondition="bHasEntry && !bEntrySide1 && !bEntrySide2 && !bEntrySide4", ExposeOnSpawn="true"))
	bool bEntrySide3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|EntryPoint",
		meta = (EditCondition="bHasEntry && !bEntrySide1 && !bEntrySide2 && !bEntrySide3", ExposeOnSpawn="true"))
	bool bEntrySide4;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|ExitPoint",
		meta = (ExposeOnSpawn="true"))
	bool bHasExit;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|ExitPoint",
		meta = (EditCondition="bHasExit",ExposeOnSpawn="true"))
	bool bCustomExit;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|ExitPoint",
		meta = (EditCondition="bHasExit && !bCustomExit", ExposeOnSpawn="true"))
	bool bRandomExit;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|ExitPoint",
		meta = (EditCondition="bHasExit && bCustomExit", ExposeOnSpawn="true"))
	int32 ExitWallNumber;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|ExitPoint",
		meta = (EditCondition="bHasExit && !bExitSide2 && !bExitSide3 && !bExitSide4", ExposeOnSpawn="true"))
	bool bExitSide1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|ExitPoint",
		meta = (EditCondition="bHasExit && !bExitSide1 && !bExitSide3 && !bExitSide4", ExposeOnSpawn="true"))
	bool bExitSide2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|ExitPoint",
		meta = (EditCondition="bHasExit && !bExitSide1 && !bExitSide2 && !bExitSide4", ExposeOnSpawn="true"))
	bool bExitSide3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|ExitPoint",
		meta = (EditCondition="bHasExit && !bExitSide1 && !bExitSide2 && !bExitSide3", ExposeOnSpawn="true"))
	bool bExitSide4;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Sizes")
	bool bUseMeshSizes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Sizes|Mesh",
		meta = (EditCondition="bUseMeshSizes", ToolTip="Set to the mesh that has the size of the floor", ExposeOnSpawn="true"))
	TObjectPtr<UStaticMesh> FloorMeshSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Sizes|Mesh",
		meta = (EditCondition="bUseMeshSizes", ToolTip="Set to the mesh that has the size of the inner wall", ExposeOnSpawn="true"))
	TObjectPtr<UStaticMesh> InnerWallMeshSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Sizes|Mesh",
		meta = (EditCondition="bUseMeshSizes", ToolTip="Set to the mesh that has the size of the outer wall", ExposeOnSpawn="true"))
	TObjectPtr<UStaticMesh> OuterWallMeshSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Sizes|Mesh",
		meta = (EditCondition="bUseMeshSizes", ToolTip="Set to the mesh that has the size of the inner corner", ExposeOnSpawn="true"))
	TObjectPtr<UStaticMesh> InnerCornerMeshSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Sizes|Mesh",
		meta = (EditCondition="bUseMeshSizes", ToolTip="Set to the mesh that has the size of the outer corner", ExposeOnSpawn="true"))
	TObjectPtr<UStaticMesh> OuterCornerMeshSize;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Sizes|Custom",
		meta = (EditCondition="!bUseMeshSizes", ExposeOnSpawn="true"))
	FVector FloorSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Sizes|Custom",
		meta = (EditCondition="!bUseMeshSizes", ExposeOnSpawn="true"))
	FVector InnerWallSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Sizes|Custom",
		meta = (EditCondition="!bUseMeshSizes", ExposeOnSpawn="true"))
	FVector OuterWallSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Sizes|Custom",
		meta = (EditCondition="!bUseMeshSizes", ExposeOnSpawn="true"))
	FVector InnerCornerSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Maze|Properties|Sizes|Custom",
		meta = (EditCondition="!bUseMeshSizes", ExposeOnSpawn="true"))
	FVector OuterCornerSize;
	

	UPROPERTY()
	TMap<FIntPoint, FMazeCellData> MazeData;

	UPROPERTY()
	FRandomStream MazeRandomStream;

	UPROPERTY()
	FRandomStream RoomRandomStream;

	UPROPERTY()
	TArray<FVector> RoomCenters;
	
	UPROPERTY()
	TArray<FVector> DeadEnds;

	UPROPERTY()
	TArray<FTransform> RemovedRoomDoorwayTransforms;

	UPROPERTY()
	FTransform EntryWallTransform;

	UPROPERTY()
	FTransform ExitWallTransform;

	UPROPERTY()
	int32 ConstructorCounter;

	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
