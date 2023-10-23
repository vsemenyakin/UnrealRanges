#pragma once

//For:
// - "Logic" functionality
#include "../UnrealRanges_Utils.h"

//For:
// - "Private::Utils::RangeIteratorType<ParentRangeType>"
// - "Private::Utils::RangeElementType<ParentRangeType>"
// - "Private::Utils::RangeIteratorType<ParentRangeType>"
// - "Capturing" functionality
#include "UnrealRanges_Private_Utils.h"

#include <utility>//for std::forward<>
#include <tuple>//for ranges settings
#include <type_traits>//for std::enable_if

namespace UnrealRanges {

	// ===============================| Basic ranges |=======================================

	template<class ParentRangeCaptureType, template<class, class> class Logic, class SettingsType>
	class TBasicRange
	{
	public:
		using ParentRangeType = UnrealRanges::Private::Utils::CapturedValueType<ParentRangeCaptureType>;
		using ParentRangeIteratorType = UnrealRanges::Private::Utils::RangeIteratorType<ParentRangeType>;
		using ValueType = UnrealRanges::Private::Utils::IteratorValueType<ParentRangeIteratorType>;
		using LogicInstanceType = Logic<ParentRangeIteratorType, SettingsType>;

		class Iterator
		{
		public:

			Iterator(
				ParentRangeIteratorType&& InCurrent, ParentRangeIteratorType&& InEnd,
				const SettingsType& InSettings)
				:
				Current(std::move(InCurrent)), End(std::move(InEnd)),
				Settings(InSettings)
			{
				LogicInstanceType::Ignore(Current, End, Settings);
			}

			auto operator*() const
			{
				UnrealRanges::Utils::Check(Current != End);
				return LogicInstanceType::Get(Current, Settings);
			}

			void operator++()
			{
				UnrealRanges::Utils::Check(Current != End);
				++Current;
				LogicInstanceType::Ignore(Current, End, Settings);
			}

			bool operator!=(const Iterator& Other) const
			{
				return (Current != Other.Current);
			}

		private:
			ParentRangeIteratorType Current;
			ParentRangeIteratorType End;
			SettingsType Settings;
		};

		//NB: "SettingsTypeFF" - to cause deducation and make possible to use Fast Forward
		template<typename SettingsTypeFF>
		TBasicRange(
			ParentRangeCaptureType&& InRangeCapture,
			SettingsTypeFF&& InSettings)
			:
			ParentRangeCapture(std::move(InRangeCapture)),
			Settings(std::forward<SettingsTypeFF>(InSettings))
		{
			UnrealRanges::Private::Utils::CheckForwardWorkaround<SettingsTypeFF, SettingsType>();
		}

		Iterator begin() const
		{
			return
			{
				UnrealRanges::Utils::RangeBegin(GetParentRange()),
				UnrealRanges::Utils::RangeEnd(GetParentRange()),
				Settings
			};
		}

		Iterator end() const
		{
			return
			{
				UnrealRanges::Utils::RangeEnd(GetParentRange()),
				UnrealRanges::Utils::RangeEnd(GetParentRange()),
				Settings
			};
		}

		template<typename PassedDownParentRangeType>
		auto Next(PassedDownParentRangeType&& ParentRangeCapture) const
		{
			UnrealRanges::Private::Utils::CheckPassedAsArgCaptureType<decltype(ParentRangeCapture)>();

			auto NextCapture = Capture(
				GetParentRange().Next(std::forward<PassedDownParentRangeType>(ParentRangeCapture))
			);

			return TBasicRange<decltype(NextCapture), Logic, SettingsType> { std::move(NextCapture), Settings };
		}

	private:
		
		const ParentRangeType& GetParentRange() const
		{
			return ParentRangeCapture.Get();
		}

		ParentRangeCaptureType ParentRangeCapture;
		SettingsType Settings;
	};

	// - - - - - Dummy

	template<template<class, class> class Logic, class SettingsType>
	class TBasicRange<UnrealRanges::Private::Utils::DummyType, Logic, SettingsType>
	{
	public:
		TBasicRange(const SettingsType& InSettings)
			:
			Settings(InSettings) { }

		template<typename PassedDownParentRangeType>
		auto Next(PassedDownParentRangeType&& ParentRangeCapture) const
		{
			UnrealRanges::Private::Utils::CheckPassedAsArgCaptureType<decltype(ParentRangeCapture)>();

			return TBasicRange<PassedDownParentRangeType, Logic, SettingsType>
			{
				std::forward<PassedDownParentRangeType>(ParentRangeCapture),
				Settings
			};
		}

		//Next functions - to make depended code happy when compiling intermediate Dummy types.
		// "for loop" should be never called for Dummies
		int* begin() const { return nullptr; }
		int* end() const { return nullptr; }

	private:
		SettingsType Settings;
	};

	// ===============================| Filtered range |=======================================

	// ---------------------------- TFilteredRange<> class ------------------------------------

	template<typename ParentIteratorType, typename PassOnTrueFilterPredicateType>
	struct TFilterRangeLogic
	{
		using SettingType = PassOnTrueFilterPredicateType;
		using ValueType = UnrealRanges::Private::Utils::IteratorValueType<ParentIteratorType>;

		static ValueType Get(const ParentIteratorType& Current, const SettingType& Setting)
		{
			return *Current;
		}

		static void Ignore(ParentIteratorType& Current, const ParentIteratorType& End, const SettingType& Setting)
		{
			const PassOnTrueFilterPredicateType& IsPassFunc = Setting;
			for (; Current != End && !IsPassFunc(*Current); ++Current);
		}
	};

	// ---------------- FilteredRange<LogicInversion, ...>(PassOnTrueFilter) ------------------

	template<UnrealRanges::Utils::Logic::ELogicInvertion LogicInversion, typename PassOnTrueFilterPredicateType>
	auto FilteredRange(PassOnTrueFilterPredicateType&& PassOnTrueFilter)
	{
		auto Predicate = UnrealRanges::Utils::MakeFunctorWithInvertionCondition_OneArg<LogicInversion>(
			std::forward<PassOnTrueFilterPredicateType>(PassOnTrueFilter)
		);
		return TBasicRange<UnrealRanges::Private::Utils::DummyType, TFilterRangeLogic, decltype(Predicate)>{ std::move(Predicate) };
	}

	template<typename PassOnTrueFilterPredicateType>
	auto FilteredRange(PassOnTrueFilterPredicateType&& PassOnTrueFilter)
	{
		return FilteredRange<UnrealRanges::Utils::Logic::NotInverted>(
			std::forward<PassOnTrueFilterPredicateType>(PassOnTrueFilter)
		);
	}

	// ==============================| Transformed range |=====================================

	// ------------------------------ TTransformRangeLogic ------------------------------------

	template<typename ParentIteratorType, typename TransformFuncType>
	struct TTransformRangeLogic
	{
		using SettingType = TransformFuncType;

		static auto Get(const ParentIteratorType& Current, const SettingType& Setting)
		{
			const TransformFuncType& TransformFunc = Setting;
			return TransformFunc(*Current);
		}

		static void Ignore(ParentIteratorType& Current, const ParentIteratorType& End, const SettingType& Setting)
		{
		}
	};

	// -------------------- TransformedRange<...>(TransformFuncType) --------------------------

	template<typename TransformFuncType>
	auto TransformedRange(TransformFuncType&& TransformFunc)
	{
		using DecayedTransformFuncType = std::decay_t<TransformFuncType>;
		return TBasicRange<UnrealRanges::Private::Utils::DummyType, TTransformRangeLogic, DecayedTransformFuncType>
		{
			std::forward<TransformFuncType>(TransformFunc)
		};
	}

	// ======================| Filtered and transformed range |================================

	// -------------------- TFilteredAndTransformedRange<> class ------------------------------

	template<typename ParentIteratorType, typename PassOnTrueFilterAndTransformPredicateType>
	struct TFilteredAndTransformedRangeLogic
	{
		using SettingType = PassOnTrueFilterAndTransformPredicateType;
		using IteratorValueType = UnrealRanges::Private::Utils::IteratorValueType<ParentIteratorType>;

		static auto Get(const ParentIteratorType& Current, const SettingType& Setting)
		{
			UnrealRanges::Utils::Check(IsPass(Current, Setting));

			const PassOnTrueFilterAndTransformPredicateType& PassOnTrueFilterAndTransformPredicateFunc = Setting;
			return UnrealRanges::Utils::GetOptionalValue(PassOnTrueFilterAndTransformPredicateFunc(*Current));
		}

		static void Ignore(ParentIteratorType& Current, const ParentIteratorType& End, const SettingType& Setting)
		{
			for (; Current != End && !IsPass(Current, Setting); ++Current);
		}

	private:
		static bool IsPass(const ParentIteratorType& Current, const SettingType& Setting)
		{
			const PassOnTrueFilterAndTransformPredicateType& PassOnTrueFilterAndTransformPredicateFunc = Setting;
			return UnrealRanges::Utils::IsOptionalSet(PassOnTrueFilterAndTransformPredicateFunc(*Current));
		}
	};

	// -------------- FilteredAndTransformedRange<...>(TransformFuncType) ---------------------

	template<typename PassOnTrueFilterAndTransformPredicateType>
	auto FilteredAndTransformedRange(PassOnTrueFilterAndTransformPredicateType&& PassOnTrueFilterAndTransform)
	{
		using DecayedFilterAndTransform = std::decay_t<PassOnTrueFilterAndTransformPredicateType>;
		return TBasicRange<UnrealRanges::Private::Utils::DummyType, TFilteredAndTransformedRangeLogic, DecayedFilterAndTransform>
		{
			std::forward<PassOnTrueFilterAndTransformPredicateType>(PassOnTrueFilterAndTransform)
		};
	}

	// ================================| Reverse range |=======================================

	template<typename ParentRangeCaptureType>
	class TReverseRange
	{
	public:
		using ParentRangeType = UnrealRanges::Private::Utils::CapturedValueType<ParentRangeCaptureType>;
		using ParentRangeIteratorType = Private::Utils::RangeIteratorType<ParentRangeType>;
		using ValueType = Private::Utils::RangeElementType<ParentRangeType>;

		class Iterator
		{
		public:

			Iterator(const ParentRangeIteratorType& InCurrent)
				: Current(InCurrent), Prev(Current)
			{
				--Prev;
			}

			auto operator*() const
			{
				return *Prev;
			}

			void operator++()
			{
				//NB: Force copy is used to make possible assigning of third party range iterator
				Private::Utils::ForceCopy(Current, Prev);
				--Prev;
			}

			bool operator!=(const Iterator& Other) const
			{
				return (Current != Other.Current);
			}

		private:

			ParentRangeIteratorType Current;
			ParentRangeIteratorType Prev;
		};

		TReverseRange(ParentRangeCaptureType&& InRangeCapture)
			: ParentRangeCapture(std::move(InRangeCapture)) { }

		Iterator begin() const
		{
			return { RangeEnd(GetRange()) };
		}

		Iterator end() const
		{
			return { RangeBegin(GetRange()) };
		}

		template<typename PassedDownParentRangeType>
		auto Next(PassedDownParentRangeType&& ParentRangeCapture) const
		{
			UnrealRanges::Private::Utils::CheckPassedAsArgCaptureType<decltype(ParentRangeCapture)>();

			auto NextCapture = Capture(
				GetRange().Next(std::forward<PassedDownParentRangeType>(ParentRangeCapture))
			);

			return TReverseRange<decltype(NextCapture)>{ std::move(NextCapture) };
		}

	private:
		const ParentRangeType& GetRange() const
		{
			return ParentRangeCapture.Get();
		}

		ParentRangeCaptureType ParentRangeCapture;
	};

	// - - - - - Dummy

	template<>
	class TReverseRange<Private::Utils::DummyType>
	{
	public:
		//See details about this begin-end in implementation of
		// "TFilteredRange<DummyType, PassOnTrueFilterPredicateType>"
		int* begin() const { return nullptr; }
		int* end() const { return nullptr; }

		template<typename PassedDownParentRangeType>
		auto Next(PassedDownParentRangeType&& ParentRangeCapture) const
		{
			UnrealRanges::Private::Utils::CheckPassedAsArgCaptureType<decltype(ParentRangeCapture)>();

			return TReverseRange<PassedDownParentRangeType>
			{
				std::forward<PassedDownParentRangeType>(ParentRangeCapture)
			};
		}
	};

	// --------------------------------- ReverseRange() -----------------------------------------

	inline auto ReverseRange()
	{
		return TReverseRange<Private::Utils::DummyType>{ };
	}

	// ================================| Step into range |=======================================

	template<class ParentRangeCaptureType, class StepIntoFuncType>
	class TStepIntoRange
	{
	public:
		using ParentRangeType = UnrealRanges::Private::Utils::CapturedValueType<ParentRangeCaptureType>;
		using ParentRangeIteratorType = UnrealRanges::Private::Utils::RangeIteratorType<ParentRangeType>;
		using ParentRangeValueType = UnrealRanges::Private::Utils::IteratorValueType<ParentRangeIteratorType>;

		using InnerRangeDirectAccessType = decltype(std::declval<StepIntoFuncType>()(std::declval<ParentRangeValueType>()));
		using InnerRangeCaptureType = decltype(Private::Utils::Capture(std::declval<InnerRangeDirectAccessType>()));
		using InnerRangeType = UnrealRanges::Private::Utils::CapturedValueType<InnerRangeCaptureType>;
		using InnerRangeIteratorType = UnrealRanges::Private::Utils::RangeIteratorType<InnerRangeType>;
		using InnerRangeValueType = UnrealRanges::Private::Utils::IteratorValueType<InnerRangeIteratorType>;

		class Iterator
		{
		public:

			Iterator(
				ParentRangeIteratorType&& InCurrent, ParentRangeIteratorType&& InEnd,
				const StepIntoFuncType& InStepIntoFunc)
				:
				Current(std::move(InCurrent)), End(std::move(InEnd)),
				StepIntoFunc(InStepIntoFunc)
			{
				if (!IsEnd())
				{
					StepIntoUntilSuccess();
				}
			}

			InnerRangeValueType operator*() const
			{
				UnrealRanges::Utils::Check(!IsEnd() && !IsInnerEnd());

				return *(InnerState->Current);
			}

			void operator++()
			{
				UnrealRanges::Utils::Check(!IsEnd() && !IsInnerEnd());
				
				NextInner();
				
				if (IsInnerEnd())
				{
					Next();

					if (!IsEnd())
					{
						StepIntoUntilSuccess();
					}
				}
			}

			bool operator!=(const Iterator& Other) const
			{
				//NB: No sense to
				// Logic may be used only for whole range iterations!
				return (Current != Other.Current);
			}

		private:

			bool IsEnd() const
			{
				return !(Current != End);
			}

			bool IsInnerEnd() const
			{
				return !(InnerState->Current != InnerState->End);
			}

			void StepIntoUntilSuccess()
			{
				UnrealRanges::Utils::Check(!IsEnd());

				for (StepInto(); !IsEnd() && IsInnerEnd(); Next(), StepInto());
			}

			void StepInto()
			{
				UnrealRanges::Utils::Check(!IsEnd());

				InnerState.Reset(StepIntoFunc(*Current));
			}

			void Next()
			{
				UnrealRanges::Utils::Check(!IsEnd());

				++Current;
			}

			void NextInner()
			{
				UnrealRanges::Utils::Check(!IsEnd() && !IsInnerEnd());

				++(InnerState->Current);
			}

			ParentRangeIteratorType Current;
			ParentRangeIteratorType End;
			StepIntoFuncType StepIntoFunc;

			struct FInnerState
			{
				//NB: Deducted template constructor is used for perfect forwarding here
				template<typename InnerRangeTypeFF>
				FInnerState(InnerRangeTypeFF&& InnerRange)
					:
					RangeCapture(std::forward<InnerRangeTypeFF>(InnerRange)),
					Current(RangeCapture.Get().begin()),
					End(RangeCapture.Get().end())
				{
					UnrealRanges::Private::Utils::CheckForwardWorkaround<InnerRangeTypeFF, InnerRangeType>();
				}

				InnerRangeCaptureType RangeCapture;
				InnerRangeIteratorType Current;
				InnerRangeIteratorType End;
			};

			UnrealRanges::Private::Utils::TOpaqueOptional<FInnerState> InnerState;
		};

		//NB: "StepIntoFuncTypeFF" - to cause deducation and make possible to use Fast Forward
		template<typename StepIntoFuncTypeFF>
		TStepIntoRange(
			ParentRangeCaptureType&& InRangeCapture,
			StepIntoFuncTypeFF&& InStepIntoFunc)
			:
			ParentRangeCapture(std::move(InRangeCapture)),
			StepIntoFunc(std::forward<StepIntoFuncTypeFF>(InStepIntoFunc))
		{
			UnrealRanges::Private::Utils::CheckForwardWorkaround<StepIntoFuncTypeFF, StepIntoFuncType>();
		}

		Iterator begin() const
		{
			return
			{
				UnrealRanges::Utils::RangeBegin(GetParentRange()),
				UnrealRanges::Utils::RangeEnd(GetParentRange()),
				StepIntoFunc
			};
		}

		Iterator end() const
		{
			return
			{
				UnrealRanges::Utils::RangeEnd(GetParentRange()),
				UnrealRanges::Utils::RangeEnd(GetParentRange()),
				StepIntoFunc
			};
		}

		template<typename NewParentRangeCaptureType>
		auto Next(NewParentRangeCaptureType&& NewParentRangeCapture)
		{
			UnrealRanges::Private::Utils::CheckPassedAsArgCaptureType<decltype(NewParentRangeCapture)>();

			auto NextCapture = Capture(
				GetParentRange().Next(std::forward<NewParentRangeCaptureType>(NewParentRangeCapture))
			);

			return TStepIntoRange<decltype(NextCapture), StepIntoFuncType>
			{
				std::move(NextCapture),
				StepIntoFunc
			};
		}

	private:
		
		const ParentRangeType& GetParentRange() const
		{
			return ParentRangeCapture.Get();
		}

		ParentRangeCaptureType ParentRangeCapture;
		StepIntoFuncType StepIntoFunc;
	};

	// - - - - - Dummy

	template<class StepIntoFuncType>
	class TStepIntoRange<UnrealRanges::Private::Utils::DummyType, StepIntoFuncType>
	{
	public:
		TStepIntoRange(const StepIntoFuncType& InStepIntoFunc)
			:
			StepIntoFunc(InStepIntoFunc) { }

		template<typename ParentRangeCaptureType>
		auto Next(ParentRangeCaptureType&& ParentRangeCapture) const
		{
			UnrealRanges::Private::Utils::CheckPassedAsArgCaptureType<decltype(ParentRangeCapture)>();

			return TStepIntoRange<ParentRangeCaptureType, StepIntoFuncType>
			{
				std::forward<ParentRangeCaptureType>(ParentRangeCapture),
				StepIntoFunc
			};
		}

		//Next functions - to make depended code happy when compiling intermediate Dummy types.
		// "for loop" should be never called for Dummies
		int* begin() const { return nullptr; }
		int* end() const { return nullptr; }

	private:
		StepIntoFuncType StepIntoFunc;
	};

	template<typename StepIntoFuncType>
	auto StepIntoRange(StepIntoFuncType&& StepIntoFunc)
	{
		return TStepIntoRange<Private::Utils::DummyType, std::decay_t<StepIntoFuncType>>
		{
			std::forward<StepIntoFuncType>(StepIntoFunc)
		};
	}

	// ================================| Recursive range |=======================================

	template<typename ValueType, typename StepTransformType>
	class TRecursiveRange
	{
	public:
		class PseudoIterator
		{
		public:

			PseudoIterator(const UnrealRanges::Utils::TUnrealRanges_Optional<ValueType>& InCurrent, const StepTransformType& InStepTransform)
				: Current(InCurrent), StepTransform(InStepTransform)
			{
			}

			auto operator*() const
			{
				UnrealRanges::Utils::Check(UnrealRanges::Utils::IsOptionalSet(Current));

				return UnrealRanges::Utils::GetOptionalValue(Current);
			}

			void operator++()
			{
				UnrealRanges::Utils::Check(UnrealRanges::Utils::IsOptionalSet(Current));

				Current = StepTransform(this->operator*());
			}

			bool operator!=(const PseudoIterator&) const
			{
				//NB: End-condition usage only
				return UnrealRanges::Utils::IsOptionalSet(Current);
			}

		private:
			UnrealRanges::Utils::TUnrealRanges_Optional<ValueType> Current;
			StepTransformType StepTransform;
		};

		//NB: "StepTransformTypeFF" - to cause deducation and make possible to use Fast Forward
		template<typename StepTransformTypeFF>
		TRecursiveRange(ValueType InRootValue, StepTransformTypeFF&& InStepTransform)
			: RootValue(InRootValue), StepTransform(std::forward<StepTransformTypeFF>(InStepTransform))
		{
			UnrealRanges::Private::Utils::CheckForwardWorkaround<StepTransformTypeFF, StepTransformType>();
		}

		PseudoIterator begin() const
		{
			return { UnrealRanges::Utils::MakeOptional(RootValue), StepTransform };
		}

		PseudoIterator end() const
		{
			return { UnrealRanges::Utils::MakeOptional<ValueType>(), StepTransform };
		}

	private:
		ValueType RootValue;
		StepTransformType StepTransform;
	};

	// -------------------------------- RecursiveRange() ----------------------------------------

	template<typename ValueType, typename StepTransformType>
	auto RecursiveRange(ValueType RootValue, StepTransformType&& StepTransform)
	{
		return TRecursiveRange<ValueType, std::decay_t<StepTransformType>>{ RootValue, std::forward<StepTransformType>(StepTransform) };
	}

	// ========================| Conditional iterator range |==================================

	// --------------------- TConditionalIteratorRange<> class --------------------------------

	template<typename ConditionalIteratorType>
	class TConditionalIteratorRange
	{
	public:
		using ValueType = UnrealRanges::Private::Utils::DereferencedType<ConditionalIteratorType>;

		//NB: This is NOT actualy iterator, it cannot be used as iterator.
		// Comparation operator is implemented to correspond range based cycles idiom.
		//
		// This is how Begin-End Based usage ranges looks:
		//
		//   for (Iter current = range.begin(); current != range.end(); ++current) { ... }
		//
		// And this is how Conditional Based iterators usage looks:
		//
		//   for (ConditionalIterator current = begin(); (bool)current; ++current) { ... } 
		//
		// Unfortunaly, we MAY HAVE NO WAY to get "end()" iterator for ConditionalIterator.
		// So, we should make some kind of workaround to make Conditional Based iterators
		// "behave like" Begin-End based.
		//
		// Iterators of Begin-End Based ranges uses "operator!=()" to check when cycle
		// should be finished, by checking "current != range.end()". It should return "false"
		// (saying "loop stop") when "current" is pointing to range end. For Conditional Based
		// iterators "loop stop" should be performed for condition "!current".
		// 
		// So, we may assume that "operator!=()" should just return "false" when
		// Conditional Based iterator wrapped by our Begin-End Based range adopter return "false"
		// on casting to boolean to stop the loop. Value passed as argument (for loop end checking)
		// will be just ignored in such usage.
		//
		class PseudoIterator
		{
		public:

			PseudoIterator(const ConditionalIteratorType& InCurrent)
				: Current(InCurrent) { }

			PseudoIterator()
				: Current() { }

			auto operator*() const
			{
				UnrealRanges::Utils::Check(!IsEnd());
				return *Current;
			}

			void operator++()
			{
				++Current;
			}

			//NB: DON'T use as actual iterator comparator!
			// Just check if "Current" is not end. See description above for details
			bool operator!=(const PseudoIterator&) const
			{
				return !IsEnd();
			}

		private:
			void Next()
			{
				UnrealRanges::Utils::Check(!IsEnd());
				++Current;
			}

			bool IsEnd() const
			{
				return !Current;
			}

			ConditionalIteratorType Current;
		};

		//NB: "ConditionalIteratorTypeFF" - to cause deducation and make possible to use Fast Forward
		template<typename ConditionalIteratorTypeFF>
		TConditionalIteratorRange(ConditionalIteratorTypeFF&& InBeginConditionalIterator)
			: BeginConditionalIterator(std::forward<ConditionalIteratorTypeFF>(InBeginConditionalIterator))
		{
			UnrealRanges::Private::Utils::CheckForwardWorkaround<ConditionalIteratorTypeFF, ConditionalIteratorType>();
		}

		PseudoIterator begin() const
		{
			return { BeginConditionalIterator };
		}

		//NB: Should be not used directly because comparation with "end()" is not used
		// at all in this range type. See description above for details
		PseudoIterator end() const
		{
			return { BeginConditionalIterator };
		}

	private:
		ConditionalIteratorType BeginConditionalIterator;
	};

	// --------------- ConditionalIteratorRange<...>(ConditionalIterator) ---------------------

	template<typename ConditionalIteratorType>
	auto ConditionalIteratorRange(ConditionalIteratorType&& ConditionalIterator)
	{
		return TConditionalIteratorRange<std::decay_t<ConditionalIteratorType>>
		{
			std::forward<ConditionalIteratorType>(ConditionalIterator)
		};
	}

	// ========================| Conditional iterator range |==================================

	// ------------------- EnumValuesRange ------------------

	template<typename EnumType>
	class TEnumConditionalIterator
	{
	public:
		TEnumConditionalIterator(const EnumType InStartValue, const EnumType InEndValue)
			: CurrentValue(InStartValue), EndValue(InEndValue) { }

		EnumType operator*() const
		{
			UnrealRanges::Utils::Check(!IsEnd());
			return CurrentValue;
		}

		void operator++()
		{
			UnrealRanges::Utils::Check(!IsEnd());
			CurrentValue = NextEnum(CurrentValue);
		}

		operator bool() const
		{
			return IsEnd();
		}

	private:
		bool IsEnd() const
		{
			return (CurrentValue != EndValue);
		}

		EnumType CurrentValue;
		const EnumType EndValue;
	};

	template<typename EnumType>
	auto EnumValuesRange(const EnumType StartValue, const EnumType EndValue)
	{
		using FEnumIteratorType = TEnumConditionalIterator<EnumType>;
		return ConditionalIteratorRange(FEnumIteratorType{ StartValue, EndValue });
	}

	// ===============================| Conditional range |========================================

	namespace TemplateDetails
	{
		namespace TConditionalRange
		{
			template<typename ParentRangeCaptureType, typename TargetRangeCaptureType>
			void CheckIsTypeConsistent()
			{
				using namespace Private::Utils;

				using ParentRangeElementType = RangeElementType<CapturedValueType<ParentRangeCaptureType>>;
				using TargetRangeElementType = RangeElementType<CapturedValueType<TargetRangeCaptureType>>;

				static_assert(IsSame<ParentRangeElementType, TargetRangeElementType>(), "Unconsistant types passed");
			}
		}
	};

	template<typename ParentRangeCaptureType, typename TargetRangeCaptureType>
	class TConditionalRange
	{
	public:
		using ParentRangeType = Private::Utils::CapturedValueType<ParentRangeCaptureType>;
		using TargetRangeType = Private::Utils::CapturedValueType<TargetRangeCaptureType>;
		using ParentRangeIteratorType = Private::Utils::RangeIteratorType<ParentRangeType>;
		using TargetRangeIteratorType = Private::Utils::RangeIteratorType<TargetRangeType>;
		using ValueType = Private::Utils::RangeElementType<ParentRangeType>;

		class Iterator
		{
		public:

			Iterator(ParentRangeIteratorType InParentCurrent, TargetRangeIteratorType InTargetCurrent, bool bInUseTarget)
				: bUseTarget(bInUseTarget), ParentCurrent(InParentCurrent), TargetCurrent(InTargetCurrent)
			{
			}

			auto operator*() const
			{
				return bUseTarget ? *TargetCurrent : *ParentCurrent;
			}

			void operator++()
			{
				if (bUseTarget)
				{
					++TargetCurrent;
				}
				else
				{
					++ParentCurrent;
				}
			}

			bool operator!=(const Iterator& Other) const
			{
				return bUseTarget ?
					(TargetCurrent != Other.TargetCurrent) :
					(ParentCurrent != Other.ParentCurrent);
			}

		private:

			bool bUseTarget;
			ParentRangeIteratorType ParentCurrent;
			TargetRangeIteratorType TargetCurrent;
		};

		TConditionalRange(ParentRangeCaptureType&& InParentRangeCapture, TargetRangeCaptureType&& InTargetRangeCapture, bool bInUseTarget) :
			bUseTarget(bInUseTarget),
			ParentRangeCapture(std::move(InParentRangeCapture)),
			TargetRangeCapture(std::move(InTargetRangeCapture)) { }

		Iterator begin() const
		{
			return {
				RangeBegin(GetParentRange()),
				RangeBegin(GetTargetRange()),
				bUseTarget
			};
		}

		Iterator end() const
		{
			return {
				RangeEnd(GetParentRange()),
				RangeEnd(GetTargetRange()),
				bUseTarget
			};
		}

		template<typename PassedDownParentRangeType>
		auto Next(PassedDownParentRangeType&& ParentRangeCapture) const
		{
			UnrealRanges::Private::Utils::CheckPassedAsArgCaptureType<decltype(ParentRangeCapture)>();

			//Convert capture "ParentRangeCapture" into Shared
			auto ParentRangeCaptureShared = std::move(ParentRangeCapture).ConvertIntoShared();

			//Pass copy of the Shared "ParentRangeCapture"
			auto ParentRangeCaptureSharedCopyForTargetRange = ParentRangeCaptureShared;
			auto NextTargetRangeCapture = Capture(
				GetTargetRange().Next(std::forward<PassedDownParentRangeType>(ParentRangeCaptureSharedCopyForTargetRange))
			);

			TemplateDetails::TConditionalRange::CheckIsTypeConsistent<decltype(ParentRangeCaptureShared), decltype(NextTargetRangeCapture)>();

			return TConditionalRange<decltype(ParentRangeCaptureShared), decltype(NextTargetRangeCapture)>
			{
				std::move(ParentRangeCaptureShared),
				std::move(NextTargetRangeCapture),
				bUseTarget
			};
		}

	private:
		const ParentRangeType& GetParentRange() const
		{
			return ParentRangeCapture.Get();
		}

		const TargetRangeType& GetTargetRange() const
		{
			return TargetRangeCapture.Get();
		}

		bool bUseTarget;
		ParentRangeCaptureType ParentRangeCapture;
		TargetRangeCaptureType TargetRangeCapture;
	};

	// - - - - - Dummy

	template<typename TargetRangeCaptureType>
	class TConditionalRange<Private::Utils::DummyType, TargetRangeCaptureType>
	{
	public:
		using TargetRangeType = Private::Utils::CapturedValueType<TargetRangeCaptureType>;

		int* begin() const { return nullptr; }
		int* end() const { return nullptr; }

		TConditionalRange(TargetRangeCaptureType&& InTargetRangeCapture, bool bInUseTarget) :
			bUseTarget(bInUseTarget),
			TargetRangeCapture(std::move(InTargetRangeCapture)) { }
		
		template<typename PassedDownParentRangeType>
		auto Next(PassedDownParentRangeType&& ParentRangeCapture) const
		{
			UnrealRanges::Private::Utils::CheckPassedAsArgCaptureType<decltype(ParentRangeCapture)>();

			//Convert capture "ParentRangeCapture" into Shared
			auto ParentRangeCaptureShared = std::move(ParentRangeCapture).ConvertIntoShared();

			//Pass copy of the Shared "ParentRangeCapture"
			auto ParentRangeCaptureSharedCopyForTargetRange = ParentRangeCaptureShared;
			auto NextTargetRangeCapture = Capture(
				GetTargetRange().Next(std::move(ParentRangeCaptureSharedCopyForTargetRange))
			);

			TemplateDetails::TConditionalRange::CheckIsTypeConsistent<decltype(ParentRangeCaptureShared), decltype(NextTargetRangeCapture)>();

			return TConditionalRange<decltype(ParentRangeCaptureShared), decltype(NextTargetRangeCapture)>
			{
				std::move(ParentRangeCaptureShared),
				std::move(NextTargetRangeCapture), 
				bUseTarget
			};
		}
	private:

		const TargetRangeType& GetTargetRange() const
		{
			return TargetRangeCapture.Get();
		}

		bool bUseTarget;
		TargetRangeCaptureType TargetRangeCapture;
	};

	// -------------------------------- ConditionalRange() ----------------------------------------

	template<typename TargetRangeType>
	auto ConditionalRange(TargetRangeType&& TargetRange, bool bUseTarget)
	{
		auto TargetRangeCapture = Private::Utils::Capture(std::forward<TargetRangeType>(TargetRange));
		return TConditionalRange<UnrealRanges::Private::Utils::DummyType, decltype(TargetRangeCapture)>
		{
			std::move(TargetRangeCapture), bUseTarget
		};
	}

	// ===============================| Set operation range |========================================

	//TODO: Support specializations for passed Operations
	//
	template<int Operation, typename RangeACaptureType, typename RangeBCaptureType>
	class TSetOperationRange
	{
	public:
		using RangeAType = Private::Utils::CapturedValueType<RangeACaptureType>;
		using RangeBType = Private::Utils::CapturedValueType<RangeBCaptureType>;

		using ValueAType = Private::Utils::RangeElementType<RangeAType>;
		using ValueBType = Private::Utils::RangeElementType<RangeBType>;
		static_assert(std::is_same<ValueAType, ValueBType>::value, "Passed Ranges should contains some type Elements");

		using ValueType = ValueAType;
		static_assert(Private::Utils::IsComparable<ValueType, ValueType>(), "Ranges elements should support \"{ElementType} == {ElementType}\"");

		using RangeAIteratorType = Private::Utils::RangeIteratorType<RangeAType>;
		using RangeBIteratorType = Private::Utils::RangeIteratorType<RangeBType>;
		
		class PseudoIterator
		{
		public:

			PseudoIterator(
				RangeAIteratorType&& InABegin, RangeAIteratorType&& InAEnd,
				RangeBIteratorType&& InBBegin, RangeBIteratorType&& InBEnd)
				:
				ABegin(InABegin), ACurrent(IsA_B() || IsAnB() ? InABegin : InAEnd), AEnd(InAEnd),
				BBegin(InBBegin), BCurrent(IsB_A() ? InBBegin : InBEnd), BEnd(InBEnd)
			{
				Ignore();
			}

			PseudoIterator()
				: Current() { }

			auto operator*() const
			{
				UnrealRanges::Utils::Check(!IsEnd());

				if (ACurrent != AEnd)
				{
					return *ACurrent;
				}
				else
				{
					return *BCurrent;
				}
			}

			void operator++()
			{
				UnrealRanges::Utils::Check(!IsEnd());

				if (ACurrent != AEnd)
				{
					++ACurrent;
					Ignore();
				}
				else
				{
					++BCurrent;
					Ignore();
				}
			}

			//NB: DON'T use as actual iterator comparator!
			// Just check if "Current" is not end. See description above for details
			bool operator!=(const PseudoIterator&) const
			{
				return !IsEnd();
			}

		private:

			bool IsEnd() const
			{
				return !(ACurrent != AEnd) && !(BCurrent != BEnd);
			}

			void Ignore()
			{
				auto IsRangeContainsValue = [](auto Begin, auto End, ValueType Value)
				{
					for (auto Current = Begin; Current != End; ++Current)
					{
						if (*Current == Value)
						{
							return true;
						}
					}
					
					return false;
				};

				//Constexpr check for switching out this branch on optimize builds
				if (IsAnB() || IsA_B())
				{
					for (; ACurrent != AEnd; ++ACurrent)
					{
						const bool bIntersected = IsRangeContainsValue(BBegin, BEnd, *ACurrent);

						if (IsAnB() && bIntersected)
						{
							//We want Itersection and we found Intersected value => Stop Ignoring!
							return;
						}
						
						if (IsA_B() && !bIntersected)
						{
							//We want A without B and we found A element not presented in B => Stop Ignoring!
							return;
						}
					}
				}

				//Constexpr check for switching out this branch on optimize builds
				// We should start ignoring B elements
				if (IsB_A() && !(ACurrent != AEnd))
				{
					for (; BCurrent != BEnd; ++BCurrent)
					{
						const bool bIntersected = IsRangeContainsValue(ABegin, AEnd, *BCurrent);

						if (!bIntersected)
						{
							//We process only "B_A" case here
							//We want B without A and we found B element not presented in A => Stop Ignoring!
							return;
						}
					}
				}
			}

			constexpr static bool IsA_B() { return (Operation & Utils::SetOperation::A_B) != 0; }
			constexpr static bool IsB_A() { return (Operation & Utils::SetOperation::B_A) != 0; }
			constexpr static bool IsAnB() { return (Operation & Utils::SetOperation::AnB) != 0; }

			RangeAIteratorType ABegin;
			RangeAIteratorType ACurrent;
			RangeAIteratorType AEnd;

			RangeBIteratorType BBegin;
			RangeBIteratorType BCurrent;
			RangeBIteratorType BEnd;
		};

		TSetOperationRange(RangeACaptureType&& RangeACapture, RangeBCaptureType&& RangeBCapture)
			: RangeACapture(std::move(RangeACapture)), RangeBCapture(std::move(RangeBCapture)) { }

		PseudoIterator begin() const
		{
			return {
				GetRangeA().begin(), GetRangeA().end(), 
				GetRangeB().begin(), GetRangeB().end()
			};
		}

		//NB: Should be not used directly because comparation with "end()" is not used
		// at all in this range type. See description above for details
		PseudoIterator end() const
		{
			return {
				GetRangeA().end(), GetRangeA().end(),
				GetRangeB().end(), GetRangeB().end()
			};
		}

	private:

		const RangeAType& GetRangeA() const
		{
			return RangeACapture.Get();
		}

		const RangeBType& GetRangeB() const
		{
			return RangeBCapture.Get();
		}

		RangeACaptureType RangeACapture;
		RangeBCaptureType RangeBCapture;
	};

	template<int Operation, typename RangeAType, typename RangeBType>
	auto SetOperationRange(RangeAType&& RangeA, RangeBType&& RangeB)
	{
		auto RangeACapture = Private::Utils::Capture(std::forward<RangeAType>(RangeA));
		auto RangeBCapture = Private::Utils::Capture(std::forward<RangeBType>(RangeB));

		return TSetOperationRange<Operation, decltype(RangeACapture), decltype(RangeBCapture)>
		{
			std::move(RangeACapture),
			std::move(RangeBCapture)
		};
	}

	// ===============================| Pipe operator |========================================

	template<typename ParentRangeType, typename DummyChildRangeType>
	auto operator | (ParentRangeType&& ParentRange, DummyChildRangeType&& ChildRangeDummy)->
		decltype(ChildRangeDummy.Next(Private::Utils::Capture(std::forward<ParentRangeType>(ParentRange))))
	{
		return ChildRangeDummy.Next(Private::Utils::Capture(std::forward<ParentRangeType>(ParentRange)));
	}

} // namespace UnrealRanges
