#pragma once

//For:
// - "Algo::MinElementBy<>()"
#include "../UnrealRanges_Common_Algorithm.h"
#include "UnrealRanges_Private_Utils.h"

//Unreal
#include "Containers/Array.h"
#include "Math/Vector.h"
#include "Misc/Optional.h"

//Std
#include <type_traits> //for std::underlying_type_t<>

namespace UnrealRanges{ namespace Algorithm{

	// ===============================| Exclude<...>(ArrayToExcludeFrom, RangeToExclude) |=======================================

	template <typename ValueType, typename AllocatorType, typename RangeToExcludeType>
	void Exclude(TArray<ValueType, AllocatorType>& ArrayToExcludeFrom, const RangeToExcludeType& RangeToExclude)
	{
		for (auto&& ElementToExclude : RangeToExclude)
		{
			ArrayToExcludeFrom.Remove(ElementToExclude);
		}
	}

	// ===============================| ClosestActorWithDistance<...>(Point, Range) |=======================================

	template<typename RangeType>
	auto ClosestActorWithDistance(const FVector& Point, RangeType&& Range)
	{
		//TODO: Add static check "IsActorsRange()" in the beginning

		// "auto" in lambda <=> "TPair<Distance, ActorType>"
		// "return-type" <=> "const TPair<Distance, ActorType>*"
		return UnrealRanges::Algorithm::MinElementBy(
			Range | ActorsWithDistanceRange(Point),
			[](auto ActorWithDistance)
			{
				return ActorWithDistance.Key;
			});
	}

	// ===============================| ClosestActorWithDistance<...>(Point, RadiusLimit, Range) |=======================================

	template<typename RangeType>
	auto ClosestActorWithDistance(const FVector& Point, const float RadiusLimit, RangeType&& Range)
	{
		//TODO: Add static check "IsActorsRange()" in the beginning

		// "auto" <=> "const TPair<Distance, ActorType>*"
		// "return-type" <=> "const TPair<Distance, ActorType>*"
		return UnrealRanges::Algorithm::MinElementBy(
			Range | ActorsNearblyWithDistanceRange(Point, RadiusLimit),
			[](auto ActorWithDistance)
			{
				return ActorWithDistance.Key;
			});
	}

	// ===============================| ClosestActor<...>(Point, Distance, Range) |=======================================

	template<typename RangeType>
	auto ClosestActor(const FVector& Point, const float Distance, RangeType&& Range)
	{
		// "auto" <=> "const TPair<Distance, ActorType>*"
		const auto ClosestPair = ClosestActorWithDistance(Point, Distance, Forward<RangeType>(Range));
		return ClosestPair ? ClosestPair.GetValue().Value : nullptr;
	}

	// ===============================| ClosestActor<...>(Point, Range) |=======================================

	template<typename RangeType>
	auto ClosestActor(const FVector& Point, RangeType&& Range)
	{
		// "auto" <=> "const TPair<Distance, ActorType>*"
		const auto ClosestPair = ClosestActorWithDistance(Point, Forward<RangeType>(Range));
		return ClosestPair.IsSet() ? ClosestPair.GetValue().Value : nullptr;
	}

	// ===============================| Shuffle<...>(ArrayToShuffle, ElementsNumToShuffle) |=======================================

	template <typename ElementType, typename Allocator>
	void Shuffle(
		TArray<ElementType, Allocator>& ArrayToShuffle, const TOptional<int32>& ElementsNumToShuffle)
	{
		const int32 Num = ArrayToShuffle.Num();

		const int32 NumToShuffle = ElementsNumToShuffle.IsSet() ?
			FMath::Min(Num, ElementsNumToShuffle.GetValue()) : Num;

		const int32 LastIndex = NumToShuffle - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			int32 IndexToSwap = FMath::RandRange(i, ArrayToShuffle.Num() - 1);

			if (i != IndexToSwap)
			{
				// swap these values
				APlayerStart* LeftSpawn = ArrayToShuffle[i];
				APlayerStart* RightSpawn = ArrayToShuffle[IndexToSwap];

				ArrayToShuffle[i] = RightSpawn;
				ArrayToShuffle[IndexToSwap] = LeftSpawn;
			}
		}
	}

	// ===============================| AddAllFromRange<...>(Collection, SourceRange) |=======================================

	template<typename CollectionToFill, typename SourceRangeType>
	void AddAllFromRange(CollectionToFill& Collection, const SourceRangeType& SourceRange)
	{
		for (const auto& RangeElement : SourceRange)
		{
			Collection.Add(RangeElement);
		}
	}

	// ===============================| RangeToArray<...>(Range) |=======================================

	template<typename RangeType>
	auto RangeToArray(const RangeType& Range)
	{
		return RangeToArray<FDefaultAllocator>(Range);
	}

	// ===============================| RangeToArray<ArrayAllocatorType, ...>(Range) |=======================================

	template<typename ArrayAllocatorType, typename RangeType>
	auto RangeToArray(const RangeType& Range)
	{
		using ValueType = UnrealRanges::Private::Utils::RangeElementType<RangeType>;

		TArray<ValueType, ArrayAllocatorType> Result;
		AddAllFromRange(Result, Range);
		return Result;
	}

	// ====================================================================
	// ====================== Common overloadings =========================
	// ====================================================================

	// -------------------------[ RANGE == TArray ]-------------------------------

	template <typename ValueType, typename AllocatorType>
	const ValueType& GetByIndex(const TArray<ValueType, AllocatorType>& Range, const unsigned int Index)
	{
		return Range[Index];
	}

	template <typename ValueType, typename AllocatorType>
	UnrealRanges::Utils::TUnrealRanges_Optional<ValueType> GetByIndexSafe(
		const TArray<ValueType, AllocatorType>& Range, const unsigned int Index)
	{
		return Range.IsValidIndex(Index) ?
			TOptional<ValueType>{ Range[Index] } :
			TOptional<ValueType>{ };
	}

	template <typename ValueType, typename AllocatorType>
	unsigned int GetRangeCount(const TArray<ValueType, AllocatorType>& Range)
	{
		return Range.Num();
	}

	template <typename ValueType, typename AllocatorType>
	bool RangeHasElements(const TArray<ValueType, AllocatorType>& Range)
	{
		return Range.Num() > 0;
	}

	template <typename ValueType, typename AllocatorType>
	auto RangeFirstElement(const TArray<ValueType, AllocatorType>& Range)
	{
		return GetByIndexSafe(Range, 0);
	}

	template <typename ValueType, typename AllocatorType>
	auto RangeLastElement(const TArray<ValueType, AllocatorType>& Range)
	{
		return RangeHasElements(Range) ?
			GetByIndex(Range, Range.Num() - 1) :
			TOptional<ValueType>{ };
	}

	// -------------------------[ RANGE == TSet ]-------------------------------

	template<typename InElementType, typename KeyFuncs, typename Allocator>
	unsigned int GetRangeCount(const TSet<InElementType, KeyFuncs, Allocator>& Range)
	{
		return Range.Num();
	}

	template<typename InElementType, typename KeyFuncs, typename Allocator>
	bool RangeHasElements(const TSet<InElementType, KeyFuncs, Allocator>& Range)
	{
		return Range.Num() > 0;
	}

	// -------------------------[ RANGE == TMap ]-------------------------------

	template<typename InKeyType, typename InValueType, typename SetAllocator, typename KeyFuncs>
	unsigned int GetRangeCount(const TMap<InKeyType, InValueType, SetAllocator, KeyFuncs>& Range)
	{
		return Range.Num();
	}

	template<typename InKeyType, typename InValueType, typename SetAllocator, typename KeyFuncs>
	bool RangeHasElements(const TMap<InKeyType, InValueType, SetAllocator, KeyFuncs>& Range)
	{
		return Range.Num() > 0;
	}

}} //namespace UnrealRanges::Algorithm
