
// =================== Universal Begin/End =====================

namespace UnrealRanges{ namespace Utils{

	namespace TemplateDetails {

		namespace RangeBegin
		{
			template<typename RangeType>
			auto _(const RangeType& Range, int)->decltype(Range.begin())
			{
				return Range.begin();
			}

			template<typename RangeType>
			auto _(const RangeType& Range, ...)->decltype(begin(Range))
			{
				return begin(Range);
			}
		}

		namespace RangeEnd
		{
			template<typename RangeType>
			auto _(const RangeType& Range, int)->decltype(Range.end())
			{
				return Range.end();
			}

			template<typename RangeType>
			auto _(const RangeType& Range, ...)->decltype(end(Range))
			{
				return end(Range);
			}
		}

	}

}} //namespace UnrealRanges::Utils
