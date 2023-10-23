#pragma once

#include <type_traits> //for std::underlying_type_t<>

namespace UnrealRanges{ namespace Utils{

	// ========================== Logic ============================

	template<Logic::ELogicInvertion LogicInvertion>
	bool CheckBoolWithInvertionCondition(const bool bValue)
	{
		static_assert(
			LogicInvertion == Logic::NotInverted ||
			LogicInvertion == Logic::Not,
			"Invalid invertion setup");

		switch (LogicInvertion)
		{
			case Logic::NotInverted:   return bValue;
			case Logic::Not:           return !bValue;
			default:                   return false;
		}
	}

	template<Logic::ELogicInvertion LogicInvertion, typename FunctorType>
	auto MakeFunctorWithInvertionCondition_OneArg(FunctorType&& Functor)
	{
		return [FunctorForwarded = std::forward<FunctorType>(Functor)](auto&& Arg)
		{
			const bool FunctorResult = static_cast<bool>(FunctorForwarded(Arg));
			return CheckBoolWithInvertionCondition<LogicInvertion>(FunctorResult);
		};
	}

	// =================== Universal Begin/End =====================

	// See "UnrealRanges_Utils_Details.h" for "TemplateDetails::RangeBegin"

	template<typename RangeType>
	auto RangeBegin(const RangeType& Range)->decltype(TemplateDetails::RangeBegin::_(Range, 0))
	{
		return TemplateDetails::RangeBegin::_(Range, 0);
	}

	template<typename RangeType>
	auto RangeEnd(const RangeType& Range)->decltype(TemplateDetails::RangeEnd::_(Range, 0))
	{
		return TemplateDetails::RangeEnd::_(Range, 0);
	}

	// =================== Universal Begin/End =====================

	namespace TemplateDetails{
		namespace IsRange{

			template<typename RangeType>
			constexpr auto _(int)->decltype(
				UnrealRanges::Utils::RangeBegin(std::declval<RangeType>()),
				UnrealRanges::Utils::RangeEnd(std::declval<RangeType>()),
				true)
			{
				return true;
			}

			template<typename RangeType>
			constexpr bool _(...)
			{
				return false;
			}

		}
	}

	template<typename PossibleRangeType>
	constexpr bool IsRange()
	{
		return TemplateDetails::IsRange::_<PossibleRangeType>(0);
	}

	// ================= Universal dereference =====================

	//TODO: Implement "Dereference_Universal" and use here

	// =================== Enums iterating =========================

	template<typename EnumType>
	EnumType NextEnum(const EnumType Enum)
	{
		using EnumNumericValueType = std::underlying_type_t<EnumType>;
		EnumNumericValueType EnumNumericValue = static_cast<EnumNumericValueType>(Enum);
		return static_cast<EnumType>(EnumNumericValue + 1);
	}

	template<typename EnumType>
	EnumType PreviousEnum(const EnumType Enum)
	{
		using EnumNumericValueType = std::underlying_type_t<EnumType>;
		EnumNumericValueType EnumNumericValue = static_cast<EnumNumericValueType>(Enum);
		return static_cast<EnumType>(EnumNumericValue - 1);
	}

	// ==================== GetMemberValue =========================

	namespace TemplateDetails{
			
		//TODO: Move to separate file and make NOT Details function for common usage
		template<typename TargetType, typename MemberPointerType>
		auto GetMemberValue(TargetType&& Target, MemberPointerType MemberPointer)->
			decltype(Target->*MemberPointer)
		{
			return Target->*MemberPointer;
		}

		template<typename TargetType, typename MemberPointerType, typename ... MethodArgTypes>
		auto GetMemberValue(TargetType&& Target, MemberPointerType MemberPointer, MethodArgTypes&& ... MethodArgs)->
			decltype((Target->*MemberPointer)(Forward<MethodArgTypes>(MethodArgs) ...))
		{
			return (Target->*MemberPointer)(Forward<MethodArgTypes>(MethodArgs) ...);
		}

		//TODO: Move to separate file and make NOT Details function for common usage
		template<typename TargetType, typename MemberPointerType>
		auto GetMemberValue(TargetType&& Target, MemberPointerType MemberPointer)->decltype(Target.*MemberPointer)
		{
			return Target.*MemberPointer;
		}

		template<typename TargetType, typename MemberPointerType, typename ... MethodArgTypes>
		auto GetMemberValue(TargetType&& Target, MemberPointerType MemberPointer, MethodArgTypes&& ... MethodArgs)->
			decltype((Target.*MemberPointer)(Forward<MethodArgTypes>(MethodArgs) ...))
		{
			return (Target.*MemberPointer)(Forward<MethodArgTypes>(MethodArgs) ...);
		}
	
	}// namespace ::TemplateDetails
	
	//NB: Return reference to field if field access is performed !
	template<typename TargetType, typename MemberPointerType, typename ... MethodArgTypes>
	auto GetMemberValue(TargetType&& Target, MemberPointerType MemberPointer, MethodArgTypes&& ... MethodArgs)
	{
		return TemplateDetails::GetMemberValue(
			Forward<TargetType>(Target), MemberPointer, Forward<MethodArgTypes>(MethodArgs) ...);
	}

	// =================== SafeCall<...>(Target, MethodPtr, Args ...) =====================

	template <typename TargetType, typename ... MethodArgsTypes, typename ... ProvidedValueTypes>
	void SafeCall(TargetType* Target,
		void(TargetType::* MethodPtr)(MethodArgsTypes ...), ProvidedValueTypes&& ...  Args)
	{
		if (Target)
			(Target->*MethodPtr)(Forward<ProvidedValueTypes>(Args) ...);
	}

	template <typename TargetType, typename ... MethodArgsTypes, typename ... ProvidedValueTypes>
	void SafeCall(const TargetType* Target,
		void(TargetType::* MethodPtr)(MethodArgsTypes ...) const, ProvidedValueTypes&& ...  Args)
	{
		if (Target)
			(Target->*MethodPtr)(Forward<ProvidedValueTypes>(Args) ...);
	}

	template <typename TargetType, typename RetType, typename ... MethodArgsTypes, typename ... ProvidedValueTypes>
	TUnrealRanges_Optional<RetType> SafeCall(TargetType* Target,
		RetType(TargetType::* MethodPtr)(MethodArgsTypes ...), ProvidedValueTypes&& ...  Args)
	{
		return Target ?
			MakeOptional((Target->*MethodPtr)(Forward<ProvidedValueTypes>(Args) ...)) :
			MakeOptional<RetType>();
	}

	template <typename TargetType, typename RetType, typename ... MethodArgsTypes, typename ... ProvidedValueTypes>
	TUnrealRanges_Optional<RetType> SafeCall(const TargetType* Target,
		RetType(TargetType::* MethodPtr)(MethodArgsTypes ...) const, ProvidedValueTypes&& ...  Args)
	{
		return Target ?
			MakeOptional((Target->*MethodPtr)(Forward<ProvidedValueTypes>(Args) ...)) :
			MakeOptional<RetType>();
	}

	// ================================== Dummies =========================================

	template<typename Type>
	const Type& ConstDummy()
	{
		static const Type Value{ };
		return Value;
	}

	// ===================== SafeGet<...>(Target, FieldPointer) ===========================

	template <typename TargetType, typename FieldType>
	const FieldType& SafeGet(const TargetType* Target, FieldType TargetType::* FieldPointer)
	{
		return Target ? Target->*FieldPointer : ConstDummy<FieldType>();
	}

	// ================================== ToPtr() ========================================

	template<ESafeness Safeness, typename ValueType>
	ValueType* ToPtr(const TUnrealRanges_Optional<ValueType*>& Optional)
	{
		UnrealRanges::Utils::CheckSupportedSafeness<Safeness>();

		const bool bSet = UnrealRanges::Utils::IsOptionalSet(Optional);

		switch (Safeness)
		{
			case UnrealRanges::Utils::Safe:
				if (!bSet)
				{
					return nullptr;
				}

				break;

			case UnrealRanges::Utils::Fast:
				UnrealRanges::Utils::Check(bSet);
				break;
		}

		return UnrealRanges::Utils::GetOptionalValue(Optional);
	}

	template<typename ValueType>
	ValueType* ToPtr(const TUnrealRanges_Optional<ValueType*>& Optional)
	{
		return ToPtr<Safe, ValueType>(Optional);
	}

	template<typename ValueType>
	ValueType* ToPtr(const std::shared_ptr<ValueType>& SharedPtr)
	{
		return !!SharedPtr ? &*SharedPtr : nullptr;
	}

	template<typename ValueType>
	ValueType* ToPtr(const std::weak_ptr<ValueType>& WeakPtr)
	{
		return ToPtr(WeakPtr.lock());
	}

	template<typename ValueType>
	ValueType* ToPtr(const TUnrealRanges_Optional<std::shared_ptr<ValueType>>& SharedPtrOptional)
	{
		return UnrealRanges::Utils::IsOptionalSet(SharedPtrOptional) ?
			ToPtr(UnrealRanges::Utils::GetOptionalValue(SharedPtrOptional)) : nullptr;
	}

	template<typename ValueType>
	ValueType* ToPtr(const TUnrealRanges_Optional<std::weak_ptr<ValueType>>& WeakPtrOptional)
	{
		return UnrealRanges::Utils::IsOptionalSet(WeakPtrOptional) ?
			ToPtr(UnrealRanges::Utils::GetOptionalValue(WeakPtrOptional)) : nullptr;
	}

}}//namespace UnrealRanges::Utils
