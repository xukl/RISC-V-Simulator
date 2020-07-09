#ifndef COMMON_HPP
#define COMMON_HPP
#include <cstdint>
constexpr inline uint32_t sign_ext(int sign_bit_pos, uint32_t orig)
{
	return static_cast<uint32_t>(
			(static_cast<int32_t>(orig) << (31 - sign_bit_pos))
			>> (31 - sign_bit_pos));
}
constexpr inline uint32_t sign_ext_bit(int pos, uint32_t orig)
{
	return static_cast<uint32_t>(
			static_cast<int32_t>(orig & 0x80000000) >> (31 - pos));
}
#endif
