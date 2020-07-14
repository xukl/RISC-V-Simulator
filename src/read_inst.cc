#include <istream>
#include <cstdint>
#include <cstring>

extern const int MAX_INST_MEMORY;
extern uint8_t i_memory[];
extern uint8_t d_memory[];
void read_inst(std::istream &is)
{
	auto saved_flags = is.flags(std::ios_base::hex);
	uint8_t *ptr = i_memory;
	while ((is >> std::ws).good())
	{
		if (is.peek() == '@')
		{
			is.get();
			size_t pos;
			is >> pos;
			ptr = i_memory + pos;
		}
		else
		{
			unsigned byte_val;
			is >> byte_val;
			*(ptr++) = byte_val;
		}
	}
	is.flags(saved_flags);
	memcpy(d_memory, i_memory, ptr - i_memory);
}
