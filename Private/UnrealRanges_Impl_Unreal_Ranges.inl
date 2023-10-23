#pragma once

//For:
// - "IsValid_Universal()"
#include "UnrealRanges_Private_UnrealSpecific.h"

// Unreal
#include "Templates/Casts.h" //for casted ranges
#include "Templates/Tuple.h" //for "RangeWithProjectionResult<>()"
#include "Templates/UnrealTypeTraits.h"//for "TIsDerivedFrom<>" asserting
#include "Math/Vector.h"

namespace UnrealRanges{

	// ==================================================================
	// ======================| Valid check & Casting |===================
	// ==================================================================

	// --------- ValidRange() ---------

	auto ValidRange()
	{
		return FilteredRange([](auto Value)
		{
			return UnrealRanges::Private::UnrealSpecific::IsValid_Universal(Value);
		});
	}

	// --------- CastedRange<TargetType>() ---------

	template<typename TargetType>
	auto CastedRange()
	{
		return TransformedRange([](auto Value)
		{
			return Cast<TargetType>(Value);
		});
	}

	template<typename TargetType>
	auto CastedRange(const TSubclassOf<TargetType>& BaseClass)
	{
		return TransformedRange([BaseClass](auto Value)
			{
				return Value && Value->GetClass()->IsChildOf(BaseClass.Get()) ?
					CastChecked<TargetType>(Value) :
					nullptr;
			});
	}

	// --------- ValidCastedRange<TargetType>() ---------

	template<typename TargetType>
	auto ValidCastedRange()
	{
		return CastedRange<TargetType>() | ValidRange();
	}

	// ==================================================================
	// ==================| Common members accessing |====================
	// ==================================================================

	// --------- GetCallRange() ---------

	auto GetCallRange()
	{
		return TransformedRange([](auto Value)
		{
			return Value.Get();
		});
	}

	// --------- KeyFieldsRange() ---------

	auto KeyFieldsRange()
	{
		return TransformedRange([](auto Value) { return Value.Key; });
	}

	// --------- ValueFieldsRange() ---------

	auto ValueFieldsRange()
	{
		return TransformedRange([](auto Value) { return Value.Value; });
	}

	// ==================================================================
	// ==========================| Combining |===========================
	// ==================================================================

	// --------- RangeWithProjectionResult<...>(Projection) ---------

	template<typename ProjectionType>
	auto RangeWithProjectionResult(ProjectionType&& Projection)
	{
		return TransformedRange(
			[ProjectionForwarded = std::forward<ProjectionType>(Projection)](auto Value)
			{
				return MakeTuple(ProjectionForwarded(Value), Value);
			});
	}

	// ======================================================================
	// =======================| Common Actor ranges |========================
	// ======================================================================

	// --------- ActorsRange<ActorType>(World) ---------

	template<typename ActorType>
	auto ActorsRange(const UObject* WorldContextObject, TSubclassOf<ActorType> InClass)
	{
		static_assert(TIsDerivedFrom<ActorType, AActor>::Value,
			"\"ActorType\" should be inherited from \"AActor\"");

		//TODO: Support selecting empty Range if "World" was not access currectly here
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);

		return ConditionalIteratorRange(TActorIterator<ActorType>{ World, InClass });
	}

	// --------- ControllersRange<ControllerType>(World) ---------

	template<typename ControllerType>
	auto ControllersRange(const UObject* WorldContextObject)
	{
		static_assert(TIsDerivedFrom<ControllerType, AController>::Value,
			"\"ControllerType\" should be inherited from \"AController\"");

		return ActorsRange<ControllerType>(WorldContextObject);
	}

	// =============================================================================
	// ===========================| Actors spacial  |===============================
	// =============================================================================

	// ------ ActorsWithDistanceRange(Point) -------

	// Returns "Range<TPair<Distance, Actor>>"
	auto ActorsWithDistanceRange(const FVector& Point)
	{
		//TODO: Add static check "IsActorsRange()" in the beginning
		return RangeWithProjectionResult([Point](auto Value)
		{
			return FVector::Distance(Point, Value->GetActorLocation());
		});
	}

	// ------ ActorsWithDistanceSquaredRange(Point) -------

	// Returns "Range<TPair<Distance, Actor>>"
	auto ActorsWithDistanceSquaredRange(const FVector& Point)
	{
		//TODO: Add static check "IsActorsRange()" in the beginning
		return RangeWithProjectionResult([Point](auto Value)
		{
			return FVector::DistSquared(Point, Value->GetActorLocation());
		});
	}

	// ------ ActorsFilterByDistance<DistanceQueryPredicate>(Point, Distance)  -------

	template<Utils::Logic::EDistanceQueryPredicate DistanceQueryPredicate>
	auto ActorsFilterByDistance(const FVector& Point, const float Distance)
	{
		//TODO: Add static check "IsActorsRange()" in the beginning
		//TODO: Optimise using Squared Distance

		static_assert(
			DistanceQueryPredicate == Utils::Logic::Close || DistanceQueryPredicate == Utils::Logic::Far,
			"Invalid DistanceQueryPredicate template argument value passed");

		//TODO: Move to seprate function and reuse in "ActorsWithDistanceFilterByDistance<...>(...)"
		auto IsPassed = [ClampedDistanceToCheck = FMath::Max(Distance, 0.0f)](const float Distance)
		{
			switch (DistanceQueryPredicate)
			{
				case Utils::Logic::Close:  return Distance < ClampedDistanceToCheck;
				case Utils::Logic::Far:    return Distance > ClampedDistanceToCheck;
				default:                   return false;
			}
		};

		return ActorsWithDistanceRange(Point) |
			FilteredAndTransformedRange([IsPassed]
			(auto ActorWithDistance) //"auto" <=> TPair<Distance, Actor*>
			{
				return UnrealRanges::Utils::MakeOptional(IsPassed(ActorWithDistance.Key), ActorWithDistance.Value);
			});
	}

	// ------ ActorsWithDistanceFilterByDistance<DistanceQueryPredicate>(Point, Distance) ------- 

	template<Utils::Logic::EDistanceQueryPredicate DistanceQueryPredicate>
	auto ActorsWithDistanceFilterByDistance(const FVector& Point, const float Distance)
	{
		//TODO: Add static check "IsActorsRange()" in the beginning
		//TODO: Optimise using Squared Distance

		static_assert(
			DistanceQueryPredicate == Utils::Logic::Close || DistanceQueryPredicate == Utils::Logic::Far,
			"Invalid DistanceQueryPredicate template argument value passed");

		auto IsPassed = [ClampedDistanceToCheck = FMath::Max(Distance, 0.0f)](const float Distance)
		{
			switch (DistanceQueryPredicate)
			{
				case Utils::Logic::Close:  return Distance < ClampedDistanceToCheck;
				case Utils::Logic::Far:    return Distance > ClampedDistanceToCheck;
				default:            return false;
			}
		};

		return ActorsWithDistanceRange(Point) |
			FilteredRange([IsPassed]
			(auto ActorWithDistance) //"auto" <=> TPair<Distance, Actor*>
			{
				return IsPassed(ActorWithDistance.Key);
			});
	}

	// ------------------------------- OutersRecursiveRange() ----------------------------------- 

	auto OutersRecursiveRange(UObject* Top)
	{
		return RecursiveRange(Top, [](UObject* Current)
		{
			UObject* Outer = Current->GetOuter();
			return UnrealRanges::Utils::MakeOptional(static_cast<bool>(Outer), Outer);
		});
	}

	// ------------------------------- OutersRecursiveRange() ----------------------------------- 

	auto OwnersRecursiveRange(AActor* Top)
	{
		return RecursiveRange(Top, [](AActor* Current)
		{
			AActor* Owner = Current->GetOwner();
			return Owner ?
				UnrealRanges::Utils::MakeOptional<AActor*>(Owner) :
				UnrealRanges::Utils::MakeOptional<AActor*>();
		});
	}

	// ---------------------------------- ToStringRange() ---------------------------------------

	auto ToStringRange()
	{
		return TransformedRange([](auto Value)
		{
			//TODO: Support here universal dereference
			return Value.ToString();
		});
	}

} //namespace UnrealRanges
