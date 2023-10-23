#pragma once

#include "UnrealRanges_Utils.h"

//Unreal
#include "Containers/Array.h"
#include "Math/UnrealMathUtility.h"//for FVector
#include "Misc/Optional.h"

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


	// --------- Exclude<...>(ArrayToExcludeFrom, RangeToExclude) --------
	//
	// IN:
	//    (1) ArrayToExcludeFrom
	//       "TArray" from which values of "RangeToExclude" should be
	//       excluded
	// 
	//    (2) RangeToExclude
	//       Range with elements that should be excluded from "ArrayToExcludeFrom"
	// 
	// ~~~~ Examples ~~~~
	// // Method that using excluding teammates from the array
	// 
	// class AGameUnit : public AActor
	// {
	//     // ...    
	// 
	//     int GetTeamId() const { return TeamId; }
	// 
	//     //NB: This implemention is just for an example.
	//     // It's much better to return range like
	//     // "ActorsRange<>() | Filtered<Not>(IsTeammate)"
	//     // in such case
	//     //
	//     TArray<AGameUnit*> GetEnemyUnits() const
	//     {
	//         auto IsTeammate = [this](AGameUnit* Other)
	//         {
	//              return this->GetTeamId() == Other->GetTeamId();
	//         };
	// 
	//         auto AllUnitsRange = ActorsRange<AGameUnit>(GetWorld());
	// 
	//         TArray<AGameUnit*> Result = RangeToArray(AllUnitsRange);
	//
	//         // Usage of "Exclude<>()", teammate units will be excluded
	//         // from the array "AllUnitsArray"
	//         Exclude(AllUnitsArray, AllUnitsRange | FilteredRange(IsTeammate)); 
	// 
	//         return Result;
	//     }
	// 
	//     // ...
	// };
	//
	template <typename ValueType, typename AllocatorType, typename RangeToExcludeType>
	void Exclude(TArray<ValueType, AllocatorType>& ArrayToExcludeFrom, const RangeToExcludeType& RangeToExclude);

	// ------ ClosestActorWithDistance<...>(Point, Range) &     -------
	//    ClosestActorWithDistance<...>(Point, RadiusLimit, Range)
	//
	// IN:
	//    (1) Point
	//       Point to compute distances to Range Actors from
	// 
	//    (2) RadiusLimit
	//       Radius in witch Actors should be found
	// 
	//    (3) Range
	//       Range of Actors from witch distances to Point
	//       should be computed
	// 
	// OUT:
	//    {ret}
	//         "TOptional< TPair<Distance {float}, Actor*> >"
	//         with nearest Actor and distance to it.
	//         If all actors in range are more far away
	//         then "RadiusLimit" or Range is empty -
	//         returned optional will be not set
	//
	// !NB: Function is not testing Elements validity of Range. You
	//  should validate input range by yourself - for example, using
	//  "... | ValidRange()" for range that should be passed to Functions
	//  and may contain invalid elements
	// 
	// ~~~~ Examples ~~~~
	//
	//  //.[Example #1]
	//  // Applying damage with falloff to closest Game Unit using [Overloading #1]
	// 
	//  const float BaseDamage = 100.f;
	//  const FVector DamageSourcePosition{ 300.f, 300.f, 300.f };
	// 
	//  TOptional<TPair<float, AGameUnit*>> ClosestUnitOptional =
	//      ClosestActorWithDistance(DamageSourcePosition, ActorsRange<AGameUnit>(World));
	//
	//  if (ClosestUnitOptional.IsSet())
	//  {
	//       const TPair<float, AGameUnit*>& Pair = ClosestUnitOptional.GetValue();
	//       Pair.Value->ApplyDamageWithFalloff(BaseDamage, Pair.Key);
	//  }
	//
	//  //.[Example #2]
	//  // Applying Heal Effect for nearest Game Unit limited by Radius using [Overloading #2]
	//  // Healing should find only damaged units - so we use filtered Actors range in example
	// 
	//  using namespace Logic; 
	// 
	//  auto DamagedActorsRange =
	//      ActorsRange<AGameUnit>(World) |
	//      RangeFilteredBy<Not>(&AGameUnit::HasFullHitPoints);
	// 
	//  const float BaseHealing = 50.f;
	//  const float EffectRadius = 250.f;
	//  const FVector EffectPosition{ 150.f, 150.f, 150.f };
	// 
	//  TOptional<TPair<float, AGameUnit*>> ClosestUnitToHeal =
	//      ClosestActorWithDistance(EffectPosition, EffectRadius, DamagedActorsRange);
	//
	//  if (ClosestUnitToHeal.IsSet())
	//  {
	//       const TPair<float, AGameUnit*>& Pair = ClosestUnitToHeal.GetValue();
	//       Pair.Value->ApplyHealing(BaseHealings, Pair.Key);
	//  }
	// 
	// [Overloading #1]
	template<typename RangeType>
	auto ClosestActorWithDistance(const FVector& Point, RangeType&& Range);

	// [Overloading #2]
	template<typename RangeType>
	auto ClosestActorWithDistance(const FVector& Point, const float RadiusLimit, RangeType&& Range);

	// ------ ClosestActor<...>(Point, Range) & ClosestActor<...>(Point, RadiusLimit, Range) -------
	//
	// IN:
	//    (1) Point
	//       Point to compute distances to Range Actors from
	// 
	//    (2) RadiusLimit
	//       Radius in witch Actors should be found
	// 
	//    (3) Range
	//       Range of Actors from witch distances to Point should be computed
	// 
	// OUT:
	//    {ret}
	//         Actor pointer if Actor was found in radius.
	//         If all actors in range are more far away
	//         then "RadiusLimit" or Range is empty -
	//         returned value will be "nullptr"
	//
	// !NB: Function is not testing Elements validity of Range. You
	//  should validate input range by yourself - for example, using
	//  "... | ValidRange()" for range that should be passed to Functions
	//  and may contain invalid elements
	// 
	// ~~~~ Examples ~~~~
	//
	//  //.[Example #1]
	//  // Send message to nearest from "MyGameUnit" team-mate
	//  // using [Overloading #1] for finding closest target.
	//  // We assume that "MyGameUnit" is not null and has method with
	//  // "bool IsTeammate(AGameUnit* Other)" signature for testing
	//  // of other Game Unit is teammate
	// 
	//  auto TeammatesRange = ;
	//      ActorsRange<AGameUnit>(World) |
	//      RangeFilteredBy(&AGameUnit::IsTeammate, MyGameUnit);
	// 
	//  const FString MessageText{ TEXT("Hello!") };
	// 
	//  if (AGameUnit* ClosestUnit = ClosestActor(MyGameUnit->GetLocation(), TeammatesRange))
	//  {
	//      ClosestUnit->SendMessage(MessageText);
	//  }
	//
	//  //.[Example #2]
	//  // Destroying of most close enemy Game Object in "DestroyRadius"
	//  // using [Overloading #2] for finding closest target.
	// 
	//  using namespace Logic;
	// 
	//  auto EnemiesRange = ;
	//      ActorsRange<AGameUnit>(World) |
	//      RangeFilteredBy<Not>(&AGameUnit::IsTeammate, MyGameUnit);
	// 
	//  const float DestroyRadius = 500.f;
	//  const FVector DestroySourcePosition{ 450.f, 450.f, 450.f };
	//
	//  if (AGameUnit* ClosestUnit = ClosestActor(DestroySourcePosition, DestroyRadius, EnemiesRange))
	//  {
	//      ClosestUnit->Destroy();
	//  }
	// 
	// [Overloading #1]
	template<typename RangeType>
	auto ClosestActor(const FVector& Point, RangeType&& Range);

	// [Overloading #2]
	template<typename RangeType>
	auto ClosestActor(const FVector& Point, const float RadiusLimit, RangeType&& Range);

	// ------ Shuffle<...>(RangeToShuffle, ElementsNumToShuffle) -------
	//
	// IN/OUT:
	//    (1) RangeToShuffle
	//       Range to shuffle. Currently only generalized "TArray" is supported
	// 
	// IN
	//    (1) ElementsNumToShuffle
	//       Num of elements from range to Shuffle
	//
	// ~~~~ Example ~~~~
	// 
	// // Shuffling the Deck of cards
	// 
	//  enum class ECard
	//  {
	//      Jack,
	//      Queen,
	//      King,
	//      Ace
	//  };
	// 
	//  TArray<ECard, TInlineAllocator<4>> Deck
	//  {
	//      ECard::Jack,
	//      ECard::Queen,
	//      ECard::King,
	//      ECard::Ace
	//  };
	//
	//  Shuffle(Deck, 3);
	//  // Call will shuffle first three elements of the "Deck"
	//
	template <typename ElementType, typename Allocator>
	void Shuffle(TArray<ElementType, Allocator>& ArrayToShuffle, const TOptional<int32>& ElementsNumToShuffle = {});

	// ----------------- AddAllFromRange<...>(Collection, SourceRange) --------------
	//
	// IN/OUT:
	//    (1) Collection
	//       Collection to fill from Range elements
	//
	// IN
	//    (1) SourceRange
	//       Range to fill "Collection" from
	//
	// Fills "Collection" by calling "Collection.Add({RangeElement})". Useful for
	// using Ranges with old-style copy-to-collections programming
	//  
	// ~~~~ Example ~~~~
	//
	//  // Copying Range elements to set for passing to existing API function
	// 
	//  // Some existing API from other Module that cannot be changed
	//  extern void SetupActorsFromDLCSettings(const TSet<AActor*>& Actors);
	//  
	//  auto NeedDLCSetup = [](AGameUnit* GameUnit)
	//	{
	//      return GameUnit->IsNeedDLCSetup();
	//	};
	// 
	//  // Read about "ActorsRange<...>(...)" below, in "Unreal specific ranges" section
	//  auto UnitsRangeForSetup = ActorsRange<AGameUnit>(World) | FilteredRange(&NeedDLCSetup);
	//
	//  // This call is correct: it's possible to call "SetForCall.Add({AGameUnit*})"
	//  // because upcasting "AGameUnit -> AActor" may be performed
	//  TSet<AActor*> SetForCall;
	//  AddAllFromRange(SetForCall, UnitsRangeForSetup);
	// 
	//  // Pass collection "TSet<AActor*>" expected by external API
	//  SetupActorsFromDLCSettings(SetForCall);
	//
	template<typename CollectionToFill, typename SourceRangeType>
	void AddAllFromRange(CollectionToFill& Collection, const SourceRangeType& SourceRange);

	// ----------------- RangeToArray<...>(Range) &   ----------------------
	//     RangeToArray<ArrayAllocatorType, ...>(Range)
	//
	// IN:
	//    <1>? ArrayAllocatorType = FDefaultAllocator
	//       Optional allocator type to be passed for "TArray<...>" that is
	//       constructing inside function
	// 
	//    (1) Range
	//       Collection to be filled from Range elements
	//
	// OUT
	//    {ret}
	//         Resulting "TArray<...>" with elements added from the Range
	//
	// Constructing and return "TArray<...>" with elements added from range
	// passed as "Range" argument
	//  
	// ~~~~ Examples ~~~~
	//
	//  //.[Example #1]
	//  // Clearing Game Unit slots using [Overloading #1] for passing enum values
	//  // range in form suitable for Game Unit API (as "TArray")
	// 
	//  enum class EUnitSlot
	//  {
	//      Firegun,
	//      Flack,
	//      RocketLauncher,
	//      Lazer,
	//      ImpluseGun,
	//      Plazma,
	//      LAST
	//  };
	//  
	//  bool IsTear1Slot(const EUnitSlot Slot)
	//  {
	//      switch (Slot)
	//      {
	//          case EUnitSlot::Firegun:
	//          case EUnitSlot::Flack:
	//          case EUnitSlot::Lazer:
	//              return true;
	//          default:
	//              return false;
	//      }
	//  }
	// 
	//  AGameUnit* GameUnit = GetGameUnitInHangar();
	//  
	//  auto Tear1SlotsRange =
	//      EnumValuesRange(EUnitSlot::Firegun, EUnitSlot::LAST) |
	//      FilteredRange(&IsTear1Slot);
	// 
	//  // In this example we assume that "AGameUnit" Actor has existing method
	//  // "AGameUnit::ClearSlots(const TArray<EUnitSlot>& Slots)"
	//  GameUnit->ClearSlots(RangeToArray(Tear1SlotsRange));
	// 
	//  //.[Example #2]
	//  // Parsing of IP to pass it to external function using [Overloading #2]
	//  // with explicit allocator pass
	// 
	//  // Function for string to number parsing
	//  template<typename ResultType>
	//  TOptional<ResultType> ParseNumber(const FString& String)
	//  {
	//     // ... some implementation
	//  }
	// 
	//  // Some existing API from other Module that cannot be changed
	//  extern void ConnectToIP(const TArray<uint8, TInlineAllocator<4>>& IP);
	// 
	//  const FString SupervisorServerIPAddress = "255.67.14.1";
	// 
	//  TArray<FString> NumbersAsStr;
	//  SupervisorServerIPAddress.ParseIntoArray(Numbers, ".");
	//  
	//  // Assumes that input IP is valid with checking by "GetValue()" call
	//  auto IP = RangeToArray<TInlineAllocator<4>>(
	//      NumbersAsStr |
	//      TransforedRange(&ParseNumber<uint8>) |
	//      RangeTransformedBy(&TOptional<uint8>::GetValue));
	// 
	//  ConnectToIP(IP);
	//
	//[Overloading #1]
	template<typename RangeType>
	auto RangeToArray(const RangeType& Range);

	//[Overloading #2]
	template<typename ArrayAllocatorType, typename RangeType>
	auto RangeToArray(const RangeType& Range);

	// ====================================================================
	// ====================== Common overloadings =========================
	// ====================================================================

	//! NB: Read about Common Overloadings at the end of the
	// "UnrealRanges_Common_Algorithm.h" file

	// -------------------------[ RANGE == TArray ]-------------------------------

	template <typename ValueType, typename AllocatorType>
	const ValueType& GetByIndex(const TArray<ValueType, AllocatorType>& Range, const unsigned int Index);

	template <typename ValueType, typename AllocatorType>
	UnrealRanges::Utils::TUnrealRanges_Optional<ValueType> GetByIndexSafe(
		const TArray<ValueType, AllocatorType>& Range, const unsigned int Index);

	template <typename ValueType, typename AllocatorType>
	unsigned int GetRangeCount(const TArray<ValueType, AllocatorType>& Range);

	template <typename ValueType, typename AllocatorType>
	bool RangeHasElements(const TArray<ValueType, AllocatorType>& Range);

	template <typename ValueType, typename AllocatorType>
	auto RangeFirstElement(const TArray<ValueType, AllocatorType>& Range);

	template <typename ValueType, typename AllocatorType>
	auto RangeLastElement(const TArray<ValueType, AllocatorType>& Range);

	// -------------------------[ RANGE == TSet ]-------------------------------

	template<typename InElementType, typename KeyFuncs, typename Allocator>
	unsigned int GetRangeCount(const TSet<InElementType, KeyFuncs, Allocator>& Range);

	template<typename InElementType, typename KeyFuncs, typename Allocator>
	bool RangeHasElements(const TSet<InElementType, KeyFuncs, Allocator>& Range);

	// -------------------------[ RANGE == TMap ]-------------------------------

	template<typename InKeyType, typename InValueType, typename SetAllocator, typename KeyFuncs>
	unsigned int GetRangeCount(const TMap<InKeyType, InValueType, SetAllocator, KeyFuncs>& Range);

	template<typename InKeyType, typename InValueType, typename SetAllocator, typename KeyFuncs>
	bool RangeHasElements(const TMap<InKeyType, InValueType, SetAllocator, KeyFuncs>& Range);

}} //namespace UnrealRanges::Algorithm

#include "Private/UnrealRanges_Impl_Unreal_Algorithm.inl"
