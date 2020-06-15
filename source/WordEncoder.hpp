#pragma once

#include <map>
#include <string>
#include <vector>

#include "utils.hpp"

class WordEncoder {
  public:
    static const std::vector<int> ipMaxNums;

    struct Address {
        std::string ip;
        int port;
    };

    WordEncoder(const std::string &wordsFilename);
    
    static std::vector<int> toBaseN(unsigned long long num, int n);
    static unsigned long long asSingleNumber(const std::vector<int> &nums, const std::vector<int> &maxNums);
    static std::vector<int> addrToNums(const Address &addr);
    std::vector<int> toWordIndices(const Address &addr);
    static std::vector<int> asMultipleNumbers(unsigned long long combined, std::vector<int> maxNums);
    std::string toPairingCode(Address address);
    static Address numsToAddr(const std::vector<int> &nums);
    Address fromPairingCode(const std::string &code);

  private:
    static std::map<std::string, int> createWordIndex(const std::vector<std::string> words);

    std::vector<std::string> words;
    std::map<std::string, int> wordToIndex;
};