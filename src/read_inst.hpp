#include <istream>
#include <cstdint>
#include <cstddef>
template <size_t capacity>
using inst_pool = uint8_t[capacity];

template <size_t capacity>
void read_inst(std::istream &is, inst_pool<capacity> &inst)
{
	auto saved_flags = is.flags(std::ios_base::hex);
	uint8_t *ptr = inst;
	while ((is >> std::ws).good())
	{
		if (is.peek() == '@')
		{
			is.get();
			size_t pos;
			is >> pos;
			ptr = inst + pos;
		}
		else
		{
			unsigned byte_val;
			is >> byte_val;
			*(ptr++) = byte_val;
		}
	}
	is.flags(saved_flags);
}
