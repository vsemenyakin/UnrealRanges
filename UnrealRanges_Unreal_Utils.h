#pragma once

#include "UnrealRanges_Utils_Types.h"

//Unreal
#include "Misc/Optional.h"
#include "UObject/SoftObjectPtr.h"

namespace UnrealRanges{ namespace Utils{

	// SoftPtr
	//TODO example: Iterating icons

	template<ESafeness Safeness, typename ValueType>
	ValueType* ToPtr(const TOptional<TSoftObjectPtr<ValueType>>& Optional);

	template<typename ValueType>
	ValueType* ToPtr(const TOptional<TSoftObjectPtr<ValueType>>& Optional);

	// SoftPtr
	//TODO example: Iterating Game Mode classes

	template<ESafeness Safeness, typename ValueType>
	TSubclassOf<ValueType> ToPtr(const TOptional<TSoftClassPtr<ValueType>>& Optional);

	template<typename ValueType>
	TSubclassOf<ValueType> ToPtr(const TOptional<TSoftClassPtr<ValueType>>& Optional);

}} //namespace UnrealRanges::Utils

#include "Private/UnrealRanges_Impl_Unreal_Utils.inl"
