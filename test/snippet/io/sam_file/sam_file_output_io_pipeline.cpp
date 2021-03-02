#include <sstream>

#include <seqan3/io/alignment_file/all.hpp>
#include <seqan3/range/views/persist.hpp>
#include <seqan3/std/ranges>

auto sam_file_raw = R"(@HD	VN:1.6	SO:coordinate	GO:none
@SQ	SN:ref	LN:45
r001	99	ref	7	30	*	=	37	39	TTAGATAAAGGATACTG	*
r003	0	ref	29	30	*	*	0	0	GCCTAAGCTAA	*	SA:Z:ref,29,-,6H5M,17,0;
r003	2064	ref	29	17	*	*	0	0	TAGGC	*	SA:Z:ref,9,+,5S6M,30,1;
r001	147	ref	237	30	*	=	7	-39	CAGCGGCAT	*	NM:i:1
)";

int main()
{
#if !SEQAN3_WORKAROUND_GCC_93983
    seqan3::sam_file_input{std::istringstream{sam_file_raw}, seqan3::format_sam{}}
        | seqan3::views::persist
        | std::views::take(3) // take only the first 3 records
        | seqan3::alignment_file_output{std::ostringstream{}, seqan3::format_sam{}};
#endif // !SEQAN3_WORKAROUND_GCC_93983
}