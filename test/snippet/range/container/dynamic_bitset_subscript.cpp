#include <seqan3/core/debug_stream.hpp>
#include <seqan3/range/container/dynamic_bitset.hpp>

int main()
{
    seqan3::dynamic_bitset t1{0b1011'1000'1111};

    // Positions are 0-based and start at the rightmost bit.
    for (size_t i = 0; i < t1.size(); ++i)
        seqan3::debug_stream << t1[i]; // 111100011101

    seqan3::debug_stream << '\n';
}
