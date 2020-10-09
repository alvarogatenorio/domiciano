#include <stdio.h>
#include <stdlib.h>

#define MASK1 0xFF
#define MASK2 0xFFFF
#define MASK3 0xFFFFFF
#define MASK4 0xFFFFFFFF

#define INT32_BYTES 4
#define FILE_SIZE_BYTES 4
#define CHUNK_SIZE_BYTES 4

#define DWORD_BYTES 4
#define CHUNK_ID_BYTES 4

#define FILE_HEADER_SIZE DWORD_BYTES + FILE_SIZE_BYTES + DWORD_BYTES
#define CHUNK_HEADER_SIZE CHUNK_ID_BYTES + CHUNK_SIZE_BYTES

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
	fread(dword, (size_t)(DWORD_BYTES), 1, file);
	for (int i = 0; i < DWORD_BYTES; i++) {
		dword[i] &= MASK1;
	}
	dword[DWORD_BYTES] = '\0';
	return 0;
}

int read_file_header(FILE* file, FileHeader* header) {
	read_dword(file, header->id);
	fread(&(header->size), (size_t)(FILE_SIZE_BYTES), 1, file);
	header->size &= MASK4;
	read_dword(file, header->codec);
	return 0;
}

int read_chunk_header(FILE* file, ChunkHeader* header) {
	read_dword(file, header->id);
	fread(&(header->size), (size_t)(CHUNK_SIZE_BYTES), 1, file);
	header->size &= MASK4;
	return 0;
}

int read_chunk(FILE* file, Chunk* chunk) {
	read_chunk_header(file, &(chunk->header));
	if ((chunk->header.size) != (chunk->header.size & MASK2)) {
		printf("ERROR: The chunk data is too large.\n");
		return 1;
	}
	chunk->data = malloc((size_t)(chunk->header.size));
	fread(chunk->data, (size_t)(chunk->header.size), 1, file);
	return 0;
}

Int32 get_memory_map_address(FILE* file) {
	fseek(file, (long) (FILE_HEADER_SIZE + CHUNK_HEADER_SIZE + INT32_BYTES), SEEK_SET);
	Int32 memory_map_address;
	fread(&memory_map_address, (size_t)(INT32_BYTES), 1, file);
	memory_map_address &= MASK4;
	return memory_map_address;
}

void print_file_header(FileHeader* header) {
	printf("file id: %s, file size: %lu, codec: %s\n", header->id, header->size, header->codec);
}

void print_chunk_header(ChunkHeader* header) {
	printf("chunk id: %s, chunk size: %lu\n", header->id, header->size);
}

int main(int argc, char** argv) {
	FILE* file = fopen("FINAL.DXR", "r");
	Int32 memory_map_address = get_memory_map_address(file);
	printf("Memory map address: %lu\n", memory_map_address);
	fseek(file, (long)(memory_map_address), SEEK_SET);
	ChunkHeader header;
	read_chunk_header(file, &header);
	print_chunk_header(&header);
	fclose(file);
	return 0;
}
