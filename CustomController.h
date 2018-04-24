// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "AIController.h"
#include "CNode.h"
#include "CMath.h"
#include "Engine/World.h"
#include "CPathFollowComponent.h"
#include "Ball.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "CustomController.generated.h"

UENUM(BlueprintType)
enum class EStatusEnum : uint8
{
	SE_IDLE     UMETA(DisplayName = "Idle"),
	SE_CHARGE   UMETA(DisplayName = "Charging"),
	SE_DOWNED   UMETA(DisplayName = "Downed"),
	SE_DEAD     UMETA(DisplayName = "Dead"),
	SE_CHASE    UMETA(DisplayName = "Chasing"),
	SE_ENGAGED  UMETA(DisplayName = "Engaged"),
	SE_REVIVE   UMETA(DisplayName = "Reviving"),
	SE_FOUNDFOE UMETA(DisplayName = "FoundFoe"),
	SE_ATTACK   UMETA(DisplayName = "Attacking"),
	SE_STALK    UMETA(DisplayName = "Stalking")
};

static const int VERTS_PER_POLYGON = 6;
static const unsigned int NULL_LINK = 0xffffffff;

UCLASS()
class ACustomController : public AAIController
{
	GENERATED_BODY()
public:
	ACustomController(const FObjectInitializer& PCIP); 
	//virtual void FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const override;
	virtual void FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const override;
	FPathFindingResult FindPath(const FPathFindingQuery& Query) const;
	UAIPerceptionComponent* perceptionComponent = GetAIPerceptionComponent(); 

	void BeginPlay() override;
	void Possess(APawn* InPawn) override;
	void OnPossess(APawn* In);
	class CDecisionClass* decisions;
	UAIPerceptionComponent* perception;
	UAISenseConfig_Sight* sight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
	EStatusEnum status = EStatusEnum::SE_IDLE; 
	UFUNCTION()
	void PerceptionUpdated(const TArray<AActor*>& Actors);	
	virtual void Tick(float DeltaTime) override;
	//const FNavAgentProperties& AgentProperties,
private:
	bool PathFindingAlgorithm(FVector startLoc, FVector endLoc, FNavigationPath& result) const;
	void spawnBall(FVector loc, FColor colour) const; 
	bool GetPolyNeighbors(NavNodeRef PolyID, TArray<CNode*>& Neighbors, CNodePool* m_nodePool) const;
	TArray<AActor*> GetActorsWithName(FString name, FString name2) const;
	bool GetAllPolys(TArray<NavNodeRef>& Polys) const;
	void DeleteAllBalls() const; 
	EStatusEnum IntToEnum(int num); 
private: 



	FORCEINLINE const ANavigationData* CGetNavData() const
	{
		const FNavAgentProperties& AgentProperties = GetNavAgentPropertiesRef(); 
		const ANavigationData* NavData = GetWorld()->GetNavigationSystem()->GetNavDataForProps(AgentProperties);
		if (NavData == NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("Error, Using main navData"));
			NavData = GetWorld()->GetNavigationSystem()->GetMainNavData();
		}

		return NavData;
	}

	FORCEINLINE bool TileIsValid(const ARecastNavMesh* NavMesh, int32 TileIndex) const
	{
		if (!NavMesh) return false;
		//~~~~~~~~~~~~~~
		const FBox TileBounds = NavMesh->GetNavMeshTileBounds(TileIndex);

		return TileBounds.IsValid != 0;
	}
public:
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations)
	//	bool* animAttack;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations)
	//	bool* animDie;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations)
	//	bool* animWounded;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations)
	//	bool* animFoundFriend;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations)
	//	bool* animFoundFoe;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations)
	//	float animFoePitchDir = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations)
		AActor* attention = nullptr;
};

struct QueryData
{
	struct dtNode* lastBestNode;
	float lastBestNodeCost;
	FVector startRef, endRef;
	float startPos[3], endPos[3];
};

struct CPoly
{
	/// Index to first link in linked list. (Or #NULL_LINK if there is no link.)
	unsigned int firstLink;

	/// The indices of the polygon's vertices.
	/// The actual vertices are located in dtMeshTile::verts.
	unsigned short verts[VERTS_PER_POLYGON];

	/// Packed data representing neighbor polygons references and flags for each edge.
	unsigned short neis[VERTS_PER_POLYGON];

	/// The user defined polygon flags.
	unsigned short flags;

	/// The number of vertices in the polygon.
	unsigned char vertCount;

	/// The bit packed area id and polygon type.
	/// @note Use the structure's set and get methods to acess this value.
	unsigned char areaAndtype;

	/// Sets the user defined area id. [Limit: < #MAX_AREAS]
	inline void setArea(unsigned char a) { areaAndtype = (areaAndtype & 0xc0) | (a & 0x3f); }

	/// Sets the polygon type. (See: #PolyTypes.)
	inline void setType(unsigned char t) { areaAndtype = (areaAndtype & 0x3f) | (t << 6); }

	/// Gets the user defined area id.
	inline unsigned char getArea() const { return areaAndtype & 0x3f; }

	/// Gets the polygon type. (See: #PolyTypes)
	inline unsigned char getType() const { return areaAndtype >> 6; }
};

