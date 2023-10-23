#pragma once

namespace UnrealRanges{ namespace Utils{

	// =============================================================
	// ========================== Logic ============================
	// =============================================================

	namespace Logic
	{
		// ------------ Logic::ELogicInvertion -----------
		//
		// Enum for setting up inversion logic.
		// 
		// NB: Used not "class" enum for more compact usage.
		// Most comfortable to use with "using namespace Logic"
		// in scope where inverted logic is needed
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
		//  // With "using" passing inversion argument looks
		//  // descritable, just like human readable sentance:
		//  // "Units ... Range FilteredBy ... not ... dead
		//  
		//  using namespace UnrealRanges::Utils::Logic;
		//  
		//  auto AliveUnits =
		//      GetUnitsRange() |
		//      RangeFilteredBy<Not>(&FUnit::IsDead);
		//
		enum ELogicInvertion
		{
			NotInverted,
			Not
		};
	}

	// =============================================================
	// ====================== Set operations =======================
	// =============================================================

	namespace SetOperation {

		enum EOperationAB
		{
			A_B = 0x1 << 0,
			B_A = 0x1 << 1,
			AnB = 0x1 << 2
		};

	} //namespace ::Utils::Set

	// =============================================================
	// ====================== Set operations =======================
	// =============================================================

	enum ESafeness : size_t
	{
		Safe = 0,
		Fast = 1
	};

	template<ESafeness Safeness>
	constexpr void CheckSupportedSafeness()
	{
		static_assert(Safeness == Safe || Safeness == Fast, "Unsupported ESafeness value passed");
	}

}}//UnrealRanges::Utils
