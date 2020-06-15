#include "libs/catch.hpp"
#include "WordEncoder.hpp"

TEST_CASE("wordEncoder", "[word-encoder]") {
    WordEncoder::Address addr = {"111.122.133.144", 11111};
    const std::vector<int> addrNums = {111, 122, 133, 144, 11111};
    
    SECTION("toNums") {
        REQUIRE(WordEncoder::addrToNums(addr) == addrNums);
    }
    SECTION("numsToAddr") {
        auto newAddr = WordEncoder::numsToAddr(addrNums);
        REQUIRE(newAddr.ip == addr.ip);
        REQUIRE(newAddr.port == addr.port);
    }
    
    SECTION("asSingleNumber") {
        REQUIRE(WordEncoder::asSingleNumber({1, 1}, {16, 16}) == 17);
        REQUIRE(WordEncoder::asSingleNumber({15, 15}, {16, 16}) == 255);
        REQUIRE(WordEncoder::asSingleNumber({15, 1}, {16, 2}) == 31);
        REQUIRE(WordEncoder::asSingleNumber({1, 15}, {2, 16}) == 31);
        REQUIRE(WordEncoder::asSingleNumber({255, 255, 255, 255, 255, 255, 255, 255}, {256, 256, 256, 256, 256, 256, 256, 256}) == 18446744073709551615ULL);
        REQUIRE(WordEncoder::asSingleNumber({255, 255, 255, 255, 255, 255, 65535}, {256, 256, 256, 256, 256, 256, 65536}) == 18446744073709551615ULL);
        REQUIRE(WordEncoder::asSingleNumber({255, 255, 255, 255, 255, 255, 1}, {256, 256, 256, 256, 256, 256, 65536}) == 18446744073709486081ULL);
        REQUIRE(WordEncoder::asSingleNumber({255, 255, 255, 255, 255, 2, 1}, {256, 256, 256, 256, 256, 256, 65536}) == 18446744073692905473ULL);
        REQUIRE(WordEncoder::asSingleNumber({255, 255, 255, 2, 1}, {256, 256, 256, 256, 65536}) == 281474960064513ULL);
        REQUIRE(WordEncoder::asSingleNumber({5, 4, 3, 2, 1}, {256, 256, 256, 256, 65536}) == 5514788470785ULL);
    }
    
    SECTION("asMultipleNumbers") {
        REQUIRE(WordEncoder::asMultipleNumbers(17, {16, 16}) == std::vector<int>({1, 1}));
        REQUIRE(WordEncoder::asMultipleNumbers(255, {16, 16}) == std::vector<int>({15, 15}));
        REQUIRE(WordEncoder::asMultipleNumbers(31, {16, 2}) == std::vector<int>({15, 1}));
        REQUIRE(WordEncoder::asMultipleNumbers(31, {2, 16}) == std::vector<int>({1, 15}));
        REQUIRE(WordEncoder::asMultipleNumbers(18446744073709551615ULL, {256, 256, 256, 256, 256, 256, 256, 256}) == std::vector<int>({255, 255, 255, 255, 255, 255, 255, 255}));
        REQUIRE(WordEncoder::asMultipleNumbers(18446744073709551615ULL, {256, 256, 256, 256, 256, 256, 65536}) == std::vector<int>({255, 255, 255, 255, 255, 255, 65535}));
        REQUIRE(WordEncoder::asMultipleNumbers(18446744073709486081ULL, {256, 256, 256, 256, 256, 256, 65536}) == std::vector<int>({255, 255, 255, 255, 255, 255, 1}));
        REQUIRE(WordEncoder::asMultipleNumbers(18446744073692905473ULL, {256, 256, 256, 256, 256, 256, 65536}) == std::vector<int>({255, 255, 255, 255, 255, 2, 1}));
        REQUIRE(WordEncoder::asMultipleNumbers(281474960064513ULL, {256, 256, 256, 256, 65536}) == std::vector<int>({255, 255, 255, 2, 1}));
        REQUIRE(WordEncoder::asMultipleNumbers(5514788470785ULL, {256, 256, 256, 256, 65536}) == std::vector<int>({5, 4, 3, 2, 1}));
    }
    
    SECTION("asSingleNumber/asMultipleNumbers") {
        REQUIRE(WordEncoder::asMultipleNumbers(WordEncoder::asSingleNumber(addrNums, WordEncoder::ipMaxNums), WordEncoder::ipMaxNums) == addrNums);
    }
    
    WordEncoder encoder("data/words.txt");
    auto decoded = encoder.fromPairingCode(encoder.toPairingCode(addr));
    REQUIRE(decoded.ip == addr.ip);
    REQUIRE(decoded.port == addr.port);
}
