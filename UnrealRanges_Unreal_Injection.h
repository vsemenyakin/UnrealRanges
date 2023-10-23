#pragma once

#ifndef UnrealRanges_Injection

#include "Misc/Optional.h"
#include "Templates/UnrealTemplate.h"//for Forward<>()
#include "Misc/AssertionMacros.h"//for check()

namespace UnrealRanges{ namespace Injection{ namespace Unreal{

	// ======================= Optional ============================

	template<typename ValueType>
	using TUnrealRanges_Optional = TOptional<ValueType>;

	template<typename ValueType>
	TUnrealRanges_Optional<ValueType> MakeOptional(const bool InSet, const ValueType& InValue)
	{
		return InSet ?
			TOptional<ValueType>{ InValue } :
			TOptional<ValueType>{ };
	}

	template<typename ValueType>
	bool IsOptionalSet(const TUnrealRanges_Optional<ValueType>& InOptional)
	{
		return InOptional.IsSet();
	}

	template<typename ValueType>
	const ValueType& GetOptionalValue(const TUnrealRanges_Optional<ValueType>& InOptional)
	{
		return InOptional.GetValue();
	}

	// =================== Random =====================

	inline int Rand(int Start, int End)
	{
		//TODO: Add include for FMath::RandRange
		return static_cast<int>(
			FMath::RandRange(
				static_cast<int>(Start),
				static_cast<int>(End)
			)
		);
	}

	// =================== Check ======================

#	define UNREAL_RANGES__DEBUG DO_CHECK //setup from Unreal

	inline void Check(const bool bCondition)
	{
		check(bCondition);
	}

	inline void Check(const bool bCondition, const char* Message)
	{
		checkf(bCondition, TEXT("%s"), Message);
	}

}}} //UnrealRanges::Injection::Unreal

#define UnrealRanges_Injection UnrealRanges::Injection::Unreal // <<< Injection define <<<

#endif
