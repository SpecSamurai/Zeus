#pragma once

#define XXH_NO_STDLIB
#define XXH_STATIC_LINKING_ONLY

#include <xxhash.h>

namespace Zeus
{
using Hash = XXH32_hash_t;

class Hasher
{
public:
    inline constexpr Hasher() : m_state{}
    {
        XXH32_reset(&m_state, SEED);
    }

    template <typename T>
    inline constexpr void Combine(const T& input)
    {
        XXH32_update(&m_state, &input, sizeof(T));
    }

    inline constexpr Hash Digest() const
    {
        return XXH32_digest(&m_state);
    }

    template <typename T>
    inline static constexpr Hash Digest(const T& input)
    {
        return XXH32(&input, sizeof(T), SEED);
    }

private:
    static constexpr XXH32_hash_t SEED{ 0 };

    XXH32_state_t m_state;
};
}
