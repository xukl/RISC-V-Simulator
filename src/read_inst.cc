#include <istream>
#include <cstdint>

extern uint8_t memory[];
void read_inst(std::istream &is)
{
	auto saved_flags = is.flags(std::ios_base::hex);
	uint8_t *ptr = memory;
	while ((is >> std::ws).good())
	{
		if (is.peek() == '@')
		{
			is.get();
			size_t pos;
			is >> pos;
			ptr = memory + pos;
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
