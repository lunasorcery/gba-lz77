#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include "io.h"

int findTokenWithLength(uint8_t* buffer, size_t offset, size_t inputLength, int length)
{
	if (offset + length > inputLength)
		return 0;

	for (uint8_t* ptr = buffer + offset - 8; ptr >= buffer; ptr--)
	{
		int delta = (ptr - buffer) - offset;

		int encodedDelta = -delta-1;
		if (encodedDelta > 0xFFF)
			return 0;

		bool match = true;
		for (int i = 0; i < length && match; ++i)
		{
			if (ptr[i] != buffer[offset+i])
				match = false;
		}
		if (match)
			return delta;
	}

	return 0;
}

int main(int argc, char** argv)
{
	if (argc <= 2)
		return 1;

	const char* inPath = argv[1];
	const char* outPath = argv[2];

	FILE* fhIn = fopen(inPath, "rb");
	if (!fhIn)
		return 1;

	size_t inputLength = io::length(fhIn);

	uint8_t* srcBuffer = new uint8_t[inputLength];
	fread(srcBuffer, 1, inputLength, fhIn);

	uint8_t* dstBuffer = new uint8_t[inputLength * 2];

	size_t inputBytesProcessed = 0;
	size_t bytesWritten = 0;
	size_t lastFlagPosition = 0;
	size_t lastFlagInputPosition = 0;
	int numFlagBits = 0;
	uint8_t flags = 0;

	while (inputBytesProcessed < inputLength)
	{
		if (numFlagBits == 0)
		{
			dstBuffer[bytesWritten] = 0x37;
			lastFlagPosition = bytesWritten;
			lastFlagInputPosition = inputBytesProcessed;
			bytesWritten++;
		}

		if (findTokenWithLength(srcBuffer, inputBytesProcessed, inputLength, 3))
		{
			int tokenSize = 0;
			int tokenDelta = 0;
			for (int attemptTokenSize = 3; attemptTokenSize <= 18; ++attemptTokenSize)
			{
				int attemptTokenDelta = findTokenWithLength(srcBuffer, inputBytesProcessed, inputLength, attemptTokenSize);
				if (attemptTokenDelta != 0) {
					tokenSize = attemptTokenSize;
					tokenDelta = attemptTokenDelta;
				} else {
					break;
				}
			}

			//printf("found token of %d bytes at %08x with offset %d\n", tokenSize, inputBytesProcessed, tokenDelta);

			int flippedTokenDelta = -tokenDelta-1;
			dstBuffer[bytesWritten] = (((tokenSize-3) & 0xf) << 4) | ((flippedTokenDelta & 0xf00) >> 8);
			bytesWritten++;
			dstBuffer[bytesWritten] = (flippedTokenDelta & 0xff);
			bytesWritten++;
			inputBytesProcessed += tokenSize;
			flags |= (0x80 >> numFlagBits);
		}
		else
		{
			dstBuffer[bytesWritten] = srcBuffer[inputBytesProcessed];
			bytesWritten++;
			inputBytesProcessed++;
			// don't set a flag
		}
		numFlagBits++;
		if (numFlagBits == 8 || inputBytesProcessed >= inputLength) {
			numFlagBits = 0;
			dstBuffer[lastFlagPosition] = flags;
			flags = 0;
		}
	}

	printf("compressed %zu bytes to %zu bytes\n", inputLength, bytesWritten);

	while ((bytesWritten % 4) != 0)
		++bytesWritten;

	{
		FILE* fhOut = fopen(outPath, "wb");
		if (!fhOut)
			return 1;
		io::writeU32(fhOut, (inputLength << 8) | 0x10);
		fwrite(dstBuffer, 1, bytesWritten, fhOut);
		fclose(fhOut);
	}

	return 0;
}
