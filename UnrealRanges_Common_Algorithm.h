#pragma once

#include <initializer_list> //for EqualsToSome<...>(...)

namespace UnrealRanges{ namespace Algorithm{


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

	
	// ****************** !!! NB !!! ******************
	// If you have some specific collection in your
	// project - it's good idea to overload some functions
	// in this file to optimize common actions with then.
	// You may find an example of such overloading
	// in several places of UnrealRanges lib:
	// - At the last, "Common Overloadings" section of
	//    "UnrealRanges_Common_Algorithm.h"
	// - At the last, "Common Overloadings" section of
	//    "UnrealRanges_Unreal_Algorithm.h"
	// ****************** !!! NB !!! ******************


	// --------- Contains<...>(Range, Value) ---------
	//
	// IN:
	//    (1) Range
	//      Range to check if "Value" is in
	//    
	//    (2) Value
	//      Value to check for existing in Range
	// 
	// OUT:
	//    {ret}
	//        "true" if Value (2) is in Range (1)
	//        "false" otherwise
	// 
	// Check containing of Value using comparation
	// "{RangeElement} == Value"
	// 
	// ~~~~ Example ~~~~
	//
	//  // Transforming numbers to words that descibes this numbers
	// 
	//  auto IsPositive = [](const int Value)
	//  {
	//      return Value > 0;
	//  };
	// 
	//  std::vector<int> Values{ -1, 0, 1, 2 };
	//  auto PositiveValues = Values | FilteredRange(IsPositive);
	// 
	//  const bool bFirst = Contains(Values, -1);
	//  const bool bSecond = Contains(PositiveValues, -1);
	//  // "bFirst" <=> "true" because "Values" contains negative
	//       element "-1"
	//  // "bSecond" <=> "false" because "PositiveValues" is range with
	//  //   filtered out negative & zero elements, so there is no "-1" in
	// 
	template <typename RangeType, typename ValueType>
	bool Contains(RangeType&& Range, ValueType&& Value);

	// --------- ContainsByPredicate<...>(Range, Pred) ---------
	//
	// IN:
	//    (1) Range
	//      Range to check if "Value" is in
	//    
	//    (2) Pred
	//      Predicate for checking element existing in Range
	// 
	// OUT:
	//    {ret}
	//        "true" if Value (2) is in Range (1)
	//        "false" otherwise
	// 
	// Check if there is an element in range for wich expression
	// "Pred({RangeElement})" is "true"
	// 
	// ~~~~ Example ~~~~
	//
	//  // Transforming numbers to words that descibes this numbers
	// 
	//  auto IsPositive = [](const int Value)
	//  {
	//      return Value > 0;
	//  };
	// 
	//  std::vector<int> Values{ -1, 0, 1, 2 };
	//  auto PositiveValues = Values | FilteredRange(IsPositive);
	// 
	// 	auto IsZero = [](const int Value)
	//  {
	//      return Value == 0;
	//  };
	// 
	//  const bool bFirst = ContainsByPredicate(Values, IsZero);
	//  const bool bSecond = ContainsByPredicate(PositiveValues, IsZero);
	//  // "bFirst" <=> "true" because "Values" contains zero element "0"
	//  // "bSecond" <=> "false" because "PositiveValues" is range with
	//  //   filtered out negative & zero elements, so there is no "0" in
	// 
	template <typename RangeType, typename PredicateType>
	bool ContainsByPredicate(RangeType&& Range, PredicateType&& Pred);

	// --------- ContainsBy<...>(Range, Value, Proj) -----------
	//
	// IN:
	//    (1) Range
	//      Range to check if "Value" is in elements projections
	//    
	//    (2) Value
	//      Value to check containing in projected Range
	// 
	//    (3) Proj
	//      Projection to apply for each Range element when
	//      checking
	// 
	// OUT:
	//    {ret}
	//        "true" if Value (2) is in projection Proj (3)
	//         of Range (1)
	//        "false" otherwise
	// 
	// Check if there is an element in range for wich expression
	// "Proj({RangeElement}) == Value" is "true"
	// 
	// ~~~~ Example ~~~~
	//
	//  // Checking for Imposter
	//  
	//  enum class ERole
	//  {
	//      Crewmate,
	//      Impostor
	//  };
	//  
	//  struct FAstronaut
	//  {
	//      std::string Name;
	//      ERole Role;
	//  };
	//  
	//  auto IsCrewmate = [](const FAstronaut& Astronaut)
	//  {
	//      return (Astronaut.Role == ERole::Crewmate);
	//  };
	//  
	//  auto GetRole = [](const FAstronaut& Astronaut)
	//  {
	//      return Astronaut.Role;
	//  };
	//  
	//  std::vector<FAstronaut> Astronauts{
	//      { "dontkillme", ERole::Crewmate },
	//      { "gnomegang", ERole::Crewmate },
	//      { "spacemonk", ERole::Impostor },
	//      { "knuckles", ERole::Crewmate }
	//  };
	//  
	//  auto CrewmatesRange = Astronauts | FilteredRange(IsCrewmate);
	//  
	//  const bool bRangeHasImposter1 = ContainsBy(Astronauts, ERole::Impostor, GetRole);
	//  const bool bRangeHasImposter2 = ContainsBy(CrewmatesRange, ERole::Impostor, GetRole);
	//  // "bRangeHasImposter1 <=> true" because there is an "FAstronaut" with "Role" equals
	//  //   to "ERole::Impostor" in "Astronauts" Range
	//  // "bRangeHasImposter2 <=> false" because range "CrewmatesRange" contains only
	//  //   "FAstronaut" with "Role" equals to "ERole::Crewmate"
	//  //   (imposters are filtered out by "FilteredRange()" call) 
	//
	template <typename RangeType, typename ValueType, typename ProjectionType>
	bool ContainsBy(RangeType&& Range, const ValueType& Value, ProjectionType&& Proj);

	// ---------  SelectWithFirstPick<...>(Range, SelectionFunc) &  ---------
	//                    Select(Range, SelectionFunc)                      
	//
	// IN:
	//    (1) Range
	//      Range to select value from
	//    
	//    (2) SelectionFunc
	//      Functor with signature "bool(Current, Best)" where
	//      "Current" - Range (1) element during iteration
	//      "Best" - best element value for [Function #2] OR
	//         optional with best element value for [Function #1]
	//      {ret} - "true" if "Current" is "better" then "Best" and
	//         should be selected as new "Best"
	//       - "false" if "Best" is "better" then "Current"
	// 
	// OUT:
	//    {ret}
	//      Optional filled with selected element or
	//      empty optional if nothing was selected or if
	//      Range (1) was empty
	// 
	// Selecting element from Range by SelectionFunc. Selection - is logic
	// that find best element by specified condition with possibility to select
	// "Current" as new "Best".
	// [Function #1] allow to decide if first Range element should be picked as best
	// [Function #2] assume that first Range element should be picked as starting best
	// 
	// ~~~~ Example ~~~~
	//
	//  //.[Common example types & variables]
	// 
	//  struct FWinPoints
	//  {
	//      int TeamId = 0;
	//      int KillPoints = 0;
	//      int LostControlPoints = 0;
	//  };  
	//  
	//  std::vector<FWinPoints> WinPoints{ { 1, 3, 1 }, { 2, 4, 1 }, { 3, 1, 0 } };
	//  
	//  //.[Example #1]
	//  // Selecting best killer team with no Control Points losing, using function
	//  // as SelectionFunc (2) (lambda may be used here too)
	//  // This is an example for [Function #1] 
	//  
	//  bool ShouldSelectAsBestKillers_NoControlPointsLost(
	//	    const FWinPoints& Current,
	//      const TUnrealRanges_Optional<FWinPoints>& Best)
	//  {
	//      if (Current.LostControlPoints > 0)
	//          return false;
	//  
	//      if (!IsOptionalSet(Best))
	//          return true;
	//  
	//      return (Current.KillPoints > GetOptionalValue(Best).KillPoints);
	//  }
	//  
	//  const TUnrealRanges_Optional<FWinPoints> Res1 = SelectWithFirstPick(
	//      WinPoints, ShouldSelectAsBestKillers_NoControlPointsLost);
	//  // "Res1" optional is filled by value equals to "FWinPoints{ 3, 1, 0 }"
	//  // because only "WinPoints[2]" has "LostControlPoints" equals to zero
	// 
	//  //.[Example #2]
	//  // Just selecting best killer team, without any additional conditions, using
	//  // labmda as SelectionFunc (2) (function may be used here too)
	//  // This is an example for [Function #2] 
	// 
	//  auto ShouldSelectAsBestKillers =
	//      [](const FWinPoints& Current, const TUnrealRanges_Optional<FWinPoints>& Best)
	//      {
	//          if (!IsOptionalSet(Best))
	//              return true;
	// 
	//          return (Current.KillPoints > GetOptionalValue(Best).KillPoints);
	//      };
	// 
	//  const TUnrealRanges_Optional<FWinPoints> Res2 = SelectWithFirstPick(
	//      WinPoints, ShouldSelectAsBestKillers);
	//  // "Res2" optional is filled by value equals to "FWinPoints{ 2, 4, 1 }"
	//  // because "WinPoints[1]" has the biggest "KillPoints" value and no other
	//  // conditions are passed to selection functor
	// 
	// [Function #1]
	template <typename RangeType, typename SelectionWithFirstPickFuncType>
	auto SelectWithFirstPick(RangeType&& Range, SelectionWithFirstPickFuncType&& SelectionFunc);

	// [Function #2]
	template <typename RangeType, typename SelectionFuncType>
	auto Select(RangeType&& Range, SelectionFuncType&& SelectionFunc);

	// --------- MinElementBy_RetOptional<...>(Range, Proj) & ---------
	//             MaxElementBy_RetOptional<...>(Range, Proj)
	//
	// IN:
	//    (1) Range
	//      For [Function #1] - range to find minimum element in
	//      For [Function #2] - range to find maximum element in
	// 
	//    (2) Proj
	//      Projection of Range (1) that is used for getting value
	//      by wich finding of best element is performed
	// 
	// OUT:
	//    {ret}
	//        "TUnrealRanges_Optional<>" filled with element found
	//         best element or not filled optional if "Range" is empty
	// 
	// Find best element by criteria...
	// ...for [Function #1] - element "Proj({RangeElement}) < {Best}"
	// ...for [Function #2] - element "Proj({RangeElement}) > {Best}"
	//
	// ~~~~ Example ~~~~
	//
	//  //.[Example #1]
	//  // Example for "MinElementBy_RetOptional<>()"
	//  // Find Unit with less health
	// 
	//  struct FUnitData
	//  {
	//      int Health = 0;  
	//  };
	//
	//  auto GetHealth = [](const FUnit& Unit)
	//  {
	//      return Unit.Health;
	//  };
	// 
	//  std::vector<FUnitData> EmptyUnitDatas{ };
	//  std::vector<FUnitData> UnitDatas{ { 30 }, { 10 }, { 20 } };
	// 
	//  const TUnrealRanges_Optional<FUnitData> UnitData1 =
	//      MinElementBy_RetOptional(EmptyUnitDatas, GetHealth);
	//  const TUnrealRanges_Optional<FUnitData> UnitData2 =
	//      MinElementBy_RetOptional(UnitDatas, GetHealth);
	//  //
	// 	// "UnitData1" is not filled optional because "EmptyUnitDatas" is
	//  //    empty range
	//  // "UnitData2" value is equals to "FUnitData{ 10 }" because
	//  //    the minimum value returned by projection "GetHealth"
	//  //    for elements of range "EmptyUnitDatas" is "10" for second
	//  //    element
	// 
	//  //.[Example #2]
	//  // Example for "MaxElementBy_RetOptional<>()"
	//  // Find team with the best scores
	//  
	//  struct FTeamResults
	//  {
	//      int TeamId = 0;
	//      int KillsNum = 0;
	//      int ControlPointCapturesNum = 0;
	//  };
	// 
	//  auto GetTeamScores = [](const FTeamResults& TeamResults)
	//  {
	//      const int ScoresForKill = 1;
	//      const int ScoresForControlPointCapture = 5;
	//      
	//      return
	//          TeamResults.KillsNum * ScoresForKill +
	//          TeamResults.ControlPointCapturesNum * ScoresForControlPointCapture;
	//  };
	// 
	//  std::vector<FTeamResults> EmptyGameResults{ };
	//  std::vector<FTeamResults> GameResults{ { 1, 9, 0 }, { 2, 4, 2 }, { 3, 0, 3 } };
	// 
	//  const TUnrealRanges_Optional<FTeamResults> BestTeamResults1 =
	//      MinElementBy_RetOptional(EmptyUnitDatas, GetHealth);
	//  const TUnrealRanges_Optional<FTeamResults> BestTeamResults2 =
	//      MinElementBy_RetOptional(UnitDatas, GetHealth);
	//  //
	//  // "BestTeamResults1" is not filled optional because "EmptyUnitDatas" is
	//  //    empty range  
	//  // "BestTeamResults2" value is equals to "FTeamResults{ 3, 0, 3 }" because
	//  //    projection that is used for computing points of "GameResults" are next
	//  //    for elements: "9", "14", "15", so third "GameResults" element
	//  //    should be returned
	// 
	// [Function #1]
	template <typename RangeType, typename ProjectionType>
	auto MinElementBy(RangeType&& Range, ProjectionType&& Proj);

	// [Function #2]
	template <typename RangeType, typename ProjectionType>
	auto MaxElementBy(RangeType&& Range, ProjectionType&& Proj);

	// --------------- FindByPredicate<...>(Range, Pred) & ------------------------
	//                   IndexByPredicate<...>(Range, Pred) 
	//
	// IN:
	//    (1) Range
	//      Range where the element should be found
	// 
	//    (2) Pred
	//      Predicate to check if current range element is that on that
	//      should be found
	// 
	// OUT:
	//    {ret}
	//        - For "[Function #1]" - returns "TUnrealRanges_Optional<{Element}>"
	//          filled with found element or not filled optional value if element
	//          was not found in Range (1) or Range (1) is empty
	//        - For [Function #2] - returns "TUnrealRanges_Optional<unsigned int>"
	//          filled with index by which found element may be accessed
	//
	// ~~~~ Example ~~~~
	//
	// //.[Example #1]
	// // Example for "FindByPredicate<>()"
	// // Find Imposter
	//
	//  enum class ERole
	//  {
	//      Crewmate,
	//      Impostor
	//  };
	//  
	//  struct FAstronaut
	//  {
	//      std::string Name;
	//      ERole Role;
	//  };
	//
	//  auto IsImposter = [](const FAstronaut& Astronaut)
	//  {
	//      return (Astronaut.Role == ERole::Impostor);
	//  };
	//  
	//  std::vector<FAstronaut> Astronauts{
	//      { "dontkillme", ERole::Crewmate },
	//      { "gnomegang", ERole::Crewmate },
	//      { "spacemonk", ERole::Impostor },
	//      { "knuckles", ERole::Crewmate }
	//  };
	//
	//  using namespace Logic;
	//  auto CrewmatesRange = Astronauts | FilterRange<Not>(IsImposter);
	// 
	//  const TUnrealRanges_Optional<FAstronaut> Imposter1 =
	//      FindByPredicate(Astronauts, IsImposter);
	//  const TUnrealRanges_Optional<FAstronaut> Imposter2 =
	//      FindByPredicate(CrewmatesRange, IsImposter);
	//  //
	//  // "Imposter1" optional value is equals to third "Astronauts"
	//  //    element because it's role is "ERole::Impostor"
	//  // "Imposter2" optional is not filled because "CrewmatesRange"
    //  //    has no element that passes "IsImposter" creteria
	//
	//  //.[Example #2]
	//  // Example for "IndexByPredicate<>()"
	//  // Find index of Crewmate to kick off
	//  
	//  const TUnrealRanges_Optional<unsigned int> KickOffIndex =
	//     IndexByPredicate(Astronauts, IsImposter);
	//  //
	//  // "KickOffIndex" will be optional value filled by "2"
	//  // because "{ "spacemonk", ERole::Impostor }" is third element
	//  // of the "Astronauts" vector
	// 
	// 	const TUnrealRanges_Optional<unsigned int> OtherKickOffIndex =
	//     IndexByPredicate(CrewmatesRange, IsImposter);
	//  //
	//  // "OtherKickOffIndex" will be not filled optional value because
	//  // "CrewmatesRange" has no Imposters
	// 
	// [Function #1]
	template <typename RangeType, typename PredicateType>
	auto FindByPredicate(RangeType&& Range, PredicateType&& Pred);

	// [Function #2]
	template<typename RangeType, typename PredicateType>
	auto IndexByPredicate(RangeType&& Range, PredicateType&& Pred);

	// --------- FindNextByPredicate<...>(Range, Pred) ---------
	//
	// IN:
	//    (1) Range
	//      Range where the element should be found
	// 
	//    (2) Pred
	//      Predicate to check if the next range element should be found
	// 
	// OUT:
	//    {ret}
	//        "TUnrealRanges_Optional<>" filled with found element
	//         or not filled optional if element was not found in Range (1)
	//         or Range (1) is empty
	//
	// ~~~~ Example ~~~~
	//
	//  Find previous item in inventory
	//  
	//  std::vector<std::string> Items{
	//      "Item1",
	//      "Item2",
	//      "Item3",
	//  };
	//  
	//  const auto ReverseInventory = Items | ReverseRange();
	//  
	//  const std::string CurrentItem = "Item3";
	//  const auto IsCurrentItem = [CurrentItem](const std::string& Item) 
	//  {
	//      return Item == CurrentItem;
	//  }
	//  
	//  const TUnrealRanges_Optional<std::string> PreviousItem = 
	//      FindNextByPredicate(ReverseInventory, IsCurrentItem);
	//  
	//  // "PreviousItem" contains "Item2"
	// 
	template<typename RangeType, typename PredicateType>
	auto FindNextByPredicate(RangeType&& Range, PredicateType&& Pred);

	// ------------- GetByIndex<...>(Range, Index) & ------------------
	//                GetByIndexSafe<...>(Range, Index) 
	//
	// IN:
	//    (1) Range
	//      Range from which elemen by index should be taken
	// 
	//    (2) Index
	//      Index of the element that should be taken from the range
	// 
	// OUT:
	//    {ret}
	//       For [Function #1]: Copy of the value placed by Index (2)
	//        place in the Range (1). Cause an Undefined Behaviour if
	//        range has not enough elements to be accessed by the Index (2)
	//       For [Function #2]: "TUnrealRanges_Optional<{RangeElementType}>"
	//        filled with value placed by Index (2) place in the Range (1) OR
	//        unfilled "TUnrealRanges_Optional<{RangeElementType}>" if the
	//        Range (1) has not enough elements to be accessed by the Index (2)
	//
	// ! NB: Recommended for Common Overloading. Read about Common
	//   Overloadings at the end of "UnrealRanges_Common_Algorithm.h"
	// 
	//TODO: Use "Check()" for the "GetByIndex()" instead of unhandled UB
	// 
	// ~~~~ Example ~~~~
	//
	//  //.[Common example types & variables]
	// 
	// std::vector<int> PrimeNumbers{ 1, 2, 3, 5, 7 };
	//
	//  //.[Example #1]
	//  // Example for "GetByIndex<...>(Range, Index)"
	//  // Prime number by index
	//
	// const int Value1 = GetByIndex(PrimeNumbers, 2);
	// //
	// // "Value1" will be filled with "3" value - because this is 
	// // Common Overloading of the function will be used here because
	// // "std::vector" is provided as Range (1)
	//
	// // const int Value2 = GetByIndex(PrimeNumbers, 5);
	// //
	// // Calling line above will cause Undefined Behaviour because Range (1)
	// // has only five elements when element that has "5" index should have
	// // "6" elements
	// 
	//  //.[Example #2]
	//  // Example for "GetByIndexSafe<...>(Range, Index)"
	//  // More then "2" prime number by index
	// 
	// auto Lambda = [](const int Number)
	// {
	//     return Number > 2;
	// };
	// 
	// auto PrimeNumbersMoreThenTwo = PrimeNumbers | FilteredRange(Lambda);
	// //
	// // Range will contains next values: "3", "5", "7"
	// 
	// const int Value3 = GetByIndexSafe(PrimeNumbers, 2);
	// //
	// // "Value1" will be "TUnrealRanges_Optional<>" filled with "7" value
	// // Common Overloadings will be not used here - because "Range" type
	// // is not STL or Unreal collection, it is "Unreal Ranges" range
	// // built by filtering "std::vector"
	// 
	// const int Value4 = GetByIndexSafe(PrimeNumbers, 3);
	// //
	// // "Value4" will be unfilled "TUnrealRanges_Optional<>" because
	// // Range (1) "PrimeNumbersMoreThenTwo" has only three elements when
	// // taking element by index "3" requires "4" elements 
	// 
	// [Function #1]
	template <typename RangeType, typename ValueType>
	auto GetByIndex(RangeType&& Range, const unsigned int Index);

	// [Function #2]
	template <typename RangeType, typename ValueType>
	auto GetByIndexSafe(RangeType&& Range, const unsigned int Index);

	// --------- GetRangeCount<...>(Range) ---------
	//
	// IN:
	//    (1) Range
	//       Range to compute elements count for
	// 
	// OUT:
	//    {ret}
	//         Elements count in Range
	// 
	// ~~~~ Example ~~~~
	//
	//  std::vector<int> InitialRange{ -2, -1, 0, 1, 2, 3, 4, 5 }; 
	// 
	//  auto OddsFilteredRange =
	//      InitialRange |
	//      FilteredRange([](int Number){ return (Number % 2 != 0); });
	// 
	//  const int OddsFilteredRangeCount = GetRangeCount(OddsFilteredRange);
	//  // Range "OddsFilteredRange" contains next elements: "-1, 1, 3, 5",
	//  // so "OddsFilteredRangeCount" is equals to "4" 
	// 
	//  auto PositiveOddsFilteredRange =
	//      OddsFilteredRange |
	//      FilteredRange([](int Number){ return (Number > 0); });
	// 
	//  const int PositiveOddsFilteredRangeCount = GetRangeCount(PositiveOddsFilteredRange);
	//  // Range "OddsFilteredRange" contains next elements: "1, 3, 5",
	//  // so "PositiveOddsFilteredRangeCount" is equals to "3" 
	//
	template <typename RangeType>
	int GetRangeCount(const RangeType& Range);

	// --------- GetRandomRangeElement<...>(Range, Pred) ---------
	//
	// IN:
	//    (1) Range
	//       Range to get random element from
	// 
	// OUT:
	//    {ret}
	//         Random element of Range
	// 
	// ! NB: Recommended for Common Overloading. Read about Common
	//   Overloadings at the end of "UnrealRanges_Common_Algorithm.h"
	// 
	// ~~~~ Example ~~~~
	//  
	//  // Getting random element from "std::vector"
	// 
	//  const int RandomValue = GetRandomRangeElement(std::vector<int>{ 1, 2, 3, 5, 7 });
	//  //
	//  // "RandomPrimeValue" is some random value from provided std::vector
	//
	template <typename RangeType>
	auto GetRandomRangeElement(const RangeType& Range);

	// ------------- RangeHasElements<...>(Range) -------------
	//
	// IN:
	//    (1) Range
	//       Range to check if there is at least one element in
	// 
	// OUT:
	//    {ret}
	//         "true" if range is has at least one element,
	//         "false" otherwise
	//
	// ! NB: Recommended for Common Overloading. Read about Common
	//   Overloadings at the end of "UnrealRanges_Common_Algorithm.h"
	//  
	// ~~~~ Example ~~~~
	//  
	//  // Checking odds/even Ranges
	//
	//  bool IsPositiveOrZero(const int Number)
	//  {
	//      return Number >= 0;
	//  }
	//  
	//  const std::vector<int> Numbers{ 0, 1, 2, 3, 4, 5 };
	//  
	//  using namespace Logic;
	// 
	//  auto PositiveOrZeroNumbers = Numbers | FilteredRange(IsPositiveOrZero);
	//  auto NegativeNumbers = Numbers | FilteredRange<Not>(IsPositiveOrZero);
	// 
	//  const bool bResult1 = RangeHasElements(PositiveOrZeroNumbers);
	//  // "bResult1 <=> true" because "Numbers" array contains
	//  // only positive or zero numbers
	// 
	//  const bool bResult2 = RangeHasElements(NegativeNumbers);
	//  // "bResult2 <=> false" because there is no negative
	//  // numbers in "Numbers" array
	//
	template<typename RangeType>
	bool RangeHasElements(const RangeType& Range);

	// ------------------------- RangeFirstElement<...>(Range) & -------------------------
	//                            RangeLastElement<...>(Range)
	//
	// IN:
	//    (1) Range
	//       Range to get first element from
	// 
	// OUT:
	//    {ret}
	//        "TUnrealRanges_Optional<{RangeElementType}>" filled with
	//        first (for [Function #1]) / last (for [Function #1]) Range element
	//        or not set "TUnrealRanges_Optional<{RangeElementType}>" if "Range" is empty
	//
	// ! NB: Recommended for Common Overloading. Read about Common
	//   Overloadings at the end of "UnrealRanges_Common_Algorithm.h"
	// 
	// ~~~~ Example ~~~~
	//
	//  //.[Common example types & variables]
	// 
	//  bool IsPositive(const int Number)
	//  {
	//      return Number > 0;
	//  }
	//  
	//  const std::vector<int> Numbers{ 0, 1, 2, 3, 4, 5 };
	//  
	//  using namespace Logic;
	// 
	//  auto PositiveNumbers = Numbers | FilteredRange(IsPositive);
	//  auto NegativeNumbers = Numbers | FilteredRange<Not>(IsPositive);
	// 
	//	//.[Example #1]
	//  // Example for "RangeFirstElement<...>(Range)"
	//  // First odds / even Ranges
	// 
	//  const TUnrealRanges_Optional<int> FirstPositiveNumber = RangeFirstElement(PositiveNumbers);
	//  //
	//  // "FirstPositiveNumber" variable is filled with "1" value
	//  // (it is first non-zero positive number in "Numbers")
	// 
	//  const TUnrealRanges_Optional<int> FirstNegativeNumber = RangeFirstElement(NegativeNumbers);
	//  //
	//  // "FirstNegativeNumber" is not set
	//  // (because "Numbers" has no negative numbers)
	//
	//	//.[Example #2]
	//  // Example for "RangeLastElement<...>(Range)"
	//  // First odds / even Ranges
	// 
	//  const TUnrealRanges_Optional<int> LastPositiveNumber = RangeLastElement(PositiveNumbers);
	//  //
	//  // "LastPositiveNumber" variable is filled with "5" value
	//  // (it is last non-zero positive number in "Numbers")
	// 
	//  const TUnrealRanges_Optional<int> LastNegativeNumber = RangeLastElement(NegativeNumbers);
	//  //
	//  // "LastNegativeNumber" is not set
	//  // (because "Numbers" has no negative numbers)
	// 
	// [Function #1]
	template<typename RangeType>
	auto RangeFirstElement(const RangeType& Range);

	// [Function #2]
	template<typename RangeType>
	auto RangeLastElement(const RangeType& Range);

	// --- EqualsToSome<...>(Value, ValuesToCompare) & EqualsToSome<...>(Value, Range) ---
	//
	// IN:
	//    (1) Value
	//       Value to check for existing in Range      
	// 
	//    (2) Range
	//       Collection to be filled from Range elements
	//
	//    (3) ValuesToCompare
	//       Used by [Overloading #2] "std::initializer_list<...>" range for
	//       checking "Value" containing
	// 
	// OUT
	//    {ret}
	//         "true" if Range passed as second argument contains "Value"
	//         (by "==" comparing). "false" otherwise
	//
	// [Overloading #2] is usefull for short inline checkes of containing some value in
	// group of specified possible values in form:
	// "EqualsToSome(Value, { Variants_to_check , ... })"
	// See [Example #2] for details
	//  
	// ~~~~ Examples ~~~~
	//
	//  //.[Example #1]
	//  // Checking new status for possible target to heal
	// 
	//  std::vector<AGameUnit*> Targets{ Actor1, Actor2, Actor3 };
	//  
	//  using namespace Logic;
	// 
	//  auto NotFullHPTargetsRange =
	//      Targets | RangeFilteredBy<Not>(&AGameUnit::HasFullHitPoints);
	// 
	//  // We asume that "GetFocusedUnit()" is some function that return new unit focused
	//  // by healing system
	//  const bool bNewTargetToHeal = EqualsToSome(GetFocusedUnit(), NotFullHPTargetsRange);
	// 
	//  //.[Example #2]
	//  // Checking if Status (used for some kind of state machine) is as expected
	//  
	//  check(EqualsToSome(GameStatus, { EStatus::Loading, EStatus::WaitingPlayers });
	// 
	//[Overloading #1]
	template<typename ValueType, typename RangeType>
	bool EqualsToSome(const ValueType& Value, const RangeType& Range);

	//[Overloading #2]
	template<typename ValueType>
	bool EqualsToSome(const ValueType& Value, const std::initializer_list<ValueType>& ValuesToCompare);

	// ========================================================================
	// ======================== Common overloadings ===========================
	// ========================================================================

	// ********************* ABOUT COMMON OVERLOADINGS ************************
	//                      
	// Common oveloadings - is overloadings for commonly used collections that
	// may be used as ranges, but has more API that allow to perform algorithms
	// more optimized
	// Common Overloadings destributes with Unreal Ranges library currently are
	// placed at the end of next files:
	// 
	// "UnrealRanges_Common_Algorithm.h" - for the STL collections
	//
	// "UnrealRanges_Unreal_Algorithm.h" - for the Unreal Engine collections
	//
	// If you are using STL and/or Unreal Engine collections - you may not
	// create any common overloadings. You are welcome to write for developer of
	// Unreal Ranges (semenyakinVSG@gmail.com) if you have any improvement ideas
	// for the Unreal Ranges "on deck" Common Overloadings.
	// 
	// If you are using some specific collections in your project, it's
	// recommended to create some Common Overloadings if possible and
	// include them with Unreal Ranges header.
	//
	// ********************* ABOUT COMMON OVERLOADINGS ************************

	//TODO: Implement Common Overloadings for the STL!

}} //namespace UnrealRanges::Algorithm

#include "Private/UnrealRanges_Impl_Common_Algorithm.inl"
