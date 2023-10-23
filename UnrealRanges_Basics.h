#pragma once

#include "UnrealRanges_Utils.h"
#include "Private/UnrealRanges_Private_Utils.h"//for "operator|" SFINAE

//Next includes group - for Pipe Operator trait
#include <utility>//for std::forward<>

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


	// --------- FilteredRange<LogicInversion, ...>(PassOnTrueFilter) ---------
	//
	// IN:
	//    <1> LogicInversion
	//       Logic invertion setup for provided condition
	// 
	//    (1) PassOnTrueFilter
	//       Functor that takes Parent Range elements and
	//       return "true" if checking for it should be passed
	//       (using "LogicInversion" for setup)
	// 
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	// 
	// If "LogicInversion == Utils::Logic::NotInverted" filter range by
	// "PassOnTrueFilter({Element}) == true" checking for each element
	//
	// If "LogicInversion == Utils::Logic::Not" filter range by
	// "PassOnTrueFilter({Element}) == false" checking for each element 
	// 
	// ~~~~ Example ~~~~
	//
	//  //.[Common example types & variables]
	// 
	//  struct FTimerData
	//  {
	//		const bool IsExpired() const
	//      {
	//          return (LeftTime <= 0.f);
	//      }
	// 
	//      float LeftTime = 0.f;
	//      std::function<void()> Callback;
	//  };
	// 
	//  std::vector<FTimerData> TimerDatas
	//  {
	//      {  -0.1f ,  [](){ /* Some Callback #1 logic */ } },
	//      {  0.3f  ,  [](){ /* Some Callback #2 logic */ } },
	//      {  0.0f  ,  [](){ /* Some Callback #3 logic */ } }
	//  };
	// 
	//  //.[Example #1]
	//  // Calling callbacks of expired timers using [Overloading #2] with
	//  // "defaulted" "LogicInversion" argument
	//
	//  auto IsTimerExpired = [](const FTimerData& TimerData)
	//  {
	//      return TimerData.IsExpired();
	//  });
	// 
	//  for (const FTimerData& Timer : TimerDatas | FilteredRange(IsTimerExpired))
	//  {
	//      TimerDatas.Callback();
	//      // Callback is called for "TimerDatas[0]" and "TimerDatas[2]"
	//      // "TimerDatas[1]" is expired
	//  }
	// 
	//  //.[Example #2]
	//  // Iterating over not expired timers [Overloading #1]
	//
	//  using namespace Logic;
	// 
	//  for (const FTimerData& Timer : TimerDatas | FilteredRange<Not>(IsTimerExpired))
	//  {
	//      // "Timer" during only-one iteration will be equals to "TimerDatas[1]"
	//      // Other timers will be filtered out because they are expired
	//  }
	// 
	// [Overloading #1]
	template<Utils::Logic::ELogicInvertion LogicInversion, typename PassOnTrueFilterPredicateType>
	auto FilteredRange(PassOnTrueFilterPredicateType&& PassOnTrueFilter);

	// [Overloading #2]
	template<typename PassOnTrueFilterPredicateType>
	auto FilteredRange(PassOnTrueFilterPredicateType&& PassOnTrueFilter);

	// --------- TransformedRange<...>(TransformFuncType) ---------
	//
	// IN:
	//    (1) TransformFuncType
	//      Functor that provide transforming logic
	// 
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	// 
	// Form transformed Range from values returned by "TransformFunc"
	// 
	// ~~~~ Example ~~~~
	//
	//  // Transforming numbers to words that descibes this numbers
	// 
	//  std::vector<int> Values{ 1, 2, 3, 4 };
	//  
	//  const char* ToString(int Value)
	//  {
	//     switch (Value)
	//     {
	//         case 1:   return "One";
	//         case 2:   return "Two";
	//         case 3:   return "Three";
	//         default:  return "<Unknown number>";
	//     }
	//  }
	// 
	//  for (const char* SquaredValue : Values | TransformedRange(&ToString))
	//  {
	//      // "SquaredValue" during iterations will be equals to
	//      // "One", "Two", "Three", "<Unknown number>"
	//  }
	// 
	template<typename TransformFuncType>
	auto TransformedRange(TransformFuncType&& TransformFunc);

	// --------------------- FilteredAndTransformedRange<...>(PassOnTrueFilterAndTransform) --------------------
	//
	// IN:
	//    (1) PassOnTrueFilterAndTransform
	//       Functor that provides transformatio and filtering logic
	//
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	// 
	// Form filtered Range from set values returned by "PassOnTrueFilterAndTransform"
	// Functor argument "PassOnTrueFilterAndTransform" provides transformation logic
	// and return "TUnrealRanges_Optional<{TransformedType}>"
	// If returned "TUnrealRanges_Optional<...>" value is set, it value is passed for formed Range
	// (using "TUnrealRanges_Optional<...>::GetValue()").
	// Otherwise, Parent Range element is filtered out
	// 
	// ~~~~ Example ~~~~
	//
	//  // Transforming strings to Enums that mapped to this strings
	//
	//  enum class EEnum
	//  {
	//      One,
	//      Two,
	//      Three
	//  }
	// 
	//  TUnrealRanges_Optional<EEnum> FromString(const std::string& String)
	//  {
	//      if (String == "One")
	//          return MakeOptional(EEnum::One);
	//      else if (String == "Two")
	//          return MakeOptional(EEnum::Two);
	//      else if (String == "Three")
	//          return MakeOptional(EEnum::Three);
	//      else
	//          return MakeOptional();
	//  }
	// 
	//  std::vector<std::string> StringsToConvert{ "One", "Two", "Three", "Four" };
	//
	//  for (EEnum ConvertedEnum | FilteredAndTransformedRange(&FromString))
	//  {
	//      // "ConvertedEnum" during iterations will be equals to
	//      // "EEnum::One", "EEnum::Two", "EEnum::Three"
	//      //
	//      // "Four" will be filtered out during provided filtered transform
	//      // because "FromString(...)" returns empty optional for such input string
	//  }
	//
	template<typename PassOnTrueFilterAndTransformPredicateType>
	auto FilteredAndTransformedRange(PassOnTrueFilterAndTransformPredicateType&& PassOnTrueFilterAndTransform);

	// ---------------------------- ReverseRange<...>() -------------------------------
	//
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	// 
	// Reversed range. Iterating it you will get elements of original range in
	// reverse order
	// 
	// !NB: Parent Range should support "--" operator call
	// 
	// ~~~~ Example ~~~~
	//
	// std::vector<int> Range{ 1, 2, 3, 4, 5 };
	// 
	// for (int32 Element : TestRange | ReverseRange())
	// {
	//     // "Element" during iterations will be equals to
	//     // "5", "4", "3", "2", "1"
	//     //
	//     // Values are got by getting elements of Parent Range in
	//     // reverse order
	// }
	//
	auto ReverseRange();

	// --------------- RecursiveRange<...>(RootValue, StepTransformFunc) ------------------
	//
	// IN:
	//    (1) RootValue
	//       Value that is used for starting recursive iteration over 
	//       NB1! Value will be used as first Range element
	//
    //    (2) StepTransformFunc
	//       Functor that provide recursive descent logic
	// 
	// OUT:
	//    {ret}
	//         Range that performs actions described below...
	//
	// Takes Value provided from the previous iteration ("RootValue" for first iteration)
	// and get next from return value of the "StepTransformFunc({CurrentValue})" call
	//
	// ~~~~ Example ~~~~
	//
	// std::list<int> List{ 1, 2, 3 };
	// 
	// auto Lambda = [](const std::list<int>::iterator Iterator)
	// {
	//    return std::next(Iterator);
	// };
	// 
	// for (const std::list<int>::iterator Iterator : RecursiveRange(List.begin(), Lambda))
	// {
	//    const int Value = *Iterator;
	// 
	//    // "Value" during iterations will be equals to
	//    // "1", "2", "3"
	//    //
	//    // Values are got by "recursive" access next elemenet for list iterator
	//    // This approach may be used for iterating over tree branches
	// }
	//
	template<typename ValueType, typename StepTransformFuncType>
	auto RecursiveRange(ValueType RootValue, StepTransformFuncType&& StepTransformFunc);

	// --------- StepIntoRange<...>(StepIntoFunc) ---------
	//
	// IN:
	//    (1) StepIntoFunc
	//       Functor "({Element})->{InnerRange}", where "{Element}" is Parent Range
	//       element and "{InnerRange}" is range that meant to be formed depended on
	//       "{Element}" (the most simple example - collection field of the "{Element}")
	// 
	// OUT:
	//    {ret}
	//         Range that performs actions described below...
	//
	// Function allow to iterate some subranges related to Parent Range elements
	// "concating" them into one Range
	// 
	// ! NB: Pay on attention for this fact: type of result Range elements will be
	//  "{InnerRange}" elements type
	// 
	// ~~~~ Example ~~~~
	// 
	//  struct FUpgrade
	//  {
	//      std::vector<int> GameplayEffectIds;
	//  };
	//  
	//  std::vector<FUpgrade> Upgrades
	//  {
	//     /*First upgrade*/  { { 1, 2 } },
	//     /*Second upgrade*/ { { 3, 4 } }
	//  };
	//  
	//  auto GetEffectIdLambda = [](const FUpgrade& Upgrade)
	//  {
	//      return Upgrade.GameplayEffectIds;
	//  };
	// 
	//  for (GameplayEffectId GameplayEffectId : Upgrades | StepIntoRange(GetEffectIdLambda))
	//  {
	//    // "GameplayEffectId" during iterations will be equals to
	//    // "1", "2", "3", "4"
	//    //
	//    // "1", "2" - will be got from the "Upgrades[0].GameplayEffectIds",
	//    // "3", "4" - will be got from the "Upgrades[1].GameplayEffectIds"
	//  }
	//
	// TODO: Implement "Concat" and replace this function to "StepIntoBy"!
	//
	template<typename StepIntoFuncType>
	auto StepIntoRange(StepIntoFuncType&& StepIntoFunc);

	// -------------- ConditionalIteratorRange<...>(ConditionalIterator) --------------
	//
	// IN:
	//    (1) ConditionalIterator
	//       "Conditional iterator" used to form Range API friendy Range
	//       as output (see details below)
	//
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	// 
	// In context of Ranges API "conditional iterator" is iterator that provides
	// condition-checking based iteration logic, like:
	// 
	// [1] for (TActorIterator<AActor> Iter{ World }; Iter; ++Iter) { ... }
	// 
	// Such iterators may be wrapped into Range API friendly Range for iterating
	// using "range for" loop.
	// 
	// Argument "ConditionalIterator (1)" type should support usage shown in [1].
	// If it is supported, "ConditionalIteratorRange<...>(...)" will return range
	// iteration over witch using "for loop" will support next quiality:
	// 
	// for (AActor* Actor : ConditionalIteratorRange(TActorIterator<AActor>{ World }))
	// {
	//     // Here "Actor" variable during iterations will takes the same values
	//     // that takes "Actor" variable in the next "condition-based" loop
	// }
	// 
	// for (TActorIterator<AActor> Iter{ World }; Iter; ++Iter)
	// {
	//     AActor* Actor = *Iter;
	// }
	// 
	// ~~~~ Example ~~~~
	//
	//  // Iterating over Locations of actors that are inside provided radius
	//  // around zero point
	//
	//  const float Radius = 750.f;
	// 
	//  auto GetActorLocationInRadius = [RadiusSquared = Radius * Radius](AActor* Actor)
	//  {
	//      const FVector ActorLocation = Actor->GetLocation();
	//      
	//      return (ActorLocation.SizeSquared() < Radius) ?
	//          TOptional<FVector>{ ActorLocation } :
	//          TOptional<FVector>{ };
	//  }; 
	// 
	//  for (const FVector LocationInRadius :
	//      ConditionalIteratorRange(TActorIterator<AActor>{ World }) |
	//      FilteredAndTransformedRange(GetActorLocationInRadius))
	//  {
	//     // "LocationInRadius" will takes value of Actor locations
	//     // for actors that placed inside sphere with radius "750.f"
	//     // placed in zero coordinates
	//  } 
	//
	template<typename ConditionalIteratorType>
	auto ConditionalIteratorRange(ConditionalIteratorType&& ConditionalIterator);

	// --------- EnumValuesRange<...>(StartValue, EndValue) ---------
	//
	// IN:
	//    (1) StartValue
	//       First Result Range element value
	//
	//    (2) EndValue
	//       Enum value on witch Result Range should be ended
	// 
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	// 
	// Construct range for iteration over "EnumType" possible values
	// starting from "StartValue" (included) until "EndValue" (NOT included!)
	// using  "NextEnum<EnumType>(Enum)" for getting next enum value.
	// 
	// ! NB: Range NOT include "EndValue"
	//
	// ~~~~ Example ~~~~
	//
	//  // Iterate over Enum values starting from specified
	// 
	//  enum class EEnum : uint8
	//  {
	//     One,
	//     Two,
	//     Three,
	//     END
	//  };
	//
	//  for (EEnum Enum : EnumValuesRange(EEnum::Two, EEnum::END))
	//  {
	//     // "Enum" during iterations will be equals to:
	//     //  "EEnum::Two", "EEnum::Three"
	//  }
	//
	template<typename EnumType>
	auto EnumValuesRange(const EnumType StartValue, const EnumType EndValue);

	// --------- ConditionalRange<...>(TargetRange, bUseTarget) ---------
	//
	// IN:
	//    (1) TargetRange
	//       The range that will cause effect only if bUseTarget == true
	//
	//	  (2) bUseTarget
	//       Makes Switch use TargetRange if bUseTarget == true,
	//       else Switch will use ParentRange
	//
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	//
	// Switches effect of TargetRange by flag in runtime
	//
	// ~~~~ Examples ~~~~
	//
	//  //.[Common example types & variables]
	// 
	//  std::vector<std::string> WayPoints{ "Point1", "Point2", "Point3" };
	//
	//  //.[Example #1]
	//  // Iterating points forward and backward
	//
	//  enum class EDirection
	//  {
	//      Forward,
	//      Backward
	//  }
	//  
	//  void TraceWay(const EDirection Direction)
	//  {
	//      const auto WayPointsRange =
	//          WayPoints |
	//          ConditionalRange(ReverseRange(), Direction == EDirection::Backward);
	//
	//      for (const std::string& Point : WayPointsRange)
	//      {
	//          // If Direction == EDirection::Forward,
	//          //  "Point" during iterations will be equals to:
	//          //  "std::string{ "Point1" }", "std::string{ "Point2" }",
	//          //  "std::string{ "Point3" }"
	//          //
	//          // else, if Direction == EDirection::Backward,
	//          //  "Point" during iterations will be equals to:
	//          // "std::string{ "Point3" }", "std::string{ "Point2" }",
	//          //  "std::string{ "Point1" }"
	//      }
	//  }
	//
	template<typename TargetRangeType>
	auto ConditionalRange(TargetRangeType&& TargetRange, bool bUseTarget);

	// --------- SetOperationRange<Operation, ...>(RangeA, RangeB) ---------
	//
	// IN:
	//    <Operation>
	//       Operation that should be perform when forming result Range
	//       Operations are passed as bitflag enum values using
	//       "UnrealRanges::Utils::SetOperation". There are three possible
	//       operations:
	//        - "A_B" aka "A - B": will add to the result Range elements from the
	//           "subtraction B from A", "values that ARE in A, but ARE NOT in B"
	//        - "B_A" aka "B - A": will add to the result Range elements from the
	//           "subtraction A from B", "values that ARE NOT in A, but ARE in B"
	//        - "AnB" aka "A and B intersection" will add to the result Range
	//           "value that ARE in A, AND that ARE in B"
	//       As operations are passed using bitmask, function allow to pass
	//       several desribed values in one time to get appropriate "concatinated"
	//       subsets in rather more optimized way then "concating" them using 
	// 
	//    (1) RangeA
	//       Range that is used as "Set A" for the Operation
	//
	//	  (2) RangeB
	//       Range that is used as "Set B" for the Operation
	// 
	// OUT:
	//    {ret}
	//         Range that contains elements got by applaying Operation to the
	//         passed Ranges
	//         
	// NB 1: Currently ranges should have same element types
	//
	// NB 2: Range elements should support "{Element} == {Element}" operation
	//
	// NB 3: It is assumed that passed Ranges does not contains repeats.
	//  For Ranges with repeats using "SetOperationRange<>()" may cause
	//  unexpected behaviour: not "undefined behavior", but "strange"
	//  in some cases. For example for next ranges:
	//  "RangeA = {1, 1, 2}" and "RangeB = {3, 1}" resulting Range for
	//  operation "AnB" will be "{ 1, 1 }", but if pass Ranges in other
	//  order: "RangeA = {3, 1}" and "RangeB = {1, 1, 2}" result will be
	//  "{ 1 }". Be aware and remember this issue when using this API 
	//
	// NB 4: When passing several Operations order of elements in the result
	//  Range is not defined
	// 
	// ~~~~ Examples ~~~~
	// 
	//  std::vector<int> A{ 1, 2, 3 };
	//  std::vector<int> B{    2, 3, 4 };
	//
	//  using namespace SetOperation;
	//
	//  //[1]
	//  for (const int Value : SetOperationRange<AnB>(A, B))
	//  {
	//     // "Value" during iterations will be equals to:
	//     // "2", "3"
	//     // because "AnB" cause intersection operation
	//     // ("elements that ARE in A AND in B")
	//     // and elements presented in the list above
	//     // are according to this operation condition
	//  }
	//  
	//  //[2]
	//  for (const int Value : SetOperationRange<A_B>(A, B))
	//  {
	//  	// "Value" during iterations will be equals to:
	//      // "1"
	//      // because "A_B" is substraction operation
	//      // ("elements that ARE in A, but ARE NOT in B")
	//  }
	//  
	//  //[3]
	//  for (const int Value : SetOperationRange<B_A>(A, B))
	//  {
	//  	// "Value" during iterations will be equals to:
	//      // "4"
	//      // because "B_A" is substraction operation
	//      // ("elements that ARE NOT in A, but ARE in B")
	//  }
	//  
	//  //[4]
	//  for (const int Value : SetOperationRange<B_A | AnB>(A, B))
	//  {
	//  	// "Value" during iterations will be equals to:
	//      // "2", "3", "4"
	//      // because two subsets are requested: "B_A"
	//      // ("B without A") as in the loop [3] and "AnB"
	//      // ("Elements in A and in B") as in loop [1]
	//      // Subsets are returned
	//  }
	//
	template<int Operation, typename RangeAType, typename RangeBType>
	auto SetOperationRange(RangeAType&& RangeA, RangeBType&& RangeB);

	// ===============================| Pipe operator |========================================

	// --------- operator|<...>(ParentRange, ChildRangeDummy) ---------
	//
	// IN:
	//    (1) ParentRange
	//       Range that should be processed by "ChildRangeDummy"
	// 
	//    (2) ChildRangeDummy
	//       Range-Part that used for processing
	// 
	// OUT:
	//    {ret}
	//         Range-Part that performs actions described below...
	// 
	// The basics ranges combination operator. Form Range that perform
	// filtering and/or transforming of "ParentRange" by "ChildRangeDummy"
	//
	// ~~~~ Example ~~~~
	//
	//  // Transforming numbers to words that descibes this numbers
	// 
	//  std::vector<int32> Values{ 1, 2, 3, 4 };
	//  
	//  const char* ToString(int32 Value)
	//  {
	//     switch (Value)
	//     {
	//         case 1:   return "One";
	//         case 2:   return "Two";
	//         case 3:   return "Three";
	//         default:  return "<Unknown number>";
	//     }
	//  }
	// 
	//  for (const char* SquaredValue : Values | TransformedRange(&ToString))
	//  {
	//      // "SquaredValue" during iterations will be equals to
	//      // "One", "Two", "Three", "<Unknown number>"
	//  }
	// 
	template<typename ParentRangeType, typename DummyChildRangeType>
	auto operator | (ParentRangeType&& ParentRange, DummyChildRangeType&& ChildRangeDummy)->
		decltype(ChildRangeDummy.Next(Private::Utils::Capture(std::forward<ParentRangeType>(ParentRange))));

} // namespace UnrealRanges

#include "Private/UnrealRanges_Impl_Basics.inl"
