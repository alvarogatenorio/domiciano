#include <stdio.h>
#include <stdlib.h>

#define MASK1 0xFF
#define MASK2 0xFFFF
#define MASK3 0xFFFFFF
#define MASK4 0xFFFFFFFF

#define FILE_SIZE_BYTES 4
#define CHUNK_SIZE_BYTES 4

#define CHUNK_ID_BYTES 4
#define DWORD_BYTES 4

typedef char DWord [DWORD_BYTES + 1];
typedef unsigned long int Int32;

typedef struct FileHeader {
	DWord id;
	Int32 size;
	DWord codec;
} FileHeader;

typedef struct ChunkHeader {
	DWord id;
	Int32 size;
} ChunkHeader;

typedef struct Chunk {
	ChunkHeader header;
	void* data;
} Chunk;

int read_dword(FILE* file, DWord dword) {
	fread(dword, DWORD_BYTES, 1, file);
	for (int i = 0; i < DWORD_BYTES; i++) {
		dword[i] &= MASK1;
	}
	dword[DWORD_BYTES] = '\0';
	return 0;
}

int read_file_header(FILE* file, FileHeader* header) {
	read_dword(file, header->id);
	fread(&(header->size), FILE_SIZE_BYTES, 1, file);
	header->size &= MASK4;
	read_dword(file, header->codec);
	return 0;
}

int read_chunk_header(FILE* file, ChunkHeader* header) {
	read_dword(file, header->id);
	fread(&(header->size), CHUNK_SIZE_BYTES, 1, file);
	header->size &= MASK4;
	return 0;
}

int read_chunk(FILE* file, Chunk* chunk) {
	read_chunk_header(file, &(chunk->header));
	chunk->data = malloc(chunk->header.size);
	fread(chunk->data, chunk->header.size, 1, file);
	return 0;
}

void print_file_header(FileHeader* header) {
	printf("file id: %s, file size: %lu, codec: %s\n", header->id, header->size, header->codec);
}

void print_chunk_header(ChunkHeader* header) {
	printf("chunk id: %s, chunk size: %lu\n", header->id, header->size);
}

int main(int argc, char** argv) {
	FILE* file = fopen("FINAL.DXR", "r");
	FileHeader file_header;
	read_file_header(file, &file_header);
	print_file_header(&file_header);
	int bytes_left = file_header.size;
	ChunkHeader header;
	while (bytes_left > 0) {
		read_chunk_header(file, &header);
		print_chunk_header(&header);
		fseek(file, header.size, SEEK_CUR);
		bytes_left -= header.size + CHUNK_ID_BYTES;
	}
	fclose(file);
	return 0;
}
