#pragma once

#include "UnrealRanges_Utils_Injected.h"
#include "Private/UnrealRanges_Utils_Details.h"

#include "UnrealRanges_Utils_Types.h"

namespace std
{
	template<typename Type> class weak_ptr;
	template<typename Type> class shared_ptr;
}

namespace UnrealRanges{ namespace Utils{


	// ****************** !!! NB !!! ******************
	// Current Range implementation does not support iteration
	// over mutable values. Changing of elements got during
	// iterations from Ranges may cause Undefined Behaviour
	// 
	// Mutating of Range elements will be supported in next
	// Range implementation version
	// ****************** !!! NB !!! ******************


	// ****************** Useful tip ******************
	// If a lot of documantion in code is anoying for you
	// and you are using Visual Studio, you may use next
	// default shortcuts to operate with it:
	//
	// "Ctrl + M + O" = collapsing of all regions in file
	// "Ctrl + M + L" = expanding of all regions in file
	// 
	// ****************** Useful tip ******************

	// ---- CheckBoolWithInvertionCondition<LogicInvertion>(bValue) ----
	//
	// IN:
	//    <1> LogicInvertion
	//       
	// 
	// OUT:
	//    {ret}
	//         Value that is equals to...
	//         ..."bValue" if "LogicInvertion <=> ELogicInvertion::NotInverted"
	//         ..."!bValue" if "LogicInvertion <=> ELogicInvertion::Not"
	//         Cause "static_assert" if "LogicInvertion" has another value
	//
	// This function is useful for Algorithms that should support logicaly
	// inverted conditions
	//
	// ~~~~ Example ~~~~ 
	//
	//  struct FUnit
	//  {
	//     // ...
	//  
	//     bool IsDead() const;
	//  
	//     // ...
	//  };
	// 
	//  template<Logic::ELogicInvertion LogicInvertion>
	//  auto GetDeadUnitsRange() 
	//  {
	//      auto FilterConditionLambda = [](const FUnit& Unit)
	//      {
	//          //          Here function is used
	//          //     VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
	//          return CheckBoolWithInvertionCondition<LogicInvertion>(Unit.IsDead());
	//      };
	// 
	//      return GetUnitsRange() | FilteredRange(FilterConditionLambda);    
	//  }
	// 
	//  // With "using" range looks descritable,
	//  // rather like human sentance:
	//  // "Get ... <-Not ... DeadUnitsRange"
	//  
	//  using namespace UnrealRanges::Utils::Logic;
	//  
	//  auto AliveUnits = GetDeadUnitsRange<Not>(); 
	// 
	template<Logic::ELogicInvertion LogicInvertion>
	bool CheckBoolWithInvertionCondition(const bool bValue);

	// ---- MakeFunctorWithInvertionCondition_OneArg<LogicInvertion>(Functor) ----
	//
	// IN:
	//    <1> LogicInvertion
	//       Inversion setup for function
	//
	//    (1) Functor
	//       Functor to be inverted in returned from function Lambda
	// 
	// OUT:
	//    {ret}
	//         Lambda that returns Lambda that returns value equals to...
	//         ..."Functor(Arg1)" if "LogicInvertion <=> ELogicInvertion::NotInverted"
	//         ..."!Functor(Arg1)" if "LogicInvertion <=> ELogicInvertion::Not"
	//         Cause "static_assert" if "LogicInvertion" has another value       
	//
	// This function is useful for Algorithms that should support logicaly
	// inverted conditions
	//
	// ~~~~ Example ~~~~ 
	//
	//  struct FUnit
	//  {
	//     // ...
	//  
	//     bool IsDead() const;
	//  
	//     // ...
	//  };
	// 
	//  template<Logic::ELogicInvertion LogicInvertion, typename PredicateType>
	//  auto UnitsByCondition(PredicateType Predicate) 
	//  {
	//      auto InvertedPredicate =
	//          MakeFunctorWithInvertionCondition_OneArg<LogicInvertion>(Predicate);
	//      //  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	//      //          Here function is used
	// 
	//      return GetUnitsRange() | FilteredRange(InvertedPredicate);    
	//  }
	//  
	//  auto IsUnitDead = [](const FUnit& Unit)
	//  {
	//      return Unit.IsDead();
	//  };
	// 
	//  // With "using" range looks descritable,
	//  // rather like human sentance:
	//  // "UnitsByConditionRange that ... Not ... Is ... Dead"
	//  
	//  using namespace UnrealRanges::Utils::Logic;
	// 
	//  auto AliveUnits = UnitsByConditionRange<Not>(IsUnitDead); 
	//
	template<Logic::ELogicInvertion LogicInvertion, typename FunctorType>
	auto MakeFunctorWithInvertionCondition_OneArg(FunctorType&& Functor);

	// =============================================================
	// =================== Universal Begin/End =====================
	// =============================================================

	// --------- RangeBegin<>(Range) and RangeEnd<>(Range) ---------
	//
	// IN:
	//    (1) Range
	//       Range to get appropriate iterator from
	// 
	// OUT:
	//    {ret}
	//         For [Funcion #1] - Iterator for first element
	//          of passed range
	//         For [Funcion #2] - Iterator for element next
	//          to last of passed range
	//
	// Universal functions for accessing ranges limit iterators.
	// Allow to access limits for all kinds of ranges, including:
	// - C++ arrays
	// - Collections that has both "begin/end()" method and
	//   "begin/end(collection)" free functions (like "std::vector")
	//   prefering method variant over function variant
	// 
	// ~~~~ Example ~~~~ 
	// 
	//  //.[Example #1]
	//  // Iterating over C++ array
	// 
	//  unsigned int NumbersArray[]{ 1, 2, 3, 4 };
	//  
	//  for (auto Current = RangeBegin(NumbersArray);
	//      Current != RangeEnd(NumbersArray);
	//      ++Current)
	//  {
	//      // "Element" during iterations will be equals to:
	//      //  "1", "2", "3", "4"
	//  }
	//
	//  //.[Example #2]
	//  // Iterating over "std::vector"
	//
	//  std::vector<unsigned int> NumbersVector{ 1, 2, 3, 4 };
	//  
	//  for (auto Current = RangeBegin(NumbersVector);
	//      Current != RangeEnd(NumbersVector);
	//      ++Current)
	//  {
	//      // "Element" during iterations will be equals to:
	//      //  "1", "2", "3", "4"
	//  }
	// 
	// [Function #1]
	template<typename RangeType>
	auto RangeBegin(const RangeType& Range)->decltype(TemplateDetails::RangeBegin::_(Range, 0));

	// [Function #2]
	template<typename RangeType>
	auto RangeEnd(const RangeType& Range)->decltype(TemplateDetails::RangeEnd::_(Range, 0));

	// =============================================================
	// ================= Universal dereference =====================
	// =============================================================

	//TODO: Implement "Dereference_Universal" and use here

	// =============================================================
	// =================== Enums iterating =========================
	// =============================================================

	// --------- NextEnum<...>(Enum) ---------
	//
	// IN:
	//    (1) Enum
	//       Enum value
	// 
	// OUT:
	//    {ret}
	//         Next enum value
	// 
	// Default implemetnation of Enum values iteration.
	// Uses enum numeric value increment update logic.
	//
	// ! NB: If you need to support specific "NextEnum(...)"
	// logic for your enum - write overloading of function
	// with next signature:
	// 
	//  template<>
	//  YourEnumType NextEnum<YourEnumType>(const YourEnumType Enum);
	//
	// ~~~~ Example ~~~~
	//
	//  // Several small examples of getting "next Enum"
	// 
	//  enum class EEnum : uint8
	//  {
	//     One,
	//     Two,
	//     Three
	//  };
	//  
	//  const EEnum Test1 = NextEnum(EEnum::One);
	//  // "Test1" is equals to "EEnum::Two"
	//  
	//  const EEnum Test2 = NextEnum(EEnum::Two);
	//  // "Test2" is equals to "EEnum::Three"
	//  
	//  const EEnum Test3 = NextEnum(EEnum::Three);
	//  // "Test3" has invalid enum value with
	//  // numberic value "EEnum::Three + 1"
	//  
	template<typename EnumType>
	EnumType NextEnum(const EnumType Enum);

	// --------- PreviousEnum<...>(Enum) ---------
	// 
	// Same as NextEnum but iterates an enum in reverse order
	//
	template<typename EnumType>
	EnumType PreviousEnum(const EnumType Enum);

	// ==========================================================
	// =================== Flexible C++ =========================
	// ==========================================================

	// --------- GetMemberValue<...>(Target, Getter, MethodArgs ...) ---------
	//
	// IN:
	//    (1) Target
	//       Pointer to object for witch the method should be called
	//
	//    (2) MemberPointer
	//       Pointer to member of Parent Range Element
	//
	//    (3) MethodArgs ...
	//       Arguments that will be passed to method of Parent Range Element if
	//       method was passed by "MemberPointer" (1) argument
	// 
	// OUT:
	//    {ret}
	//       Return value that described below...
	//
	//  If MemberPointer (2) argument is Field Pointer - return field value
	//  If MemberPointer (2) argument is Method Pointer - return result of
	//  method call with arguments pack (3) passed as arguments
	//
	// 	// ! NB: When using access by method call (2) you should pass _all arguments_,
	//  even args that was defaulted in method declaration.
	//
	// ~~~~ Examples ~~~~
	//
	//  class FClass
	//  {
	//  public:
	//  
	//     int32 GetField(const bool bClamped = false) const
	//     {
	//         return bClamped ? std::max(Field, 0) : Field;
	//     }
	//  
	//     int32 Field = 0;
	//  };
	//
	//  const FClass Object{ -1 };
	//
	//  const int32 Value1 = GetMemberValue(Object, MemberPointer);
	//  // "Value1" is equals to "-1"
	//
	//  // const int32 Value2 = GetMemberValue(Object, GetField, false);
	//  // Line above will not compile - all arguments should be passed to method,
	//  // even defaulted
	//
	//  const int32 Value2 = GetMemberValue(Object, GetField, false);
	//  const int32 Value3 = GetMemberValue(Object, GetField, true);
	//  // "Value2" is equals to "-1" because "false" was passed to clamping setup
	//  // "Value3" is equals to "0" because "true" was passed to clamping setup
	//
	template<typename TargetType, typename MemberPointerType, typename ... MethodArgTypes>
	auto GetMemberValue(TargetType&& Target, MemberPointerType MemberPointer, MethodArgTypes&& ... MethodArgs);

	// --------- SafeCall<...>(Target, MethodPtr) ---------
	//
	// IN:
	//    (1) Target
	//       Pointer to object for witch the method should be called
	//    (2) MethodPtr
	//       Pointer to method that should be called if passed object
	//       is not null
	// OUT:
	//    {ret}
	//       For overloading [1] and [2] - nothing
	//       For overloading [3] and [4] - wrapped to optional result
	//       of calling method or empty optional typed as value returned
	//       by "MethodPtr" (2) argument
	//         
	// Calls method "MethodPtr" for "Target" if the "Target" is not null
	//
	// ~~~~ Examples ~~~~
	// 
	// struct Unit
	// {
	//    void Shoot(int BulletsNumToShoot);
	//    int GetId() const;
	// };
	// 
	// Unit* NullPtr = nullptr;
	// const auto* ConstPtr = new Unit();
	// auto* Ptr = new Unit();
	//
	// SafeCall(NullPtr, &Unit::Shoot, 1);
	// // On line above no any method will be called because "NullPtr" is not valid
	//
	// // SafeCall(ConstPtr, &Unit::Shoot, 1);
	// // Line above will not compile: cannot call non-const method fo const ptr
	//
	// SafeCall(Ptr, &Unit::Shoot, 1);
	// // On line above method call "Ptr->Shoot(1)" will be performed
	// 
	// const TOptional<int> Id1 = SafeCall(NullPtr, &Unit::GetId);
	// const TOptional<int> Id2 = SafeCall(ConstPtr, &Unit::GetId);
	// const TOptional<int> Id3 = SafeCall(Ptr, &Unit::GetId);
	// 
	// // "Id1" optional - will be empty because "nullptr" Target is passed
	// // "Id2" optional - will be filled by value returned from call "ConstPtr->GetId()"
	// // "Id3" optional - will be filled by value returned from call "Ptr->GetId()" 
	// 
	// [Overloading #1]
	template <typename TargetType, typename ... MethodArgsTypes, typename ... ProvidedValueTypes>
	void SafeCall(TargetType* Target,
		void(TargetType::* MethodPtr)(MethodArgsTypes ...), ProvidedValueTypes&& ...  Args);

	// [Overloading #2]
	template <typename TargetType, typename ... MethodArgsTypes, typename ... ProvidedValueTypes>
	void SafeCall(const TargetType* Target,
		void(TargetType::* MethodPtr)(MethodArgsTypes ...) const, ProvidedValueTypes&& ...  Args);

	// [Overloading #3]
	template <typename TargetType, typename RetType, typename ... MethodArgsTypes, typename ... ProvidedValueTypes>
	TUnrealRanges_Optional<RetType> SafeCall(TargetType* Target,
		RetType(TargetType::* MethodPtr)(MethodArgsTypes ...), ProvidedValueTypes&& ...  Args);

	// [Overloading #4]
	template <typename TargetType, typename RetType, typename ... MethodArgsTypes, typename ... ProvidedValueTypes>
	TUnrealRanges_Optional<RetType> SafeCall(const TargetType* Target,
		RetType(TargetType::* MethodPtr)(MethodArgsTypes ...) const, ProvidedValueTypes&& ...  Args);

	// --------- SafeGet<...>(Target, FieldPointer) ---------
	//
	// IN:
	//    (1) Target
	//       Pointer to object for witch the field should be get
	//    (2) FieldPointer
	//       Pointer to field of "Target" that should be get 
	// 
	// OUT:
	//    {ret}
	//       Return value that described below...
	//         
	// Returns value of "FieldPointer" field of "Target" if "Target"
	// is not "nullptr" or, otherwise,
	// "ConstDummy<>()" value with type same as "FieldPointer" field
	//
	// ~~~~ Examples ~~~~
	//
	//  struct FConfig
	//  {
	//     unsigned int Param{ 1 };
	//  };
	// 
	//  FConfig* NullConfig = nullptr;
	//  
	//  const unsigned int ParamValue = SafeGet(NullConfig, FConfig::Param);
	//  // "ParamValue" is equals to "1"
	//  // This is because passed "NullConfig" passed as "Target" argument
	//  // is equals to "nullptr", so "Dummy<FConfig>" is taken as target
	//  // which is default-constructed object with "Param" field equals to "1"
	//
	// 	FConfig NotNullConfig{ 42 };
	// 
	//  const unsigned int ParamValue = SafeGet(&NotNullConfig, FConfig::Param); 
	//  // "ParamValue" is equals to "42"
	//  // This is because "&NotNullConfig" is not "nullptr", so field of value
	//  // is returned
	//
	template <typename TargetType, typename FieldType>
	const FieldType& SafeGet(const TargetType* Target, FieldType TargetType::* FieldPointer);

	// --------- ConstDummy<Type>() ---------
	// 
	// OUT:
	//    {ret}
	//       Return value that described below...
	//         
	// Returns value assumed to fill "default reference" for number of functions
	// (for example, for "SafeGet()"). By default, it returns default-constructed
	// static object, but specialization for another behaviour may be created
	// 
	// ! NB: Value is assumed to be immutable, so it is shared between all threads
	//
	// ~~~~ Examples ~~~~
	//
	//  stuct FSubConfig
	//  {
	//      unsigned int Param{ 1 };
	//      // ... and 100 other fields
	//  };
	// 
	//  struct FConfig
	//  {
	//      FSubConfig SubConfig;
	//  };
	//
	//  // Next function is usable for gettings FConfig field value in safe way
	//  // with saving return-by-reference signature of config accessor
	//
	//  const FSubConfig& GetSubconfig()
	//  {
	//      const FConfig* LoadedConfig = LoadConfig();
	//      if (!LoadedConfig)
	//      {
	//          // Print an error
	// 
	//          return ConstDummy<FSubConfig>();
	//      }
	// 
	//      return LoadedConfig->SubConfig;
	//  }
	//
	template<typename Type>
	const Type& ConstDummy();

	// =============================================================================
	//TODO: Create documentation for this methods

	template<ESafeness Safeness, typename ValueType>
	ValueType* ToPtr(const TUnrealRanges_Optional<ValueType*>& Optional);

	template<typename ValueType>
	ValueType* ToPtr(const TUnrealRanges_Optional<ValueType*>& Optional);

	template<typename ValueType>
	ValueType* ToPtr(const std::shared_ptr<ValueType>& Optional);

	template<typename ValueType>
	ValueType* ToPtr(const std::weak_ptr<ValueType>& Optional);

	template<typename ValueType>
	ValueType* ToPtr(const TUnrealRanges_Optional<std::shared_ptr<ValueType>>& SharedPtrOptional);

	template<typename ValueType>
	ValueType* ToPtr(const TUnrealRanges_Optional<std::weak_ptr<ValueType>>& WeakPtrOptional);

}}//namespace UnrealRanges::Utils

#include "Private/UnrealRanges_Impl_Utils.inl"
