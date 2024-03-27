#pragma once

#include <stdint.h>
#include <stddef.h>

#if !(__GNUC__ >= 13 || __clang_major__ >= 19)
#define constexpr const
#endif

/**
 * @brief A macro that gets the containing structure of a member.
 *
 * This macro is used to retrieve the containing structure of a member variable.
 * It's a common idiom in C programming, especially when dealing with data structures
 * and type abstraction.\n
 * It calculates the address of the structure by subtracting
 * the offset of the member within the structure from the address of the member itself.\n
 *
 * This definition was taken from the Linux kernel source code.
 *
 * @param ptr Pointer to the member.
 * @param type Type of the container struct this is embedded in.
 * @param member Name of the member within the struct.
 * @return Pointer to the containing structure.
 */
#define container_of(ptr, type, member) ({                  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type, member) );})


/**
 * @brief Calculates a hash value for a given data using the FNV-1a hash algorithm.
 *
 * FNV-1a has excellent dispersion for short strings and is relatively fast on modern processors.
 *
 * @param data Pointer to the data to be hashed.
 * @param len The length of the data to be hashed.
 * @return The calculated hash value (64-bit).
 */
static inline uint64_t fnv1a_hash(const uint8_t *data, const size_t len) {
    // FNV offset basis and FNV prime are two constants used in the FNV-1a hash algorithm.
    constexpr uint64_t FNV_offset_basis = 0xcbf29ce484222325ULL;
    constexpr uint64_t FNV_prime[[maybe_unused]] = 0x00000100000001b3ULL;

    // Initialize the hash to the FNV offset basis
    uint64_t hash = FNV_offset_basis;

    // Hash each byte in the buffer
    for (size_t i = 0; i < len; ++i) {
        // XOR the hash with the current byte
        hash ^= (uint64_t) data[i];
        // Multiply the hash by the FNV prime
#if __NO_FNV_GCC_OPTIMIZATION__
        // If FNV GCC optimization is not enabled, multiply the hash by the FNV prime
        hash *= FNV_prime;
#else
        // If FNV GCC optimization is enabled, perform an equivalent operation that is faster on some processors
        hash += (hash << 1) + (hash << 4) + (hash << 5) +
                (hash << 7) + (hash << 8) + (hash << 40);
#endif
    }

    return hash;
}

/**
 * @brief Enum representing various serialization constants.
 */
enum {
    SER_NIL = 0, ///< Represents a nil value.
    SER_ERR = 1, ///< Represents an error message.
    SER_STR = 2, ///< Represents a string value.
    SER_INT = 3, ///< Represents an integer value.
    SER_DBL = 4, ///< Represents a double value.
    SER_ARR = 5, ///< Represents an array.
};
