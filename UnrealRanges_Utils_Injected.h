#pragma once

#include "UnrealRanges_Common_Injection.h"

namespace UnrealRanges{ namespace Utils{

	// ========================== Optional ============================

	//TODO: Add documentation!

	template<typename ValueType>
	using TUnrealRanges_Optional = UnrealRanges_Injection::TUnrealRanges_Optional<ValueType>;

	template<typename ValueType>
	TUnrealRanges_Optional<ValueType> MakeOptional();

	template<typename ValueType>
	TUnrealRanges_Optional<ValueType> MakeOptional(const ValueType& InValue);

	template<typename ValueType>
	TUnrealRanges_Optional<ValueType> MakeOptional(const bool InSet, const ValueType& InValue);

	template<typename ValueType>
	bool IsOptionalSet(const TUnrealRanges_Optional<ValueType>& InOptional);

	template<typename ValueType>
	const ValueType& GetOptionalValue(const TUnrealRanges_Optional<ValueType>& InOptional);

	template<typename ValueType>
	const ValueType& GetOptionalValueChecked(const TUnrealRanges_Optional<ValueType>& InOptional);

	// ========================== Random ============================

	int Rand(int Start, int End);

	// ========================== Random ============================

	void Check(const bool bCondition);
	void Check(const bool bCondition, const char* Message);

}}//namespace UnrealRanges::Utils

#include "Private/UnrealRanges_Impl_Utils_Injected.inl"
