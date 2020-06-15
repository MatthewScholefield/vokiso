#include "WordEncoder.hpp"
#include <algorithm>
#include <sstream>

const std::vector<int> WordEncoder::ipMaxNums = {256, 256, 256, 256, 65536};

WordEncoder::WordEncoder(const std::string &wordsFilename) : words(readLines(wordsFilename)), wordToIndex(createWordIndex(words)) {}

std::vector<int> WordEncoder::toBaseN(unsigned long long num, int n) {
    std::vector<int> digits;
    while (num > 0) {
        digits.push_back(num % n);
        num /= n;
    }
    std::reverse(digits.begin(), digits.end());
    return digits;
}

unsigned long long WordEncoder::asSingleNumber(const std::vector<int> &nums, const std::vector<int> &maxNums) {
    unsigned long long combined = 0;
    unsigned long long place = 1;
    for (size_t i = 0; i < nums.size(); ++i) {
        combined += nums[nums.size() - 1 - i] * place;
        place *= maxNums[maxNums.size() - 1 - i];
    }
    return combined;
}

std::vector<int> WordEncoder::addrToNums(const Address &addr) {
    std::vector<int> nums;
    std::stringstream ss(addr.ip);
    std::string token;
    while (std::getline(ss, token, '.')) {
        nums.push_back(std::stoi(token));
    }
    nums.push_back(addr.port);
    return nums;
}

std::vector<int> WordEncoder::toWordIndices(const Address &addr) {
    return toBaseN(asSingleNumber(addrToNums(addr), ipMaxNums), words.size());
}

std::vector<int> WordEncoder::asMultipleNumbers(unsigned long long combined, std::vector<int> maxNums) {
    std::vector<int> nums;
    for (size_t i = 0; i < maxNums.size(); ++i) {
        int maxNum = maxNums[maxNums.size() - 1 - i];
        nums.push_back(combined % maxNum);
        combined /= maxNum;
    }
    if (combined > 0) {
        throw std::runtime_error("Invalid combined number. Remaining: " + std::to_string(combined));
    }
    std::reverse(nums.begin(), nums.end());
    return nums;
}

std::string WordEncoder::toPairingCode(Address address) {
    std::stringstream ss;
    std::vector<int> wordIndices = toWordIndices(address);
    for (size_t i = 0; i < wordIndices.size(); ++i) {
        ss << words[wordIndices[i]];
        if (i != wordIndices.size() - 1) {
            ss << " ";
        }
    }
    return ss.str();
}

WordEncoder::Address WordEncoder::numsToAddr(const std::vector<int> &nums) {
    std::stringstream ss;
    for (size_t i = 0; i < nums.size() - 1; ++i) {
        ss << std::to_string(nums[i]);
        if (i < nums.size() - 2) {
            ss << ".";
        }
    }
    Address addr;
    addr.ip = ss.str();
    addr.port = nums[nums.size() - 1];
    return addr;
}

WordEncoder::Address WordEncoder::fromPairingCode(const std::string &code) {
    unsigned long long combined;
    {
        std::stringstream ss(code);
        std::string token;
        std::vector<int> nums;
        std::vector<int> maxNums;
        while (ss >> token) {
            nums.push_back(wordToIndex[token]);
            maxNums.push_back(words.size());
        }
        combined = asSingleNumber(nums, maxNums);
    }
    return numsToAddr(asMultipleNumbers(combined, ipMaxNums));
}

std::map<std::string, int> WordEncoder::createWordIndex(const std::vector<std::string> words) {
    std::map<std::string, int> wordIndex;
    for (size_t i = 0; i < words.size(); ++i) {
        wordIndex[words[i]] = i;
    }
    return wordIndex;
}
