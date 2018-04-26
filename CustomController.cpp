// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomController.h"
#include "CPathFollowComponent.h"
#include "Ball.h"
#include "EngineUtils.h"
#include "CUNavAreaJump.h"
#include "CDecisionClass.h"
#include "Runtime/Engine/Classes/Engine/StaticMeshActor.h"
#include "Runtime/Engine/Classes/GameFramework/Character.h"
#include "Runtime/Engine/Classes/AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/AI/Navigation/RecastNavMesh.h"

 ACustomController::ACustomController(const FObjectInitializer& PCIP) : Super(PCIP.SetDefaultSubobjectClass<UCPathFollowComponent>(TEXT("PathFollowingComponent")))
 {
	 perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception")); 
	 sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("sight"));
	 SetPerceptionComponent(*perception); 

	 if (perception)
	 {
		 perception->ConfigureSense(*sight);
		 perception->SetDominantSense(sight->GetSenseImplementation());
		 decisions = new CDecisionClass(perception, this);
		 //animAttack =      &decisions->aniAttack; 
		 //animDie =         &decisions->aniDie; 
		 //animFoundFoe =    &decisions->aniFoundFoe; 
		 //animFoundFriend = &decisions->aniFoundFriend; 
		 //animWounded =     &decisions->aniWounded; 
	 }
	 else 
		 UE_LOG(LogTemp, Warning, TEXT("Failed to create UAIPerceptionComponent"));
	  
	 Tags.Add("CAI"); 
 }

 void ACustomController::BeginPlay()
 {
	 if (perception)
	 {
		 perception->OnPerceptionUpdated.AddDynamic(this, &ACustomController::PerceptionUpdated);
	 }

	 UAIPerceptionSystem::RegisterPerceptionStimuliSource(this, UAISense_Sight::StaticClass(), this);

	 Super::BeginPlay();
 }

 void ACustomController::Possess(APawn * InPawn)
 {
	 Super::Possess(InPawn);
	 OnPossess(InPawn);
	 startPos = GetPawn()->GetActorLocation();
 }

 void ACustomController::OnPossess(APawn * In)
 {
	 if (sight)
	 {
		 sight->SightRadius = 1536;
		 sight->LoseSightRadius = 2048;
		 sight->PeripheralVisionAngleDegrees = 200;
		 sight->DetectionByAffiliation.bDetectEnemies = true;
		 sight->DetectionByAffiliation.bDetectFriendlies = true;
		 sight->DetectionByAffiliation.bDetectNeutrals = true;
		 sight->AutoSuccessRangeFromLastSeenLocation = 200;

		 if (perception)
		 {
			 perception->ConfigureSense(*sight);
		 }
	 }
	 startPos = GetCharacter()->GetActorLocation();

 }

 void ACustomController::PerceptionUpdated(const TArray<AActor*>& Actors)
 {
	 for (int i = 0; i < Actors.Num(); i++)
	 {
		 UE_LOG(LogTemp, Warning, TEXT("See: %s"), *Actors[i]->GetName());
	 }
	 attention = decisions->UpdateVision(Actors); 
	 if (attention)
	 {
		 UE_LOG(LogTemp, Warning, TEXT("IsPayingAttentionTo: %s"), *attention->GetName());
	 }
	 else
	 {
		 UE_LOG(LogTemp, Warning, TEXT("No object to pay attention to"));
	 }
	//UE_LOG(LogTemp, Warning, TEXT("As int: %i"), (int)status);
	//
	////UE_LOG(LogTemp, Warning, TEXT("Passing status: %e"), status);
	//UE_LOG(LogTemp, Warning, TEXT("As int: %i"), (int)status);
 }

 void ACustomController::Tick(float DeltaTime)
 {
	 Super::Tick(DeltaTime); 
	 EStatusEnum tempStatus = IntToEnum(decisions->Update((int)status));
	 if (currentlyAnimated)
		 return;
	 //if (GetCharacter()->GetVelocity().IsNearlyZero(10))
		// timeStill += DeltaTime;
	 //if (timeStill == 4 && status != EStatusEnum::SE_IDLE)
	 //{
		// MoveToLocation(startPos);
		// timeStill = 0;
		// UE_LOG(LogTemp, Warning, TEXT("Held still too long, moving back"));
		// tempStatus = EStatusEnum::SE_IDLE;
	 //}
	// if (status == EStatusEnum::SE_REVIVE && )
	 if (tempStatus == status)
		 return; 



	 status = tempStatus; 
	 //Do the status thing if status has changed
	 switch (status)
	 {
	 case EStatusEnum::SE_IDLE:
		 break;
	 case EStatusEnum::SE_CHARGE:
		 MoveToActor(attention);
		 break;
	 case EStatusEnum::SE_DOWNED:
		 StopMovement(); 
		 break;
	 case EStatusEnum::SE_DEAD:
		 StopMovement(); 
		 break;
	 case EStatusEnum::SE_CHASE:
		 MoveToActor(attention); 
		 break;
	 case EStatusEnum::SE_ENGAGED:
		 break;
	 case EStatusEnum::SE_REVIVE:
		 MoveToActor(decisions->GetFriendRef());
		 break;
	 case EStatusEnum::SE_EMOTING:
		 break;
	 case EStatusEnum::SE_ATTACK:
		 status = EStatusEnum::SE_CHASE; 
		 break;
	 case EStatusEnum::SE_STALK:
		 MoveToActor(attention);
		 break;
	 default:
		 break;
	 }
 }

 EStatusEnum ACustomController::IntToEnum(int num)
 {
	 switch (num)
	 {
	 case 0:
		 return EStatusEnum::SE_IDLE;
		 break;
	 case 1:
		 return EStatusEnum::SE_CHARGE;
		 break;
	 case 2:
		 return EStatusEnum::SE_DOWNED;
		 break;
	 case 3:
		 return EStatusEnum::SE_DEAD;
		 break;
	 case 4:
		 return EStatusEnum::SE_CHASE;
		 break;
	 case 5:
		 return EStatusEnum::SE_ENGAGED;
		 break;
	 case 6:
		 return EStatusEnum::SE_REVIVE;
		 break;
	 case 7:
		 return EStatusEnum::SE_EMOTING;
		 break;
	 case 8:
		 return EStatusEnum::SE_ATTACK;
		 break;
	 case 9:
		 return EStatusEnum::SE_STALK;
		 break;
	 default:
		 break;
	 }
	 return EStatusEnum();
 }

 FPathFindingResult ACustomController::FindPath(const FPathFindingQuery& Query) const
 {
	 const ANavigationData* NavData = Query.NavData.Get();
	 DeleteAllBalls(); 
	 FPathFindingResult Result(ENavigationQueryResult::Error);

	 //Create a new path if we need to, or reuse an existing one
	 Result.Path = Query.PathInstanceToFill.IsValid() ? Query.PathInstanceToFill : NavData->CreatePathInstance<FNavigationPath>(Query);
	 //Path to return to Query
	 FNavigationPath* NavPath = Result.Path.Get();

	 if (NavPath != nullptr)
	 {
		 if ((Query.StartLocation - Query.EndLocation).IsNearlyZero())
		 {
			 Result.Path->GetPathPoints().Reset();
			 Result.Path->GetPathPoints().Add(FNavPathPoint(Query.EndLocation));
			 Result.Result = ENavigationQueryResult::Success;
		 }
		 else if (Query.QueryFilter.IsValid())
		 {
			 PathFindingAlgorithm(Query.StartLocation, Query.EndLocation, *NavPath);
	
			 //If there is a sizable jump between nodes, set IsPartial to true
			 TArray<FNavPathPoint>& pathPoints = NavPath->GetPathPoints(); 
			 //There will never be a jump command on the final node
			 for (int i = 0; i < pathPoints.Num() - 2; ++i)
			 {
				 //If there is a significant height disparity between nodes
				 if (!FVector((pathPoints[i + 1].Location - pathPoints[i].Location).Z).IsNearlyZero(30))
				 {
					 pathPoints[i].Flags = ENavAreaFlag::Jump;
					 UE_LOG(LogTemp, Warning, TEXT("Path Has Jump Node"));
				 }
			 }
			 NavPath->MarkReady();

			Result.Result = ENavigationQueryResult::Success;
		 }
	 }

	 return Result;
 }

 bool ACustomController::PathFindingAlgorithm(FVector startLoc, FVector endLoc, FNavigationPath& path) const
 {
	 UE_LOG(LogTemp, Warning, TEXT("Entered PathFindingAlgorithm"));

	 //Set max nodes here
	 CNodePool * m_nodePool = new (malloc(sizeof(CNodePool))) CNodePool(1000);		///< Pointer to node pool.
	 CNodeQueue* m_openList = new (malloc(sizeof(CNodeQueue))) CNodeQueue(1000);     ///< Pointer to open list queue.
	
	 if (!m_nodePool)
		 UE_LOG(LogTemp, Error, TEXT("Error: Out of Memory"));

	 auto world = GetWorld();
	 auto navSystem = world->GetNavigationSystem();
	 auto navData = navSystem->GetMainNavData(FNavigationSystem::DontCreate);
	 const ARecastNavMesh* navMesh = Cast<ARecastNavMesh>(navData);
	 //FNavMeshNodeFlags flags = navMesh->GetFlags();
	 //auto loc = startLoc;
	 auto extent = FVector(100.0f, 100.0f, 100.0f);
	 NavNodeRef startRef = navMesh->FindNearestPoly(startLoc, extent, navData->GetDefaultQueryFilter());
	 auto areaID = navMesh->GetPolyAreaID(startRef);
	 TArray<NavNodeRef> polyPool;
	 GetAllPolys(polyPool);

	 for (int i = 0; i < polyPool.Num(); i++)
	 {
		 FVector center;
		 navMesh->GetPolyCenter(polyPool[i], center);
		 //Cannot edit component properties outside of constructor AND cannot pass parameters?!
		 //spawnBall(center, FColor().Blue);  ///< Spawn ball here to show all nodes
		 //Create a node for each poly
		 m_nodePool->getNode(polyPool[i], true);
		 m_nodePool->getNodeAtPoly(polyPool[i])->pos = center;

	 }
	 TArray<FNavPathPoint>& pathPoints = path.GetPathPoints();
	 pathPoints.Empty();
	 //pathPoints.Add(startLoc);
	 // navMesh->GetPolyFlags(areaID, flags);
	  //getNode = true FindNode = false; 

	 CNode* startNode = m_nodePool->getNode(startRef, true);
	 startNode->pos = startLoc;
	 startNode->parentId = 0;
	 startNode->cost = 0;
	 startNode->total = Dist(&startLoc, &endLoc);
	 m_openList->push(startNode);
	 pathPoints.Add(startNode->pos); 
	 CNode* lastBestNode = startNode;
	 float lastBestNodeCost = startNode->total;
	 const int loopLimit = m_nodePool->getMaxNodes() + 1;
	 bool pathFound = false;
	 int loopCounter = 0;
	 NavNodeRef finalPoly = navMesh->FindNearestPoly(endLoc, extent, navData->GetDefaultQueryFilter());
	 FVector up = FVector::ZeroVector;

	 while (!m_openList->empty())
	 {
		 //Make balls higher for later searches
		 up.Z++;
		 up.Z++;
		 up.Z++;
		 up.Z++;
		 //Remove node from openlist and mark as closed 
		 CNode* bestNode = m_openList->pop();
		 FVector ballpos;
		 Add(&ballpos, &bestNode->pos, &up);
		 spawnBall(ballpos, FColor().Yellow); ///< Spawn ball here to show all nodes traversed
		 bestNode->flags = NODE_CLOSED;

		 //Path has been found
		 if (bestNode->id == finalPoly)
		 {
			 UE_LOG(LogTemp, Warning, TEXT("Path Has Been Found!"));
			 //lastBestNode = bestNode;
			 TArray<FVector> tempPath;
			 while (bestNode->parentId)
			 {
				 tempPath.Add(bestNode->pos);
				 bestNode = bestNode->parentId;
				 if (tempPath.Num() > 1000)
				 {
					 UE_LOG(LogTemp, Warning, TEXT("Loop error in parent backtrack"));
					 break;
				 }
			 }
			 //Reverse path
			 FVector average; 
			 //DeleteAllBalls(); ///< Use this function to only show final ball path
			 for (int i = tempPath.Num() - 1; i >= 0; --i)
			 {
				 pathPoints.Add(tempPath[i]);
				 //average += path[i]; 
				 //if ((!FVector(path[i] - path[i + 1]).Z).IsNearlyZero(30))
				 //{
					
                 //}
				 
				spawnBall(tempPath[i], FColor().Green); ///< Spawn ball here to show path found
			 }
			 pathPoints.Add(endLoc);
			 break;
		 }

		 loopCounter++;

		 //Incase infinite loop
		 if (loopCounter >= loopLimit * 2)
		 {
			 UE_LOG(LogTemp, Error, TEXT("Error: Loop Counter has exceeded normal limits"));
			 break;
		 }

		 //Get array of Neighbours
		 TArray<CNode*> neighbours;
		 if (!GetPolyNeighbors(bestNode->id, neighbours, m_nodePool))
			  UE_LOG(LogTemp, Error, TEXT("Error: Couldn't get PolyNeighbours"));


		 //Loop through neighbours and open nodes
		 for (int i = 0; i < neighbours.Num(); i++)
		 {
			 //If neighbour is valid
			 if (!neighbours[i]->id)
			 {
				 UE_LOG(LogTemp, Warning, TEXT("Error: Neighbour had no ID"));
				 continue;
			 }
				 
			 //Get node of neighbour
			 CNode* nodeRef = m_nodePool->getNodeAtPoly(neighbours[i]->id);
			 if (nodeRef->flags == NODE_CLOSED)
				 continue;

			 //If node hasn't got a parent, set this as parent
			 if (neighbours[i]->parentId == 0 && neighbours[i]->parentId != startNode)
				 neighbours[i]->parentId = bestNode;

			 if (nodeRef->flags == NODE_OPEN)
			 {
				 //Check if node is easier to traverse from this direction
				 if (nodeRef->cost > Dist(&nodeRef->pos, &nodeRef->parentId->pos) + nodeRef->parentId->cost)
					 nodeRef->cost = Dist(&nodeRef->pos, &nodeRef->parentId->pos) + nodeRef->parentId->cost;
				 if (nodeRef->total > (nodeRef->cost + Dist(&nodeRef->pos, &endLoc)))
				 {
					 nodeRef->total = nodeRef->cost + Dist(&nodeRef->pos, &endLoc);
					 nodeRef->parentId = bestNode;
				 }
				 m_openList->modify(nodeRef);
			 }
			 //If node has never been visited before
			 else
			 {
				 nodeRef->parentId = bestNode; 
				 nodeRef->cost = Dist(&nodeRef->pos, &nodeRef->parentId->pos) + nodeRef->parentId->cost;
				 nodeRef->total = nodeRef->cost + Dist(&nodeRef->pos, &endLoc);
				 m_openList->push(nodeRef);
				 nodeRef->flags = NODE_OPEN;
				 spawnBall(nodeRef->pos, FColor().Red); ///< Spawn ball here to show all visited nodes
			 }
		 }
		 // hasFoundNextLink = true; 
	//  }
		 //return true; 
	 }
	 if (m_openList->empty())
		 UE_LOG(LogTemp, Error, TEXT("No Path Found"));
	 return true; 
 }

 void ACustomController::FindPathForMoveRequest(const FAIMoveRequest & MoveRequest, FPathFindingQuery & Query, FNavPathSharedPtr & OutPath) const
 {
	 SCOPE_CYCLE_COUNTER(STAT_AI_Overall);

	 UNavigationSystem* NavSys = UNavigationSystem::GetCurrent(GetWorld());
	 if (NavSys)
	 {
		 FPathFindingResult PathResult = FindPath(Query);
		 //FPathFindingResult PathResult = NavSys->FindPathSync(Query);
		 if (PathResult.Result != ENavigationQueryResult::Error)
		 {
			 if (PathResult.IsSuccessful() && PathResult.Path.IsValid())
			 {
				 if (MoveRequest.IsMoveToActorRequest())
				 {
					 PathResult.Path->SetGoalActorObservation(*MoveRequest.GetGoalActor(), 100.0f);
				 }

				 PathResult.Path->EnableRecalculationOnInvalidation(true);
				 OutPath = PathResult.Path;
			 }
		 }
		 else
		 {
			 /* UE_VLOG(this, LogAINavigation, Error, TEXT("Trying to find path to %s resulted in Error")
			 , MoveRequest.IsMoveToActorRequest() ? *GetNameSafe(MoveRequest.GetGoalActor()) : *MoveRequest.GetGoalLocation().ToString());
			 UE_VLOG_SEGMENT(this, LogAINavigation, Error, GetPawn() ? GetPawn()->GetActorLocation() : FAISystem::InvalidLocation
			 , MoveRequest.GetGoalLocation(), FColor::Red, TEXT("Failed move to %s"), *GetNameSafe(MoveRequest.GetGoalActor()));*/
		 }
	 }
 }

 bool ACustomController::GetAllPolys(TArray<NavNodeRef>& Polys) const
 {
	 //Get Nav Data
	 const ANavigationData* NavData = CGetNavData();

	 const ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
	 if (!NavMesh)
		 return false;
	 
	 TArray<FNavPoly> EachPolys;
	 for (int32 v = 0; v < NavMesh->GetNavMeshTilesCount(); v++)
	 {
		 //CHECK IS VALID FIRST OR WILL CRASH!!! 
		 //Use continue in case the valid polys are not stored sequentially
		 if (!TileIsValid(NavMesh, v))
			 continue;

		 //Get all polys in map
		 NavMesh->GetPolysInTile(v, EachPolys);
	 }

	 //Add polys
	 for (int32 v = 0; v < EachPolys.Num(); v++)
		 Polys.Add(EachPolys[v].Ref);	 

	 return true;
 }

 TArray<AActor*> ACustomController::GetActorsWithName(FString staticMesh, FString skeletalMesh = "") const
 {
	 TArray<AActor*> result; 
	 for (TActorIterator<AStaticMeshActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	 {
		 // Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		 AStaticMeshActor *Mesh = *ActorItr;
		 if (ActorItr->GetName().Contains(staticMesh))
			 result.Add(*ActorItr); 
	 }
	 if (skeletalMesh != "")
	 for (TActorIterator<ACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	 {
		 // Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		 ACharacter *Mesh = *ActorItr;
		 if (ActorItr->GetName().Contains(skeletalMesh))
			 result.Add(*ActorItr);
	 }
	 return result; 
 }

 void ACustomController::spawnBall(FVector loc, FColor colour) const
 {
	 // FActorSpawnParameters spawnInfo;
	 // spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	 //UE_LOG(LogTemp, Warning, TEXT("Spawned Sphere"));
	 DrawDebugSphere(GetWorld(), loc, 24.0f, 4.0f, colour, true); 
	// ABall* ball = GetWorld()->SpawnActorDeferred<ABall>(ABall::StaticClass(), FTransform(FVector(loc.X, loc.Y + 5, loc.Z)));
	// ball->Colour(colour);
	// ball->FinishSpawning(FTransform(FVector(loc.X, loc.Y + 5, loc.Z)));
 }

 void ACustomController::DeleteAllBalls() const
 {
	 for (TActorIterator<ABall> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		 ActorItr->Destroy(); 
	 FlushPersistentDebugLines(GetWorld()); 
 }


 bool ACustomController::GetPolyNeighbors(NavNodeRef PolyID, TArray<CNode*>& Neighbors, CNodePool * m_nodePool) const
 {
	 if (!PolyID)
		 return false;

	 float searchDistance = 1000; 
	 TArray<NavNodeRef> polyPool;
	 GetAllPolys(polyPool);
	 auto hitOut = FHitResult(ForceInit);

	 //Ignore floors and characters when checking for interruptions
	 TArray<AActor*> temp = GetActorsWithName("Floor", "Character");
	 TArray<AActor*>& ActorsToIgnore = temp; 
	 FCollisionQueryParams TraceParams(FName(TEXT("VictoryCore Trace")), true, ActorsToIgnore[0]);
	 TraceParams.bTraceComplex = true;

	 //Ignore Actors
	 TraceParams.AddIgnoredActors(ActorsToIgnore);
	 //Check distance between poly and all other polys and return closest
	 
	 
		 //If within distance
		 //int distance = (Dist(&m_nodePool->getNodeAtPoly(polyPool[i])->pos, &m_nodePool->getNodeAtPoly(PolyID)->pos));
		 //UE_LOG(LogTemp, Warning, TEXT("Distance: %i"), distance);
	 for (int i = 0; i < polyPool.Num(); i++)
		 if ((Dist(&m_nodePool->getNodeAtPoly(polyPool[i])->pos, &m_nodePool->getNodeAtPoly(PolyID)->pos) < searchDistance))
		 { //and nothing between points
			 GetWorld()->LineTraceSingleByChannel(hitOut, m_nodePool->getNodeAtPoly(polyPool[i])->pos, m_nodePool->getNodeAtPoly(PolyID)->pos, ECC_Pawn, TraceParams);
			 if (hitOut.GetActor() == NULL || !FVector((m_nodePool->getNodeAtPoly(polyPool[i])->pos - m_nodePool->getNodeAtPoly(PolyID)->pos).Z).IsNearlyZero(30))
				 //and not node making query
				 if (polyPool[i] != PolyID)
					 Neighbors.Add(m_nodePool->getNodeAtPoly(polyPool[i]));
		 }
	 
	 if (Neighbors.Num() == 0)
		 return false; 

	return true;
 }


 /*const NavNodeRef PolyRef = static_cast<NavNodeRef>(PolyID);
 FVector *center = &m_nodePool->getNodeAtPoly(PolyRef)->pos;
 FVector *extents = new FVector(100.0f, 100.0f, 100.0f);*/
 //	FVector *bmin, *bmax;

 //Sub(bmin, center, extents);
 //Add(bmax, center, extents);

 // Find tiles the query touches.
 //int minx, miny, maxx, maxy;
 //m_nav->calcTileLoc(bmin, &minx, &miny);
 //m_nav->calcTileLoc(bmax, &maxx, &maxy);
 //
 //ReadTilesHelper TileArray;

 //int n = 0;
 //for (int y = miny; y <= maxy; ++y)
 //{
 //	for (int x = minx; x <= maxx; ++x)
 //	{
 //		int nneis = m_nav->getTileCountAt(x, y);
 //		const dtMeshTile** neis = (const dtMeshTile**)TileArray.PrepareArray(nneis);

 //		m_nav->getTilesAt(x, y, neis, nneis);
 //		for (int j = 0; j < nneis; ++j)
 //		{
 //			n += queryPolygonsInTile(neis[j], bmin, bmax, filter, polys + n, maxPolys - n);
 //			if (n >= maxPolys)
 //			{
 //				*polyCount = n;
 //				return DT_SUCCESS | DT_BUFFER_TOO_SMALL;
 //			}
 //		}
 //	}
 //}
 //*polyCount = n;