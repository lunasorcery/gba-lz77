#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include "io.h"

int main(int argc, char** argv)
{
	if (argc <= 2)
		return 1;

	const char* inPath = argv[1];
	const char* outPath = argv[2];

	FILE* fhIn = fopen(inPath, "rb");
	if (!fhIn)
		return 1;

	uint32_t header = io::readU32(fhIn);
	uint8_t magic = (header & 0xff);
	uint32_t decompressedLength = (header >> 8);

	if (magic != 0x10) {
		printf("Incorrect magic number, expected 0x10, got 0x%02x\n", magic);
		return 1;
	}

	uint8_t* destBuffer = new uint8_t[decompressedLength];
	size_t bytesWritten = 0;
	while (bytesWritten < decompressedLength)
	{
		uint8_t flags = io::readU8(fhIn);
		for (int i = 0; i < 8 && bytesWritten < decompressedLength; ++i)
		{
			bool type = (flags & (0x80 >> i));
			if (!type)
			{
				uint8_t value = io::readU8(fhIn);
				destBuffer[bytesWritten] = value;
				bytesWritten++;
			}
			else
			{
				uint16_t value = io::readU16(fhIn);
				uint16_t disp = ((value & 0xf) << 8) | (value >> 8);
				uint8_t n = ((value >> 4) & 0xf);
			//	printf("pos %x, src %x, length %d\n", bytesWritten, bytesWritten-disp-1, n+3);
				for (int j = 0; j < n + 3; ++j)
				{
					destBuffer[bytesWritten] = destBuffer[bytesWritten-disp-1];
					bytesWritten++;
				}
			}
		}
	}
	fclose(fhIn);

	printf("decompressed %zu bytes\n", bytesWritten);

	{
		FILE* fhOut = fopen(outPath, "wb");
		if (!fhOut)
			return 1;
		fwrite(destBuffer, 1, decompressedLength, fhOut);
		fclose(fhOut);
	}

	return 0;
}
