#pragma once

#include "UnrealRanges_Utils_Types.h"

namespace std
{
	template<typename Type> class weak_ptr;
	template<typename Type> class shared_ptr;
}

//TODO: Move to separate utility file
//{
#define PriorityArgs_4 0, 0, 0

#define Prioriy_1 int, int, int
#define Prioriy_2 int, int, ...
#define Prioriy_3 int, ...
#define Prioriy_4 ...
//}

namespace UnrealRanges{ namespace Utils{

	//-------------------------[1: Raw pointer]-------------------------

	template<typename ToType, ESafeness CastType, typename FromType>
	const ToType* UCast(const FromType* From);

	template<typename ToType, ESafeness CastType, typename FromType>
	ToType* UCast(FromType* From);

	//-----------------------[2: Shared pointer]------------------------

	template<typename ToType, ESafeness CastType, typename FromType>
	std::shared_ptr<ToType> UCast(const std::shared_ptr<FromType>& From);

	//------------------------[3: Weak pointer]-------------------------

	template<typename ToType, ESafeness CastType, typename FromType>
	std::weak_ptr<ToType> UCast(const std::weak_ptr<FromType>& From);

	namespace Overloadable{

		//--------------[Prioritized Raw pointers cast]-----------------

		template<typename ToType, ESafeness CastType, typename FromType>
		const ToType* UCast(const FromType* From, Prioriy_4);

		template<typename ToType, ESafeness CastType, typename FromType>
		ToType* UCast(FromType* From, Prioriy_4);

	} //UnrealRanges::Utils::Overloadable

}}//namespace UnrealRanges::Utils

#include "Private/UnrealRanges_Impl_Utils_UCast.inl"
