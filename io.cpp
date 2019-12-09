#include "io.h"

namespace io
{
	uint8_t readU8(FILE* fh)
	{
		uint8_t value;
		fread(&value, sizeof(value), 1, fh);
		return value;
	}

	uint16_t readU16(FILE* fh)
	{
		uint16_t value;
		fread(&value, sizeof(value), 1, fh);
		return value;
	}

	uint32_t readU32(FILE* fh)
	{
		uint32_t value;
		fread(&value, sizeof(value), 1, fh);
		return value;
	}

	void writeU8(FILE* fh, uint8_t value)
	{
		fwrite(&value, sizeof(value), 1, fh);
	}

	void writeU16(FILE* fh, uint16_t value)
	{
		fwrite(&value, sizeof(value), 1, fh);
	}

	void writeU32(FILE* fh, uint32_t value)
	{
		fwrite(&value, sizeof(value), 1, fh);
	}

	size_t length(FILE* fh)
	{
		size_t pos = ftell(fh);
		fseek(fh, 0, SEEK_END);
		size_t length = ftell(fh);
		fseek(fh, pos, SEEK_SET);
		return length;
	}
}
