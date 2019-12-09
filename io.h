#pragma once

#include <cstdio>
#include <cstdint>

namespace io
{
	uint8_t readU8(FILE* fh);
	uint16_t readU16(FILE* fh);
	uint32_t readU32(FILE* fh);
	void writeU8(FILE* fh, uint8_t value);
	void writeU16(FILE* fh, uint16_t value);
	void writeU32(FILE* fh, uint32_t value);
	size_t length(FILE* fh);
}