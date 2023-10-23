#pragma once

#include "Private/UnrealRanges_Impl_Utils.h"

#ifndef UnrealRanges_Injection
//TODO: Pass here Default optional Injection
//#	define UnrealRanges_Injection__Optional UnrealRanges::UnrealInjection
#endif

namespace UnrealRanges{ namespace Utils{

	// ======================= Optional ============================

	template<typename ValueType>
	using TUnrealRanges_Optional = UnrealRanges_Injection::TUnrealRanges_Optional<ValueType>;

	template<typename ValueType>
	TUnrealRanges_Optional<ValueType> MakeOptional(const bool InSet, const ValueType& InValue);

	template<typename ValueType>
	TUnrealRanges_Optional<ValueType> MakeOptional(const bool InSet, ValueType&& InValue);

	template<typename ValueType>
	bool IsOptionalSet(const TUnrealRanges_Optional<ValueType>& InOptional);

	template<typename ValueType>
	const ValueType& GetOptionalValue(const TUnrealRanges_Optional<ValueType>& InOptional);

}}//namespace UnrealRanges::Utils

#include "Private/UnrealRanges_Impl_Utils.inl"
