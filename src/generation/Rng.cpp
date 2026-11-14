#include "generation/Rng.hpp"

#include <limits>

Rng::Rng(std::uint64_t seed)
    : m_engine(seed)
    , m_seed(seed)
{
}

std::uint64_t Rng::seed() const
{
    return m_seed;
}

std::uint64_t Rng::nextU64()
{
    return m_engine();
}

int Rng::range(int minInclusive, int maxInclusive)
{
    if (maxInclusive <= minInclusive)
    {
        return minInclusive;
    }

    const std::uint64_t span =
        static_cast<std::uint64_t>(static_cast<std::int64_t>(maxInclusive) - minInclusive) + 1;
    constexpr std::uint64_t kMax = std::numeric_limits<std::uint64_t>::max();
    const std::uint64_t limit = (kMax / span) * span;

    std::uint64_t value = m_engine();
    while (value >= limit)
    {
        value = m_engine();
    }

    return minInclusive + static_cast<int>(value % span);
}

bool Rng::chance(int percent)
{
    return range(0, 99) < percent;
}

std::uint64_t Rng::deriveSeed(std::uint64_t baseSeed, int attempt)
{
    std::uint64_t z = baseSeed + 0x9E3779B97F4A7C15ULL * (static_cast<std::uint64_t>(attempt) + 1ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

std::uint64_t randomSeed()
{
    std::random_device device;
    return (static_cast<std::uint64_t>(device()) << 32) | static_cast<std::uint64_t>(device());
}