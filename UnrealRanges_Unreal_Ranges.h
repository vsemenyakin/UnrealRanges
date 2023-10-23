#pragma once

//Unreal
#include "Engine/World.h"// for ActorsRange<>()
#include "Math/UnrealMathUtility.h"//for FVector usages

namespace UnrealRanges{


	// ****************** !!! NB !!! ******************
	// Current Range implementation does not support iteration
	// over mutable values. Changing of elements got during
	// iterations from Ranges may cause Undefined Behaviour
	// 
	// Mutating of Range elements will be supported in next
	// Range implementation version
	// ****************** !!! NB !!! ******************


	// ****************** !!! NB !!! ******************
	// Please, pay and attention for SPECIAL NOTES:
	//  1. For "ActorsRange<ActorType>(World)"
	//   Use it instead of Unreal "TActorsRange", see
	//   "UnrealRanges::ActorsRange<>()" comments for
	//   details.
	// ****************** !!! NB !!! ******************


	// ****************** Useful tip ******************
	// If a lot of documantion in code is anoying for you
	// and you are using Visual Studio, you may use next
	// default shortcuts to operate with it:
	//
	// "Ctrl + M + O" = collapsing of all regions in file
	// "Ctrl + M + L" = expanding of all regions in file
	// 
	// ****************** Useful tip ******************


	// ==================================================================
	// ======================| Valid check & Casting |===================
	// ==================================================================

	// --------- ValidRange() ---------
	// 
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	// 
	// Filter range by "IsValid_Universal({Element})" checking for each element 
	//
	// ~~~~ Example ~~~~
	// 
	//  // Iterate over valid Actors
	// 
	//  TArray<AActor*> Actors{ Actor1, Actor2, nullptr, Actor4 };
	// 
	//  for (AActor* Actor : Actors | ValidRange())
	//  {
	//      // "Actor" during iterations will be equals to:
	//      //  "Actor1", "Actor2" and "Actor4"
	//  }
	//
	auto ValidRange();

	// --------- CastedRange<TargetType>() ---------
	//
	// IN:
	//    <1> TargetType
	//       Type to witch Parent Range elements should be casted
	//
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	// 
	// Transform range using "Cast<TargetType>({Element})" for each element
	//
	// ! NB: Output range has no validity filtration! In most cases
	// it's better to use "ValidCastedRange<TargetType>()"
	//
	// ~~~~ Example ~~~~
	//
	//  // Iterate over Actors casted to Pawn
	// 
	//  TArray<AActor*> Actors{ NotPawnActor, Pawn, nullptr, Soldier };
	// 
	//  for (APawn* Pawn : Actors | CastedRange<APawn>())
	//  {
	//      // "Pawn" during iterations will be equals to:
	//      //  "nullptr", "Pawn", "nullptr", "Soldier"
	//  }
	//
	template<typename TargetType>
	auto CastedRange();

	// --------- ValidCastedRange<TargetType>() ---------
	//
	// IN:
	//   <1> TargetType
	//      Type to witch Parent Range should be casted and validated after it
	//
	// OUT:
	//   {ret}
	//        Range-Part that performs actions described below...
	// 
	// Transform range using "Cast<TargetType>({Element})" for each element &
	// filter only valid values (by "IsValid_Universal({Element})" check) after casting
	//
	// ~~~~ Example ~~~~
	//
	//  // Iterate over Actors casted to Pawn & valid after casting
	// 
	//  TArray<AActor*> Actors{ NotPawnActor, Pawn, nullptr, Soldier };
	// 
	//  for (APawn* Pawn : Actors | ValidCastedRange<APawn>())
	//  {
	//      // "Pawn" during iterations will be equals to:
	//      //  "Pawn", "Soldier"
	//  }
	//
	template<typename TargetType>
	auto ValidCastedRange();

	// ==================================================================
	// ==================| Common members accessing |====================
	// ==================================================================

	// --------- GetCallRange() ---------
	//
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	// 
	// Transform range using "{Element}.Get()" (with no arguments) call for each element
	//
	// ! NB: Output range has no validity filtration. In most cases "GetCallRange()" will
	// be used with "ValidRange()", next way: " ... | GetCallRange() | ValidRange() | ..."
	//
	// ~~~~ Example ~~~~
	// 
	//  TArray<TWeakObjectPtr<AActor>> WeakActors{ Actor1, Actor2, nullptr, Actor4 };
	// 
	//  for (AActor* Actor : WeakActors | GetCallRange())
	//  {
	//      // By getting values using "Get()" from Weak Ptrs
	//      // in "WeakActors", "Actor" during iterations
	//      // will be equals to:
	//      //  "Actor1", "Actor2", "nullptr" and "Actor4"
	//  }
	//
	auto GetCallRange();
	
	// --------- KeyFieldsRange() & ValueFieldsRange() ---------
	//
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	//
	// Form transformed range of "{Element}.Key" ([Function #1])
	// and "{Element}.Value" ([Function #2]) values.
	// Useful for iterating over "TMap<...>" collection elements.
	//
	// ~~~~ Examples ~~~~
	//
	//  //.[Common example types & variables]
	// 
	//  enum class EEnum : uint8
	//  {
	//     One,
	//     Two,
	//     Three
	//  };
	//  
	//  TMap<EEnum, FString> EnumToStringMapping
	//  {
	//     { EEnum::One,   Text("One")   },
	//     { EEnum::Two,   Text("Two")   },
	//     { EEnum::Three, Text("Three") }
	//  };
	//
	//  //.[Example #1]
	//  // Iteration over Map Keys using [Function #1]
	//
	//  for (EEnum Key : EnumToStringMapping | KeyFieldsRange())
	//  {
	//      // "Key" during iterations will be equals to:
	//      //  "EEnum::One", "EEnum::Two", "EEnum::Three"
	//  }
	//
	//  //.[Example #2]
	//  // Iteration over Map Values using [Function #2]
	//
	//  for (const FString& Value : EnumToStringMapping | ValueFieldsRange())
	//  {
	//      // "Key" during iterations will be equals to:
	//      //  "FString{ TEXT("One")   }",
	//      //  "FString{ TEXT("Two")   }",
	//      //  "FString{ TEXT("Three") }"
	//  }
	//
	// [Function #1]
	auto KeyFieldsRange();

	// [Function #2]
	auto ValueFieldsRange();

	// ==================================================================
	// ==========================| Combining |===========================
	// ==================================================================
	
	//TODO: Implement Tuple for C++14 and move this type of ranges to Common_Ranges

	// --------- RangeWithProjectionResult<...>(Projection) ---------
	//
	// IN:
	//    (1) Projection
	//       Projection that is used to compute value added
	//       as first element of resulting Range-Part element
	//
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	//
	// Form transformed range that contains "TPair<Projection({Element}), {Element}>"
	// values. Usefull for Ranges that should make some additions computing during
	// iterations (example: iterating over objects with computing distance to specified location)
	// and/or are used for some additional info based on element states
	//
	// ~~~~ Example ~~~~
	// 
	//  enum class ETargetHitStatus : uint8
	//  {
	//      Inside,
	//      Outside
	//  };
	// 
	//  bool GetTargetHitStatus(FVector HitLocation)
	//  {
	//     const FVector TargetLocation{ 0.f, 0.f };
	//     const float TargetRadius{ 1.f };
	//     
	//     const float DistanceFromTargetToHit = FVector::Distance(TargetLocation, HitLocation);
	//
	//     return (DistanceFromTargetToHit <= TargetRadius) ?
	//	       ETargetHitStatus::Inside : ETargetHitStatus::Outside;
	//  }
	// 
	//  TArray<FVector> Hits{ { 0.f, 0.f }, { 0.5f, 0.5f }, { 1.f, 1.f } };
	//
	//  for (TPair<ETargetHitStatus, FVector> HitWithStatus : Hits | RangeWithProjectionResult(&GetTargetHitStatus))
	//  {
	//      // "HitWithStatus" during iterations will be equals to:
	//      //  "TPair<ETargetHitStatus, FVector>{ ETargetHitStatus::Inside, { 0.f, 0.f } }"
	//      //  "TPair<ETargetHitStatus, FVector>{ ETargetHitStatus::Inside, { 0.5f, 0.5f } }"
	//      //  "TPair<ETargetHitStatus, FVector>{ ETargetHitStatus::Outside, { 1.f, 1.f } }"
	//  }
	//
	template<typename ProjectionType>
	auto RangeWithProjectionResult(ProjectionType&& Projection);

	// ======================================================================
	// =======================| Common Actor ranges |========================
	// ======================================================================

	// --------- ActorsRange<ActorType>(WorldContextObject) ---------
	//
	// IN:
	//    <1> ActorType
	//       Base type of actors to iterate
	//   
	//    (1) WorldContextObject
	//       Object that is used to get the World which should be used
	//       to iterate actors in
	// 
	// OUT:
	//    {ret}
	//         Range that described below...
	//
	// Range-adopted version of "TActorIterator<...>". Return Range
	// of Actors inherited from <1> "ActorType" in the World got using
	// (1) "WorldContextObject"
	//
	// !NB: Actors are guaranteed to be valid
	// (such that passes "IsValid({Element})" check).
	// 
	// ~~~~ Example ~~~~
	//
	//  for (ACharacter* Character : ActorsRange<ACharacter>(WorldContextObject))
	//  {
	//      // "Character" will be filled by pointers to all actors inherited from "ACharacter"
	//  }
	//
	// ================= !SPECIAL NOTE! =================
	//
	// Please, use this range instead of Unreal default
	// "TActorRange<...>" from "EngineUtils.h" when using
	// all Unreal Ranges library ranges.  It is needed because
	// of specific of Unreal "TActorIterator" API use
	// checking policy: it forbids call of "!=operator" with
	// "TActorIterator::End" value at the "left" side of the call,
	// meanwhile Unreal Ranges uses "IsEnd()" call using "!=operator"
	// call for "TActorIterator::End" values.
	//
	template<typename ActorType>
	auto ActorsRange(const UObject* WorldContextObject, TSubclassOf<ActorType> InClass = ActorType::StaticClass());

	// --------- ControllersRange<ControllerType>(WorldContextObject) ---------
	//
	// IN:
	//    <1>? ControllerType = "AController"
	//       Base type of Controllers to iterate. Should be
	//       inherited from "AController"
	//   
	//    (1) WorldContextObject
	//       Object that is used to get the World which should be used
	//       to iterate actors in
	// 
	// OUT:
	//    {ret}
	//         Range that described below...
	//
	// Return Range of Actors from <1> "ActorType" in the World
	// got using (1) "WorldContextObject"
	// 
	// ~~~~ Examples ~~~~
	//
	//  //.[Example #1]
	//  // Iteration over game-specific "AGameAIController" controllers
	// 
	//  for (AGameAIController* AIController :
	//      ControllersRange<AGameAIController>(WorldContextObject))
	//  {
	//      // "AIController" will be filled by pointers to all valid actors
	//      // inherited from "AGameAIController"
	//  }
	//
	//  //.[Example #2]
	//  // Using of defaulted ControllerType <1> argument value
	// 
	//  for (AController* Controller : ControllersRange(WorldContextObject))
	//  {
	//      // "Controller" will be filled by pointers to all actors
	//      // inherited from "ACharacter"
	//  }
	//
	template<typename ControllerType = AController>
	auto ControllersRange(const UObject* WorldContextObject);

	// =============================================================================
	// ===========================| Actors spacial  |===============================
	// =============================================================================

	// ------ ActorsWithDistanceRange(Point) & ActorsWithDistanceSquaredRange(Point) -------
	//
	// IN:
	//    (1) Point
	//       Point to compute distances to Parent Range Actors from
	// 
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	//
	// Form transformed range with "TPair<Distance {float}, Actor>>" Elements,
	// where "Distance = FVector::Distance({Element}->GetActorLocation(), {Point})" (for [Function #1])
	// or "Distance = FVector::DistSquared({Element}->GetActorLocation(), {Point})" (for [Function #2])
	// 
	// Usefull as part of simple World spacial requests
	// 
	// ~~~~ Examples ~~~~
	//
	//  //.[Example #1]
	//  // Passing damage with distance falloff to Actors
	// 
	//  const float BaseDamage = 100.f;
	//  const FVector DamageApplyPoint{ 1000.f, 1000.f, 1000.f };
	// 
	//  for (TPair<float, AGameUnit*> UnitWithDistance :
	//		ActorsRange<AGameUnit>(World) |
	//		ActorsWithDistanceRange(DamageApplyPoint))
	//  {
	//      UnitWithDistance.Value->ApplyDamageWithFalloff(BaseDamage, UnitWithDistance.Key);
	//  }
	//
	//  //.[Example #2]
	//  // Little-bit more optimized version of [Example #1] that uses
	//  // "ActorsWithDistanceSquaredRange(...)" for more effective distances
	//  // computing. Assumed that "ApplyDamageWithFalloff_SquaredDistance(...)"
	//  // is uses squared distance checks in implementation
	// 
	//  const float BaseDamage = 100.f;
	//  const FVector DamageApplyPoint{ 1000.f, 1000.f, 1000.f };
	// 
	//  for (TPair<float, AGameUnit*> UnitWithDistance :
	//		ActorsRange<AGameUnit>(World) |
	//		ActorsWithDistanceSquaredRange(DamageApplyPoint))
	//  {
	//      UnitWithDistance.Value->ApplyDamageWithFalloff_SquaredDistance(BaseDamage, UnitWithDistance.Key);
	//  }
	// 
	// [Function #1]
	auto ActorsWithDistanceRange(const FVector& Point);

	// [Function #2]
	auto ActorsWithDistanceSquaredRange(const FVector& Point);

	// ------ ActorsFilterByDistance<DistanceQueryPredicate>(Point, Distance) &  -------
	//    ActorsWithDistanceFilterByDistance<DistanceQueryPredicate>(Point, Distance)
	//
	// IN:
	//    <1> DistanceQueryPredicate
	//       Enum that setup query condition, type "Logic::EDistanceQueryPredicate" [Type #1]:
	//       If equals to "Logic::Close" then iterates over Actors closer then "Distance"
	//       If equals to "Logic::Far" then iterates over Actors more far then "Distance"
	// 
	//    (1) Point
	//       Point to compute distances to Parent Range Actors from
	// 
	//    (2) Distance
	//       Distance to Actors
	// 
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	// 
	// Form transformed range with Actors that more close (when "DistanceQueryPredicate == Logic::Close")
	// or more far away (when "DistanceQueryPredicate == Logic::Far") from "Point" then "Distance".
	//
	// [Function #2] works same as [Function #1], but form Range of pairs "TPair<Distance {float}, Actor*>"
	// 
	// ~~~~ Example ~~~~
	//
	//  //.[Example #1]
	//  // Applying Buff Effect in radius. Use [Function #1] with defaulted "Logic::Close" value
	//  // for "DistanceQueryPredicate" template argument
	// 
	//  const float BuffApplyRadius = 1000.f;
	//  const FVector BuffApplyPoint{ 500.f, 500.f, 500.f };
	// 
	//  for (AGameUnit* Unit :
	//		ActorsRange<AGameUnit>(World) |
	//		ActorsFilterByDistance(BuffApplyPoint, BuffApplyRadius))
	//  {
	//      Unit->ApplyBuff();
	//  }
	// 
	//  //.[Example #2]
	//  // Hiding actors that out of view distance. Use [Function #1] with exlicitly passed
	//  // "Logic::Far" value for "DistanceQueryPredicate" template argument
	// 
	//  const float MaxViewDistance = 10000.f;
	//  const FVector ViewPosition{ 1000.f, 1000.f, 1000.f };
	// 
	//  using namepsace Logic;
	//  for (AGameUnit* Unit :
	//		ActorsRange<AGameUnit>(World) |
	//		ActorsWithDistanceRange<Far>(ViewPosition, MaxViewDistance))
	//  {
	//      Unit->Hide();
	//  }
	//
	//  //.[Example #3]
	//  // Prioritizing targets for AI. Uses [Function #2] for collecting
	//  // Actors with distances. Use passed by default "Logic::Close" value
	//  // for "DistanceQueryPredicate" template argument
	//  
	//  const float AIUnitViewDistance = 5000.f;
	//  const FVector AIUnitPosition{ 300.f, 300.f, 300.f };
	// 
	//  for (TPair<float, AGameUnit*> UnitWithDistance :
	//		ActorsRange<AGameUnit>(World) |
	//		ActorsWithDistanceFilterByDistance(AIUnitPosition, AIUnitViewDistance))
	//  {
	//      // Possible signature of: "AddPrioritizedTarget(AGameUnit* Target, float Priority)"
	//      // More far <=> less prioritized target, so distance is negativized when passing to method
	//
	//      Unit->AddPrioritizedTarget(UnitWithDistance.Value, -UnitWithDistance.Key);
	//  }
	// 
	// [Type #1]
	
	namespace Utils{ namespace Logic{

		enum EDistanceQueryPredicate
		{
			Close,
			Far
		};

	}} //namespace ::Utils::Logic

	// [Function #1]
	template<Utils::Logic::EDistanceQueryPredicate DistanceQueryPredicate = Utils::Logic::Close>
	auto ActorsFilterByDistance(const FVector& Point, const float Distance);

	// [Function #2]
	template<Utils::Logic::EDistanceQueryPredicate DistanceQueryPredicate = Utils::Logic::Close>
	auto ActorsWithDistanceFilterByDistance(const FVector& Point, const float Distance);

	// -------------------------- OutersRecursiveRange(Top) -----------------------------
	//
	// IN:
	//    (1) Top
	//       "UObject" pointer to start iterating over parent Outers from
	// 
	// OUT:
	//    {ret}
	//         Range that contains from elements described below...
	//
	// First range element is "Top" UObject, then each next element is got by calling
	// "{Current}->GetOuter()" until "GetOuter()" is not "nullptr"
	//
	// ~~~~ Example ~~~~
	//
	//  AActor* ActorOnTheMap = GetMapActor();
	//
	//  for (UObject* Object : OutersRecursiveRange(ActorOnTheMap))
	//  {
	//      // "Object" will be filled by values of Outers. Here may be something like:
	//      // "{ActorOnTheMap} pointer", "{Map} pointer", "{GameInstance} pointer", et c.
	//  }
	//
	auto OutersRecursiveRange(UObject* Top);

	// -------------------------- OwnersRecursiveRange(Top) -----------------------------
	//
	// IN:
	//    (1) Top
	//       "AActor" pointer to start iterating over parent Owners from
	// 
	// OUT:
	//    {ret}
	//         Range that contains from elements described below...
	//
	// First range element is "Top" Actor, then each next element is got by calling
	// "{Current}->GetOwner()" until "GetOwner()" is not "nullptr"
	//
	// ~~~~ Example ~~~~
	//
	//  AActor* CharacterItem = GetCharacterItem();
	//
	//  for (AActor* Owner : OwnersRecursiveRange(CharacterItem))
	//  {
	//      // "Owner" will be filled by values of Owners. Here may be something like:
	//      // "{CharacterBackpack} pointer" and "{Character} pointer"
	//  }
	//
	auto OwnersRecursiveRange(AActor* Top);

	// --------- ValidRange() ---------
	// 
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	// 
	// Transform range by calling "{Element}.ToString()"
	// 
	// NB: Currently support only direct call, without Universal Dereference 
	//
	// ~~~~ Example ~~~~
	// 
	//  TArray<FName> NamesArray{
	//     FName{ TEXT("First") },
	//     FName{ },
	//     FName{ TEXT("Second") }
	//  };
	// 
	//  const FString NamesString = FString::Join(
	//      NamesArray | ValidRange() | ToStringRange(),
	//      TEXT(",")
	//  );
	// 
	//  // "NamesString" will be "First,Second"
	//
	auto ToStringRange();

} //namespace UnrealRanges

#include "Private/UnrealRanges_Impl_Unreal_Ranges.inl"
