#pragma once

#include "../UnrealRanges_Utils.h"//for "TUnrealRanges_Optional<>"

//For:
// - "::RangeElementType<>()"
// - "UnrealRanges_RandRange()"
#include "UnrealRanges_Private_Utils.h"

#include <utility> //for "std::forward<...>"
#include <type_traits> //for "std::decay_t<...>"

namespace UnrealRanges{ namespace Algorithm{

	// ===============================| Contains<...>(Range, Value) |=======================================

	template <typename RangeType, typename ValueType>
	bool Contains(RangeType&& Range, ValueType&& Value)
	{
		for (auto&& Elem : Range)
			if (Elem == Value)
				return true;

		return false;
	}

	// ===============================| ContainsByPredicate<...>(Range, Pred) |=======================================

	template <typename RangeType, typename PredicateType>
	bool ContainsByPredicate(RangeType&& Range, PredicateType&& Pred)
	{
		return UnrealRanges::Utils::IsOptionalSet(
			FindByPredicate(std::forward<RangeType>(Range), Pred)
		);
	}

	// ===============================| ContainsBy<...>(Range, Value, Proj) |=======================================

	template <typename RangeType, typename ValueType, typename ProjectionType>
	bool ContainsBy(RangeType&& Range, const ValueType& Value, ProjectionType&& Proj)
	{
		return !!FindBy(std::forward<RangeType>(Range), Value, Proj);
	}

	// ================================| Select<...>(Range, SelectionFunc) |========================================

	template <typename RangeType, typename SelectionWithFirstPickFuncType>
	auto SelectWithFirstPick(RangeType&& Range, SelectionWithFirstPickFuncType&& SelectionFunc)
	{
		using ValueType = UnrealRanges::Private::Utils::RangeElementType<RangeType>;

		TUnrealRanges_Optional<ValueType> Best;

		for (auto Current : Range)
		{
			if (SelectionFunc(Current, Best))
			{
				Best = UnrealRanges::Utils::MakeOptional(Current);
			}
		}

		return Best;
	}

	template <typename RangeType, typename SelectionFuncType>
	auto Select(RangeType&& Range, SelectionFuncType&& SelectionFunc)
	{
		using ValueType = UnrealRanges::Private::Utils::RangeElementType<RangeType>;

		return SelectWithFirstPick(Range,
			[SelectionFuncForwared = std::forward<SelectionFuncType>(SelectionFunc)]
			(const ValueType& Current, const TUnrealRanges_Optional<ValueType>& Best)
			{
				return !UnrealRanges::Utils::IsOptionalSet(Best) || SelectionFuncForwared(Current, Best.GetValue());
			});
	}

	// ===============================| MinElementBy<...>(Range, Proj) |=======================================

	template <typename RangeType, typename ProjectionType>
	auto MinElementBy(RangeType&& Range, ProjectionType&& Proj)
	{
		using ValueType = UnrealRanges::Private::Utils::RangeElementType<RangeType>;

		return Select(
			Range,
			[ProjForwared = std::forward<ProjectionType>(Proj)](const ValueType& Current, const ValueType& Best)
			{
				return ProjForwared(Current) < ProjForwared(Best);
			});
	}

	// ===============================| MaxElementBy<...>(Range, Proj) |=======================================

	template <typename RangeType, typename ProjectionType>
	auto MaxElementBy(RangeType&& Range, ProjectionType&& Proj)
	{
		using ValueType = UnrealRanges::Private::Utils::RangeElementType<RangeType>;

		return Select(
			Range,
			[ProjForwared = std::forward<ProjectionType>(Proj)](const ValueType& Current, const ValueType& Best)
			{
				return ProjForwared(Current) > ProjForwared(Best);
			});
	}

	// ===============================| FindByPredicate<...>(Range, Proj) |=======================================

	template <typename RangeType, typename PredicateType>
	auto FindByPredicate(RangeType&& Range, PredicateType&& Pred)
	{
		using ValueType = UnrealRanges::Private::Utils::RangeElementType<RangeType>;

		for (auto&& Elem : Range)
		{
			if (Pred(Elem))
			{
				return UnrealRanges::Utils::MakeOptional(Elem);
			}
		}

		return UnrealRanges::Utils::MakeOptional<ValueType>();
	}

	template<typename RangeType, typename PredicateType>
	auto IndexByPredicate(RangeType&& Range, PredicateType&& Pred)
	{
		unsigned int Index = 0;
		
		for (auto&& Elem : Range)
		{
			if (Pred(Elem))
			{
				return UnrealRanges::Utils::MakeOptional(Index);
			}

			++Index;
		}

		return UnrealRanges::Utils::MakeOptional<unsigned int>();
	}

	// ===============================| FindNextByPredicate<...>(Range, Pred) |=======================================

	template<typename RangeType, typename PredicateType>
	auto FindNextByPredicate(RangeType&& Range, PredicateType&& Pred)
	{
		using ValueType = UnrealRanges::Private::Utils::RangeElementType<RangeType>;
		bool bFoundByPredicate = false;

		for (auto&& Elem : Range)
		{
			if (bFoundByPredicate)
			{
				return UnrealRanges::Utils::MakeOptional(Elem);
			}

			bFoundByPredicate = Pred(Elem);
		}

		return UnrealRanges::Utils::MakeOptional<ValueType>();
	}

	// ===============================| GetRangeCount<...>(Range) |=======================================

	template <typename RangeType>
	auto GetByIndex(RangeType&& Range, const unsigned int Index)
	{
		return UnrealRanges::Utils::GetOptionalValueChecked(
			UnrealRanges::Utils::GetByIndexSafe(Range, Index));
	}

	template <typename RangeType>
	auto GetByIndexSafe(RangeType&& Range, const unsigned int Index)
	{
		using ValueType = UnrealRanges::Private::Utils::RangeElementType<RangeType>;

		return FindByPredicate(Range,
			[Index, CurrentIndex = 0](const ValueType&) mutable
			{
				return (CurrentIndex++) == Index;
			});
	}

	// ===============================| GetRangeCount<...>(Range) |=======================================

	template <typename RangeType>
	int GetRangeCount(const RangeType& Range)
	{
		// TODO: Make overloading for Unreal collections

		int Count = 0;
		for (auto Unused : Range)
		{
			++Count;
		}

		return Count;
	}

	// ===============================| GetRandomRangeElement<...>(Range, Pred) |=======================================

	template <typename RangeType>
	auto GetRandomRangeElement(const RangeType& Range)
	{
		using ValueType = UnrealRanges::Private::Utils::RangeElementType<RangeType>;

		const int Count = GetRangeCount(Range);

		return Count > 0 ?
			GetByIndexSafe(Range, UnrealRanges::Utils::Rand(0, Count - 1)) :
			UnrealRanges::Utils::MakeOptional<ValueType>();
	}

	// ===============================| RangeHasElements<...>(Range) |=======================================

	template<typename RangeType>
	bool RangeHasElements(const RangeType& Range)
	{
		for (const auto& Unused : Range)
		{
			return true;
		}

		return false;
	}

	// ===============================| RangeFirstElement<...>(Range) |=======================================

	template<typename RangeType>
	auto RangeFirstElement(const RangeType& Range)
	{
		using ValueType = UnrealRanges::Private::Utils::RangeElementType<RangeType>;

		for (const ValueType& RangeElement : Range)
		{
			return UnrealRanges::Utils::MakeOptional(RangeElement);
		}

		return UnrealRanges::Utils::MakeOptional<ValueType>();
	}

	// ===============================| RangeLastElement<...>(Range) |=======================================

	template<typename RangeType>
	auto RangeLastElement(const RangeType& Range)
	{
		using ValueType = UnrealRanges::Private::Utils::RangeElementType<RangeType>;

		TUnrealRanges_Optional<ValueType> Result = UnrealRanges::Utils::MakeOptional<ValueType>();

		for (const ValueType& Current : Range)
		{
			Result = UnrealRanges::Utils::MakeOptional<ValueType>(Current);
		}

		return Result;
	}

	// ===============================| EqualsToSome<...>(Value, ValuesToCompare) |=======================================

	template<typename ValueType, typename RangeType>
	bool EqualsToSome(const ValueType& Value, const RangeType& Range)
	{
		for (const ValueType& ValueToCompare : Range)
		{
			if (Value == ValueToCompare)
			{
				return true;
			}
		}

		return false;
	}

	// ===============================| EqualsToSome<...>(Value, Range) |=======================================

	template<typename ValueType>
	bool EqualsToSome(const ValueType& Value, const std::initializer_list<ValueType>& ValuesToCompare)
	{
		return EqualsToSome<ValueType, std::initializer_list<ValueType>>(Value, ValuesToCompare);
	}

}} // namespace UnrealRanges::Algorithm
