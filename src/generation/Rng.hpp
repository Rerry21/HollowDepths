#pragma once

#include <cstdint>
#include <random>

class Rng
{
public:
    explicit Rng(std::uint64_t seed);

    std::uint64_t seed() const;
    std::uint64_t nextU64();
    int range(int minInclusive, int maxInclusive);
    bool chance(int percent);

    static std::uint64_t deriveSeed(std::uint64_t baseSeed, int attempt);

private:
    std::mt19937_64 m_engine;
    std::uint64_t m_seed;
};

std::uint64_t randomSeed();