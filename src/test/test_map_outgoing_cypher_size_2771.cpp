#include "test_map_outgoing_cypher_size_2771.h"

#include "map/map_networking_capacity.h"

#include <array>
#include <iostream>

auto runMapOutgoingCypherSize2771SelfTests() -> bool
{
    struct word_case_t
    {
        uint32 packetSize;
        uint32 expectedWords;
    };

    struct block_case_t
    {
        uint32 wordCount;
        uint32 expectedBlocks;
    };

    constexpr auto wordCases = std::array{
        word_case_t{ 0, 0 },
        word_case_t{ 3, 0 },
        word_case_t{ 4, 0 },
        word_case_t{ 7, 0 },
        word_case_t{ 8, 2 },
        word_case_t{ 11, 2 },
        word_case_t{ 12, 2 },
        word_case_t{ 15, 2 },
        word_case_t{ 16, 4 },
        word_case_t{ 20, 4 },
        word_case_t{ 24, 6 },
    };

    constexpr auto blockCases = std::array{
        block_case_t{ 0, 0 },
        block_case_t{ 2, 1 },
        block_case_t{ 4, 2 },
        block_case_t{ 6, 3 },
    };

    for (const auto& test : wordCases)
    {
        if (mapnetworkinghelpers::OutgoingCypherWordCount(test.packetSize) != test.expectedWords)
        {
            std::cerr << "map outgoing cypher size 2771 self-test failed: word count for packetSize="
                      << test.packetSize << '\n';
            return false;
        }
    }

    for (const auto& test : blockCases)
    {
        if (mapnetworkinghelpers::OutgoingCypherBlockCount(test.wordCount) != test.expectedBlocks)
        {
            std::cerr << "map outgoing cypher size 2771 self-test failed: block count for wordCount="
                      << test.wordCount << '\n';
            return false;
        }
    }

    // Combined path used by finalizePacket: words then blocks.
    if (mapnetworkinghelpers::OutgoingCypherBlockCount(mapnetworkinghelpers::OutgoingCypherWordCount(20)) != 2)
    {
        std::cerr << "map outgoing cypher size 2771 self-test failed: combined path\n";
        return false;
    }

    return true;
}
