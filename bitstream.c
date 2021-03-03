#include "bitstream.h"

#include "assert.h"
#include "assume.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

_Static_assert(sizeof(bitstream_t) == 32UL,
	"sizeof(bitstream_t) ought to be 32");

void bitstream_flush(bitstream_t *stream)
{
	ASSERT(stream->base <= stream->pointer);
	ASSERT(stream->pointer < stream->base + stream->size);
	ASSERT(stream->bits < 8);
	if (stream->bits & 7)
		bitstream_putbits(stream, 0, 8 - stream->bits);
	ASSERT(stream->bits == 0 && stream->buffer == 0);
	size_t size = stream->pointer - stream->base;
	for (size_t pos = 0; pos < size;) {
		size_t len = size - pos;
		const void *buf = stream->base + pos;
		ssize_t written = write(STDOUT_FILENO, buf, len);
		if (written < 0)
			perror("write"), exit(7);
		pos += written;
	}
	stream->pointer = stream->base;
}

uint16_t bitstream_getbits(bitstream_t *stream, uint8_t length)
{
	ASSERT(stream->base <= stream->pointer);
	ASSERT(stream->pointer < stream->base + stream->size);
	ASSERT(stream->bits < 8);
	ASSERT(0 < length && length <= 16);
	while (stream->bits < length) {
		if (stream->base + stream->length <= stream->pointer) {
			ssize_t rlen = read(STDIN_FILENO, stream->base, stream->size);
			if (rlen < 0)
				perror("read"), exit(8);
			if (rlen == 0)
				fputs("insufficient input to decompress\n", stderr), exit(9);
			stream->length = rlen;
			stream->pointer = stream->base;
		}
		stream->bits += 8;
		ASSERT(stream->bits < 24);
		stream->buffer <<= 8;
		stream->buffer |= *stream->pointer++;
	}
	stream->bits -= length;
	ASSERT(stream->bits < 8);
	return (stream->buffer >> stream->bits) & ((1U << length) - 1);
}

void bitstream_init(bitstream_t *stream, void *base, size_t size)
{
	ASSERT(size < (1UL << 35));
	stream->base = stream->pointer = base;
	stream->length = stream->buffer = stream->bits = 0;
	stream->size = size;
}

void bitstream_putbits(bitstream_t *stream, uint16_t bits, uint8_t length)
{
	ASSERT(stream->base <= stream->pointer);
	ASSERT(stream->pointer < stream->base + stream->size);
	ASSERT(stream->bits < 8);
	ASSERT(0 < length && length <= 16);
	stream->bits += length;
	stream->buffer <<= length;
	stream->buffer |= bits & ((1U << length) - 1);
	while (8 <= stream->bits) {
		stream->bits -= 8;
		*stream->pointer++ = (uint8_t)(stream->buffer >> stream->bits);
		ASSERT(stream->base <= stream->pointer);
		if (stream->base + stream->size == stream->pointer) {
			for (size_t pos = 0; pos < stream->size;) {
				size_t len = stream->size - pos;
				const void *buf = stream->base + pos;
				ssize_t written = write(STDOUT_FILENO, buf, len);
				if (written < 0)
					perror("write"), exit(10);
				pos += written;
			}
			stream->pointer = stream->base;
		}
		ASSERT(stream->pointer <= stream->base + stream->size);
	}
	ASSERT(stream->bits < 8);
	stream->buffer &= (1U << stream->bits) - 1;
}
