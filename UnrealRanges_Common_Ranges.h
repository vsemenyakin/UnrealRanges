#pragma once

#include "UnrealRanges_Utils.h"//for UnrealRanges::Utils::Logic::ELogicInvertion

namespace UnrealRanges{


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


	// --------- RangeFilteredBy<LogicInversion, ...>(MemberPointer, MethodArgs ...) ---------
	//
	// IN:
	//    <1>? LogicInversion = "Logic::Not"
	//       Possible logic inversion used for filter condition
	//
	//    (1) MemberPointer
	//       Pointer to member of Parent Range Element
	//
	//    (2) MethodArgs ...
	//       Arguments that will be passed to method of Parent Range Element if
	//       method was passed by "MemberPointer" (1) argument
	//
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	//
	// Filter range using class member. Behaves like "RangeTransformedBy<...>(...)"
	// (you may find it's documentation by name), but:
	// 
	// - not transform type of Parent Range, just filter it's elements checking
	// "static_cast<bool>({Element}.*MemberPointer)" or
	// "static_cast<bool>(({Element}.*MemberPointer)(MethodArgs ...))"
	// depended on passed member pointer type.
	// 
	// - support optional first "LogicInversion" template argument.
	//  If "Logic::Not" is passed, result of checking is inverted 
	//  If this template argument is not passed ([Overloading #2] is called),
	//  "Logic::NotInverted" is passed "by default".
	// 
	// ~~~~ Examples ~~~~
	//
	//  //.[Common example types & variables]
	//
	//  class FClass
	//  {
	//  public:
	//  
	//     bool HasHitPoints(const bool bIncludeShields = true) const
	//     {
	//         return bIncludeShields ?
	//		       (HitPoints + Shields > 0) : (HitPoints > 0);
	//     }
	//  
	//     int32 HitPoints = 0;
	//     int32 Shields = 0;
	//  };
	//
	//  std::vector<FClass> Values{ { 1, 1 }, { 1, 0 }, { 0, 0 } };
	//  std::vector<FClass> Pointers{ &Values[0], &Values[1], &Values[2] };
	//  
	//  //.[Example #1]
	//  // !Attention: As for "RangeTransformedBy<...>(...)", you should pass even
	//  // defaulted arguments ("bIncludeShields" in next example).
	// 
	//  for (FClass* Alive : Pointers | RangeFilteredBy(&FClass::HasHitPoints, true))
	//  {
	//      // "Alive" during iterations will be equals to:
	//      //  "&Values[0]" and "&Values[1]"
	//      //
	//      // Elemenet equals to "&Values[2]" will be filtered out because it has
	//      // "HitPoints == 0" and "Shields == 0".
	//  }
	//  
	//  //.[Example #2]
	//  // Use casting  with inverted modifier.
	//  // Behaves like "!static_cast<bool>({Element}.Shields)" check
	// 
	//  using namespace Logic;
	//  for (FClass* WithoutShields : Pointers | RangeFilteredBy<Not>(&FClass::Shields))
	//  {
	//      // "Alive" during iterations will be equals to:
	//      //  "&Values[1]" and "&Values[2]"
	//      //
	//      // Elemenet equals to "&Values[0]" will be filtered out because it has
	//      // "Shields != 0" and so "!static_cast<bool>(Shields) == false".
	//  }
	//
	// [Overloading #1]
	template<Utils::Logic::ELogicInvertion LogicInversion, typename FieldOrMethodPointer, typename ... MethodArgTypes>
	auto RangeFilteredBy(FieldOrMethodPointer MemberPointer, MethodArgTypes&& ... MethodArgs);

	// [Overloading #2]
	template<typename FieldOrMethodPointer, typename ... MethodArgTypes>
	auto RangeFilteredBy(FieldOrMethodPointer MemberPointer, MethodArgTypes&& ... MethodArgs);

	// --------- RangeTransformedBy<...>(MemberPointer, MethodArgs) ---------
	//
	// IN:
	//    (1) MemberPointer
	//       Pointer to member of Parent Range Element
	//
	//    (2) MethodArgs ...
	//       Arguments that will be passed to method of Parent Range Element if
	//       method was passed by "MemberPointer" (1) argument
	//
	// OUT:
	//    {ret} - Range-Part that performs actions described below...
	//
	// Form transformed range by "UnrealRanges::Utils::GetMemberValue<>()" that
	// means...
	// 
	// 1. <If "MemberPointer" is pointer to _field_ of Parent Range elements>
	//  ...returning value of field pointed by "MemberPointer" for each element
	//    (aka "{Element}.*MemberPointer")
	//  
	//
	// 2. <If "MemberPointer" is pointer to _method_ of Parent Range elements>
	//  ...returning result of call "MemberPointer(MethodArgs ...)"
	//    (aka "({Element}.*MemberPointer)(MethodArgs ...)")
	// 
	// Values of Parent Range may be both _values_ and _pointers_. If values are
	// pointers - Members are accessed by "->" operator.
	// 
	// ! NB1: When using access by method call (2) you should pass _all arguments_,
	//  even args that was defaulted in method declaration.
	// 
	// ! NB2: No any _pointers_ validation is performed! If you want be sure that
	//  pointer elements are correct, use "RangeTransformedBy<...>(...)" with
	//  "ValidRange()" range filter:
	// "... | ValidRange() | RangeTransformedBy<...>(...) | ..."
	// 
	// ~~~~ Examples ~~~~
	//
	//  //.[Common example types & variables]
	// 
	//  class FClass
	//  {
	//  public:
	//  
	//     int32 GetField(const bool bClamped = false) const
	//     {
	//         return bClamped ? FMath::Max(Field, 0) : Field;
	//     }
	//  
	//     int32 Field = 0;
	//  };
	//  
	//  std::vector<FClass> Values{ { -1 }, { 0 }, { 1 } };
	//  std::vector<FClass*> Pointers{ &Values[0], &Values[1], &Values[2] };
	//
	//  //.[Example #1]
	//  // Elements of transformed range are "int32" (as type of "Field" member)
	// 
	//  for (int32 Element : Values | RangeTransformedBy(&FClass::Field))
	//  {
	//      // "Element" during iterations will be equals to:
	//      //  "-1", "0", "1"
	//  }
	// 
	//  //.[Example #2]
	//  // !Attention: All method arguments should be passed, even defaulted
	//  // ("bClamped" in next example).
	//  
	//  for (int32 Element : Values | RangeTransformedBy(&FClass::GetField, false))
	//  {
	//      // "Element" during iterations will be equals to:
	//      //  "-1", "0", "1"
	//  }
	// 
	//  //.[Example #3]
	//  // Example of iteration over Parent Range filled by pointers
	// 
	//  for (int32 Element : Pointers | RangeTransformedBy(&FClass::GetField, true))
	//  {
	//      // "Element" during iterations will be equals to:
	//      //  "0", "0", "1"
	//      // First element is "0" because "true" is passed as "Clamped" argument
	//  }
	//
	template<typename MemberPointerType, typename ... MethodArgTypes>
	auto RangeTransformedBy(MemberPointerType MemberPointer, MethodArgTypes&& ... MethodArgs);

	// --------- FilterExclude<...>(ElementToIgnore) ---------
	//
	// IN:
	//    (1) ElementToIgnore
	//       Value that will used to exclude Parent Range
	//       elements by comparation ("==") call.
	//
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	//
	// Form filtered range without elements that are equals to
	// "ElementToIgnore" argument by calling "==" operator
	//
	// ~~~~ Examples ~~~~
	//
	//  //.[Common example types & variables]
	// 
	//  std::vector<std::string> Names{ "One", "Two", "Three" };
	//
	//  //.[Example #1]
	//  // Iterating over all values without std::string{ "Two" }
	// 
	//  static const std::string NameToFilterOut{ "Two" };
	// 
	//  for (const std::string& Name : Names | FilterExclude(NameToFilterOut))
	//  {
	//      // "Name" during iterations will be equals to:
	//      //  "std::string{ "One" }", "std::string{ "Three" }"
	//      //
	//      // Element equals to "std::string{ "Two" }" will be
	//      // filtered out because it equals to "NameToFilterOut"
	//  }
	//
	//  //.[Example #2]
	//  // Iterating over all values without std::string{ "Two" }
	//
	//  static const char* NameStrToFilterOut{ "Two" };
	//
	//  for (const std::string& Name : Names | FilterExclude(NameStrToFilterOut))
	//  {
	//      // "Name" during iterations will be equals to:
	//      //  "std::string{ "One" }", "std::string{ "Three" }"
	//      //
	//      // Element equals to "std::string{ "Two" }" will be filtered out
	//      // because "std::string{ "Two" } == NameStrToFilterOut" is "true"
	//  }
	//
	template<typename ElementType>
	auto FilterExclude(ElementType ElementToIgnore);

} //namespace UnrealRanges

#include "Private/UnrealRanges_Impl_Common_Ranges.inl"
