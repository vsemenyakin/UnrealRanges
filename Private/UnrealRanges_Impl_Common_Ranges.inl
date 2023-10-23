#pragma once

//For:
// - "UnrealRanges::Utils::Logic::ELogicInvertion"
// - "UnrealRanges::Utils::CheckBoolWithInvertionCondition<>()"
// - "UnrealRanges::Utils::GetMemberValue()"
#include "../UnrealRanges_Utils.h"

#include <utility> //for "std::forward<...>"

namespace UnrealRanges{

	// ========| RangeFilteredBy<LogicInversion, ...>(MemberPointer, MethodArgs ...) |========

	// ------------------------------------- Common ------------------------------------------

	template<Utils::Logic::ELogicInvertion LogicInversion, typename FieldOrMethodPointer, typename ... MethodArgTypes>
	auto RangeFilteredBy(FieldOrMethodPointer MemberPointer, MethodArgTypes&& ... MethodArgs)
	{
		return FilteredRange([MemberPointer, MethodArgs ...](auto Value)
		{
			const bool bValueCheckResult = static_cast<bool>(GetMemberValue(Value, MemberPointer, MethodArgs ...));
			return Utils::CheckBoolWithInvertionCondition<LogicInversion>(bValueCheckResult);
		});
	}

	// ----------------------------- Defaulted "LogicInversion" ------------------------------

	template<typename FieldOrMethodPointer, typename ... MethodArgTypes>
	auto RangeFilteredBy(FieldOrMethodPointer MemberPointer, MethodArgTypes&& ... MethodArgs)
	{
		return RangeFilteredBy<Utils::Logic::NotInverted>(MemberPointer, std::forward<MethodArgTypes>(MethodArgs) ...);
	}

	// =================| RangeTransformedBy<...>(MemberPointer, MethodArgs) |================

	template<typename MemberPointerType, typename ... MethodArgTypes>
	auto RangeTransformedBy(MemberPointerType MemberPointer, MethodArgTypes&& ... MethodArgs)
	{
		return TransformedRange([MemberPointer, MethodArgs ...](auto Value)
		{
			return Utils::GetMemberValue(Value, MemberPointer, MethodArgs ...);
		});
	}

	// =======================| FilterExclude<...>(ElementToIgnore) |=========================

	template<typename ElementType>
	auto FilterExclude(ElementType ElementToIgnore)
	{
		return FilteredRange([ElementToIgnore](auto Value)
		{
			return (Value != ElementToIgnore);
		});
	}

} //namespace UnrealRanges
