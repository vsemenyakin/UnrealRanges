#pragma once

namespace UnrealRanges{ namespace Utils{

	template<ESafeness Safeness, typename ValueType>
	ValueType* ToPtr(const TOptional<TSoftObjectPtr<ValueType>>& Optional)
	{
		UnrealRanges::Utils::CheckSupportedSafeness<Safeness>();

		const bool bSet = UnrealRanges::Utils::IsOptionalSet(Optional);

		switch (Safeness)
		{
			case UnrealRanges::Utils::Safe:
				if (!bSet)
				{
					return { };
				}

				break;

			case UnrealRanges::Utils::Fast:
				UnrealRanges::Utils::Check(bSet);
				break;
		}

		const TSoftObjectPtr<ValueType>& WrappedSoftPtr = UnrealRanges::Utils::GetOptionalValue(Optional);
		return WrappedSoftPtr.Get();

	}

	template<typename ValueType>
	ValueType* ToPtr(const TOptional<TSoftObjectPtr<ValueType>>& Optional)
	{
		return ToPtr<Safe, ValueType>(Optional);
	}

	template<ESafeness Safeness, typename ValueType>
	TSubclassOf<ValueType> ToPtr(const TOptional<TSoftClassPtr<ValueType>>& Optional)
	{
		UnrealRanges::Utils::CheckSupportedSafeness<Safeness>();

		const bool bSet = UnrealRanges::Utils::IsOptionalSet(Optional);

		switch (Safeness)
		{
			case UnrealRanges::Utils::Safe:
				if (!bSet)
				{
					return { };
				}

				break;

			case UnrealRanges::Utils::Fast:
				UnrealRanges::Utils::Check(bSet);
				break;
		}

		const TSoftClassPtr<ValueType>& WrappedSoftPtr = UnrealRanges::Utils::GetOptionalValue(Optional);
		return WrappedSoftPtr.Get();
	}

	template<typename ValueType>
	TSubclassOf<ValueType> ToPtr(const TOptional<TSoftClassPtr<ValueType>>& Optional)
	{
		return ToPtr<Safe, ValueType>(Optional);
	}

}} //namespace Utils::UnrealRanges
