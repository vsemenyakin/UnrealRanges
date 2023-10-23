#pragma once

namespace UnrealRanges{ namespace Utils{

	//------------------------------Common------------------------------

	namespace TemplateImpl{ namespace UniversalCast{

		template<ESafeness CastType, typename FromType, typename ToType>
		void CheckCastSupported()
		{
			UnrealRanges::Utils::CheckSupportedSafeness<CastType>();
			static_assert(
				std::is_class<FromType>::value ||
				std::is_class<ToType>::value, "Currently only object pointers may be used");
		}

	}}//TemplateImpl::UniversalCast

	//-------------------------[1: Raw pointer]--------------------------

	namespace Overloadable{

		namespace TemplateImpl{ namespace UniversalCast{

			template<ESafeness CastType, typename FromPtrType, typename ToPtrType>
			struct RawPointer;

			template<typename FromPtrType, typename ToPtrType>
			struct RawPointer<Fast, FromPtrType, ToPtrType>
			{
				static ToPtrType _(FromPtrType FromPtr){ return static_cast<ToPtrType>(FromPtr); }
			};

			template<typename FromPtrType, typename ToPtrType>
			struct RawPointer<Safe, FromPtrType, ToPtrType>
			{
				static ToPtrType _(FromPtrType FromPtr){ return dynamic_cast<ToPtrType>(FromPtr);  }
			};

		}}//TemplateImpl::UniversalCast

		template<typename ToType, ESafeness CastType, typename FromType>
		const ToType* UCast(const FromType* From, Prioriy_4)
		{
			UnrealRanges::Utils::TemplateImpl::UniversalCast::CheckCastSupported<CastType, FromType, ToType>();

			return TemplateImpl::UniversalCast::RawPointer<CastType, const FromType*, const ToType*>::_(From);
		}

		template<typename ToType, ESafeness CastType, typename FromType>
		ToType* UCast(FromType* From, Prioriy_4)
		{
			UnrealRanges::Utils::TemplateImpl::UniversalCast::CheckCastSupported<CastType, FromType, ToType>();

			return TemplateImpl::UniversalCast::RawPointer<CastType, FromType*, ToType*>::_(From);
		}
	}

	template<typename ToType, ESafeness CastType, typename FromType>
	const ToType* UCast(const FromType* From)
	{
		return Overloadable::UCast<ToType, CastType>(From, PriorityArgs_4);
	}

	template<typename ToType, ESafeness CastType, typename FromType>
	ToType* UCast(FromType* From)
	{
		return Overloadable::UCast<ToType, CastType>(From, PriorityArgs_4);
	}

	//-----------------------[2: Shared pointer]-------------------------

	namespace TemplateImpl{ namespace UniversalCast{

		template<ESafeness CastType, typename FromType, typename ToType>
		struct SharedPointer;

		template<typename FromType, typename ToType>
		struct SharedPointer<Fast, FromType, ToType>
		{
			static std::shared_ptr<ToType> _(const std::shared_ptr<FromType>& FromPtr) { return std::static_pointer_cast<ToType>(FromPtr); }
		};

		template<typename FromType, typename ToType>
		struct SharedPointer<Safe, FromType, ToType>
		{
			static std::shared_ptr<ToType> _(const std::shared_ptr<FromType>& FromPtr){ return std::dynamic_pointer_cast<ToType>(FromPtr);  }
		};

	}}//TemplateImpl::UniversalCast

	template<typename ToType, ESafeness CastType, typename FromType>
	std::shared_ptr<ToType> UCast(const std::shared_ptr<FromType>& From)
	{
		UnrealRanges::Utils::TemplateImpl::UniversalCast::CheckCastSupported<CastType, FromType, ToType>();
		
		return TemplateImpl::UniversalCast::SharedPointer<CastType, FromType, ToType>::_(From);
	}

	//-------------------------[3: Weak pointer]-------------------------

	template<typename ToType, ESafeness CastType, typename FromType>
	std::weak_ptr<ToType> UCast(const std::weak_ptr<FromType>& From)
	{
		//Uses "shared_ptr" casting of the pointer by "lock()"
		return TemplateImpl::UniversalCast::SharedPointer<CastType, FromType, ToType>::_(From.lock());
	}


}}//namespace UnrealRanges::Utils
