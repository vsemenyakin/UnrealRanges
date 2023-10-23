#pragma once

// ---------------------- Optional -------------------------
//
// "Optional" injection should containe functionality needed
// for passing by value results that may be "not filled"
// For example - results of Find requests in Ranges or,
// generaly, on returning particular value from Range
// (that may be empty).
//
// Optional Injection should satisfy next conditions:
//
// 1. Namespace naming: "Optional"
//  Should be placed in namespace (or struct) named "Optional"
//  inside UnrealRanges Injection
//
// 2. Should have class: "template<typename ValueType> Impl"
//  Should have class template (or type alias template) that
//  takes template argument for storing optional value type
//  and implements logic for storing optional value
// 
// 3. "template<typename ValueType> Impl" API:
//
//  ______________________________________
//   Constructors
//     ~ [1] Function in namespace "Optional"
//      "template<typename ValueType> Impl<ValueType> MakeOptional(
//          bool InSet, const ValueType& InValue)"
//     ~ [2] Optional, function in namespace "Optional"
//      "template<typename ValueType> Impl<ValueType> MakeOptional(
//          bool InSet, ValueType&& InValue)"
//  - - - - - - - - - - - - - - - - - - - 
//  Functions [1,2] are farbic functions for Optional that may be
//  filled or not depending on flag
//  "InValue" argument - value to fill Optional
//  "InSet" argument - filling status for Optional
//  Function [2] (value-moving) may be not implemented
//
//  ______________________________________
//   Fill status checking
//     ~ [1] Function in namespace "Optional"
//      "template<typename ValueType> bool IsOptionalSet(
//          const Impl<ValueType>& InOptional)"
//  - - - - - - - - - - - - - - - - - - - 
//  Function [1] return filling status of the Optional. Return
//   "true" if optional is filled and "false" otherwise
// 
//  ______________________________________
//   Value accessing
//     ~ [1] Function in namespace "Optional"
//      "const ValueType& GetOptionalValue(const Impl<ValueType>& InOptional)"
//  - - - - - - - - - - - - - - - - - - - 
//  Function [1] return the value of the optional

#ifndef UnrealRanges_Injection

#include <cstdlib> //for "std::rand()"
#include <stdexcept> //for "std::runtime_error"
#include <type_traits> //for "std::decay_t<>"

namespace UnrealRanges{ namespace Injection{ namespace Default{

	// ================ Optional ==================

	template<typename ValueType>
	class TUnrealRanges_Optional
	{
		template<typename ValueType_>
		friend TUnrealRanges_Optional<std::decay_t<ValueType_>> MakeOptional(const bool InSet, ValueType_&& InValue);

		template<typename ValueType_>
		friend bool IsOptionalSet(const TUnrealRanges_Optional<ValueType_>& InOptional);

		template<typename ValueType_>
		friend const ValueType_& GetOptionalValue(const TUnrealRanges_Optional<ValueType_>& InOptional);

	public:
		bool bSet;
		ValueType Value;
	};

	template<typename ValueType>
	TUnrealRanges_Optional<std::decay_t<ValueType>> MakeOptional(const bool InSet, ValueType&& InValue)
	{
		return { InSet, std::forward<ValueType>(InValue) };
	}

	template<typename ValueType>
	bool IsOptionalSet(const TUnrealRanges_Optional<ValueType>& InOptional)
	{
		return InOptional.bSet;
	}

	template<typename ValueType>
	const ValueType& GetOptionalValue(const TUnrealRanges_Optional<ValueType>& InOptional)
	{
		return InOptional.Value;
	}

	// =================== Random =====================

	inline int Rand(int Start, int End)
	{
		const int RangeForDivisionRemainder = End - Start + 1;
		return RangeForDivisionRemainder > 1 ?
			(Start + std::rand() % RangeForDivisionRemainder) :
			Start;
	}

	// =================== Check =====================

	inline void Check(const bool bCondition)
	{
		if (!bCondition)
			throw;
	}

	inline void Check(const bool bCondition, const char* Message)
	{
		if (!bCondition)
			throw std::runtime_error{ Message };
	}

}}} //UnrealRanges::Injection::Default

#define UnrealRanges_Injection UnrealRanges::Injection::Default // <<< Injection define <<<

#endif
