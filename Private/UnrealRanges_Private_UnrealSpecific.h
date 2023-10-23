#pragma once

#include "UObject/WeakObjectPtrTemplates.h"//for "IsValid()"
#include "EngineUtils.h"//for "TActorRange<>"

namespace UnrealRanges{ namespace Private{ namespace UnrealSpecific{

	// =============================| IsValid_Universal<...>() |===============================

	namespace TemplateDetails{

		// Function call variant is prefared
		template<typename T>
		auto IsValid_Universal(T Value, int32, int32)->decltype(IsValid(Value))
		{
			return IsValid(Value);
		}

		template<typename T>
		auto IsValid_Universal(T Value, int32, ...)->decltype(Value.IsValid())
		{
			return Value.IsValid();
		}

		template<typename T>
		auto IsValid_Universal(T Value, ...)->decltype(!!Value)
		{
			return !!Value;
		}

	} //namespace ::TemplateDetails

	template<typename T>
	bool IsValid_Universal(T Value)
	{
		return TemplateDetails::IsValid_Universal(Value, 0, 0);
	}

	// ==================| MACRO support: Ranges_StaticCheckRangeType() |=====================

	namespace TemplateDetails
	{
		namespace IsActorRange
		{
			template<typename NotActorRange> struct _ { constexpr static bool __ = false; };
			template<typename AnyType>       struct _<TActorRange<AnyType>> { constexpr static bool __ = true; };
		}
	}

	template<typename MaybeActorRange> constexpr bool bIsActorRange = TemplateDetails::IsActorRange::_<MaybeActorRange>::__;

	template<typename TypeToTest>
	struct StaticCheckRangeType
	{
		static_assert(!bIsActorRange<TypeToTest>,
			"Don't use \"TActorRange<Type>\" with Ranges!\n"
			"It\'s better to use \"ActorsRange<...>(...)\" in such case");
	};

}}} //namespace UnrealRanges::Private::UnrealSpecific

#define Ranges_StaticCheckRangeType(Type)\
	constexpr static UnrealRanges::Private::UnrealSpecific::StaticCheckRangeType<Type> _{ };
