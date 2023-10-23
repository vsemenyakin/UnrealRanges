#pragma once

#include "../UnrealRanges_Common_Injection.h"

namespace UnrealRanges{ namespace Utils{

	// ======================= Optional ============================

	template<typename ValueType>
	TUnrealRanges_Optional<ValueType> MakeOptional()
	{
		return UnrealRanges_Injection::MakeOptional(false, ValueType{ });
	}

	template<typename ValueType>
	TUnrealRanges_Optional<ValueType> MakeOptional(const ValueType& InValue)
	{
		return UnrealRanges_Injection::MakeOptional(true, InValue);
	}

	template<typename ValueType>
	TUnrealRanges_Optional<ValueType> MakeOptional(const bool InSet, const ValueType& InValue)
	{
		return UnrealRanges_Injection::MakeOptional(InSet, InValue);
	}

	template<typename ValueType>
	bool IsOptionalSet(const UnrealRanges_Injection::TUnrealRanges_Optional<ValueType>& InOptional)
	{
		return UnrealRanges_Injection::IsOptionalSet(InOptional);
	}

	template<typename ValueType>
	const ValueType& GetOptionalValue(const UnrealRanges_Injection::TUnrealRanges_Optional<ValueType>& InOptional)
	{
		return UnrealRanges_Injection::GetOptionalValue(InOptional);
	}

	template<typename ValueType>
	const ValueType& GetOptionalValueChecked(const UnrealRanges_Injection::TUnrealRanges_Optional<ValueType>& InOptional)
	{
		UnrealRanges_Injection::Check(UnrealRanges_Injection::IsOptionalSet(InOptional));
		return UnrealRanges_Injection::GetOptionalValue(InOptional);
	}

	// ========================== Random ============================

	inline int Rand(int Start, int End)
	{
		return UnrealRanges_Injection::Rand(Start, End);
	}

	// ========================== Check ============================

#	if UNREAL_RANGES__DEBUG //{

	inline void Check(const bool bCondition)
	{
		return UnrealRanges_Injection::Check(bCondition);
	}

	inline void Check(const bool bCondition, const char* Message)
	{
		return UnrealRanges_Injection::Check(bCondition, Message);
	}

#	else // } UNREAL_RANGES__DEBUG {

	inline void Check(const bool bCondition)
	{
	}

	inline void Check(const bool bCondition, const char* Message)
	{
	}

#	endif //} UNREAL_RANGES__DEBUG

}}//namespace UnrealRanges::Utils
