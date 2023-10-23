#pragma once

#include "../UnrealRanges_Utils.h"//for "RangeBegin<>()"

#include <type_traits>//for "std::decay_t<>"
#include <utility>//for "std::declval<>()"
#include <memory>//for "std::shared_ptr<>()"

namespace UnrealRanges{ namespace Private{ namespace Utils{

	// =================================| Useful templates |=================================

	template<typename RangeType>
	using RangeIteratorType = std::decay_t<
		decltype(
			UnrealRanges::Utils::RangeBegin(
				std::declval<const RangeType>()
			)
		)
	>;
	
	template<typename Type>
	using DereferencedType = std::decay_t<
		decltype(
			*std::declval<const Type>()
		)
	>;
	
	template<typename IteratorType>
	using IteratorValueType = std::decay_t<
		DereferencedType<
			IteratorType
		>
	>;

	template<typename RangeType>
	using RangeElementType = std::decay_t<
		DereferencedType<
			RangeIteratorType<RangeType>
		>
	>;

	// ==================================| Value capture |===================================

	template<typename ValueType>
	class TValueCapture_Ref
	{
	public:
		TValueCapture_Ref(ValueType& InValueRef) : Ref(InValueRef) { }
		TValueCapture_Ref(TValueCapture_Ref&&) = default; //Moving is ordinary use case
		
		//Copy is okay too - because "TValueCapture_Ref" is "shared" version for itself
		// (See comments for "ConvertIntoShared()" for details)
		TValueCapture_Ref(const TValueCapture_Ref&) = default;

		ValueType& Get() const { return Ref; }

		//NB: As "TValueCapture_Ref" is "reference wrapper" by it's nature,
		// it may be used as shared version of itself, no need to perform
		// full scale sharing.
		// Because of referencing nature of the "TValueCapture_Ref",
		// this method may be called for any referencing form of
		// the "TValueCapture_Ref": for value, rvalues and lvalues.
		TValueCapture_Ref ConvertIntoShared() const { return *this; }

	private:
		ValueType& Ref;
	};	

	template<typename ValueType>
	class TValueCapture_SharedCopy
	{
	public:
		//Shared Capture may be copy or moved, it is
		// main purpose of this kind of Captures
		TValueCapture_SharedCopy(TValueCapture_SharedCopy&&) = default;
		TValueCapture_SharedCopy(const TValueCapture_SharedCopy&) = default;

		const ValueType& Get() const { return *SharedPtr; }

		//NB: "ConvertIntoShared()" should be never called because SharedCopy
		// is always created ONLY from TValueCapture_Copy
		//TValueCapture_Ref ConvertIntoShared() const

	private:
		//NB: May be created only from "TValueCapture_Copy"
		template<typename ValueType_> friend class TValueCapture_Copy;
		
		TValueCapture_SharedCopy(ValueType&& InValue) :
			SharedPtr(std::make_shared<ValueType>(std::move(InValue))) { }

		std::shared_ptr<ValueType> SharedPtr;
	};

	template<typename ValueType>
	class TValueCapture_Copy
	{
		public:
			TValueCapture_Copy(ValueType&& InValue) : Copy(std::forward<ValueType>(InValue)) { }
			TValueCapture_Copy(TValueCapture_Copy&&) = default; //Moving is ordinary use case, but...
			TValueCapture_Copy(const TValueCapture_Copy&) = delete; //...copy should be never used

			const ValueType& Get() const { return Copy; }

			//May be called onlty
			TValueCapture_SharedCopy<ValueType> ConvertIntoShared() && { return std::move(Copy); }

		private:
			ValueType Copy;
	};

	// ----------------------------------------------------------------------------------

	template<typename ValueType>
	TValueCapture_Ref<ValueType> Capture(ValueType& InValueRef)
	{
		return TValueCapture_Ref<ValueType>{ InValueRef };
	}

	template<typename ValueType>
	TValueCapture_Copy<ValueType> Capture(ValueType&& InValue)
	{
		return TValueCapture_Copy<ValueType>{ std::forward<ValueType>(InValue) };
	}

	// ----------------------------------------------------------------------------------

	namespace TemplateDetails
	{
		namespace CapturedValueType
		{
			template<typename NotValueCapture> struct _                           {                  };
			template<typename Type>            struct _<TValueCapture_Ref<Type>>  { using __ = Type; };
			template<typename Type>            struct _<TValueCapture_Copy<Type>> { using __ = Type; };
		}
	}

	template<typename MaybeValueCapture>
	using CapturedValueType = typename TemplateDetails::CapturedValueType::_<std::decay_t<MaybeValueCapture>>::__;

	// =================| Optional for types without default cosntructor |===============

	template<typename ValueType>
	class TOpaqueOptional
	{
	public:
		TOpaqueOptional() = default;

		template<typename ... ConstructorArgTypes>
		TOpaqueOptional(ConstructorArgTypes&& ... ConstructorArgs)
		{
			Reset(std::forward<ConstructorArgTypes>(ConstructorArgs) ...);
		}

		TOpaqueOptional& operator=(const TOpaqueOptional& Other) = delete;
		TOpaqueOptional& operator=(TOpaqueOptional&& Other) = delete;

		template<typename ... ConstructorArgTypes>
		void Reset(ConstructorArgTypes&& ... ConstructorArgs)
		{
			ValuePtr = new (ValueMemoryPtr())ValueType{ std::forward<ConstructorArgTypes>(ConstructorArgs) ... };
		}

		bool IsSet() const
		{
			return (ValuePtr != nullptr);
		}

		const ValueType* operator->() const
		{
			UnrealRanges::Utils::Check(IsSet());

			return ObjectPtr();
		}

		ValueType* operator->()
		{
			const ValueType* ConstResult = const_cast<const TOpaqueOptional*>(this)->operator->();
			return const_cast<ValueType*>(ConstResult);
		}

		const ValueType& operator*() const
		{
			return *(this->operator->());
		}

		ValueType& operator*()
		{
			return *(this->operator->());
		}

		~TOpaqueOptional()
		{
			if (ValueType* ValuePtrTmp =  ObjectPtr())
			{
				ValuePtrTmp->~ValueType();
			}
		}

	private:
		using ByteType = unsigned char;
		
		const ValueType* ObjectPtr() const
		{
			return ValuePtr;
		}

		ValueType* ObjectPtr()
		{
			const ValueType* ConstResult = const_cast<const TOpaqueOptional*>(this)->ObjectPtr();
			return const_cast<ValueType*>(ConstResult);
		}

		ByteType* ValueMemoryPtr()
		{
			return ValueMemory;
		}

		alignas(ValueType) ByteType ValueMemory[sizeof(ValueType)];
		ValueType* ValuePtr = nullptr;
	};

	// =======================================| ForceCopy |========================================

	namespace TemplateDetails
	{
		namespace ForceCopy
		{
			inline void Dummy(){ }

			//NB: Decltype is used as "concept" for SFINAE check if assigning is possible for arguments,
			// Dummy - just to return "void" as result tyoe of "_<>()" call
			// Overloading with "..." is used to perform mem copy for all types that are not support
			// assigning operator

			template<typename ValueType>
			auto _(ValueType& ValueTo, const ValueType& ValueFrom, int)->decltype(ValueTo = ValueFrom, Dummy())
			{
				ValueTo = ValueFrom;
			}

			template<typename ValueType>
			void _(ValueType& ValueTo, const ValueType& ValueFrom, ...)
			{
				memcpy(&ValueTo, &ValueFrom, sizeof(ValueType));
			}
		}
	}

	template<typename ValueType>
	ValueType& ForceCopy(ValueType& ValueTo, const ValueType& ValueFrom)
	{
		TemplateDetails::ForceCopy::_(ValueTo, ValueFrom, 0);
		return ValueTo;
	}

	// =============================| Common type trait functions |=============================

	template<typename TypeA, typename TypeB>
	constexpr bool IsSame()
	{
		return std::is_same<TypeA, TypeB>::value;
	}

	template<typename Type>
	constexpr bool IsRRef()
	{
		return std::is_rvalue_reference<Type>::value;
	}

	namespace TemplatesImpl
	{
		namespace IsComparable
		{
			template<typename TypeA, typename TypeB>
			constexpr auto _(int)->decltype(std::declval<TypeA>() == std::declval<TypeB>(), true)
			{
				return true;
			}

			template<typename TypeA, typename TypeB>
			constexpr auto _(...)->decltype(false)
			{
				return false;
			}
		}
	}

	template<typename TypeA, typename TypeB>
	constexpr bool IsComparable()
	{
		return TemplatesImpl::IsComparable::_<TypeA, TypeB>(0);
	}

	// =============================| Common compile time checks |==============================

	template<typename TypeFF, typename Type>
	constexpr void CheckForwardWorkaround()
	{
		static_assert(IsSame<std::decay_t<TypeFF>, std::decay_t<Type>>(), "FF workaround error");
	}

	template<typename Type>
	constexpr void CheckPassedAsArgCaptureType()
	{
		static_assert(IsRRef<Type>(), "Captures should be ALWAYS passed as rvalue reference!");
	}

	// =================================| Utility types |=======================================

	class DummyType;

}}} //namespace UnrealRanges::Private::Utils
