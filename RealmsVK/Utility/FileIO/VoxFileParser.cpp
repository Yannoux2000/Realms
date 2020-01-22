#include "VoxFileParser.h"
#include "../../Base/Math/Vec3.h"

#include <fstream>

using namespace rlms;

void rlms::VoxFileParser::Load (Voxelite& data, std::string const& filename) {
	std::ifstream file (filename, std::ios::in | std::ios::binary);
	if (!fileHasValidSignature (filename, &file)) {
		throw InvalidVoxFile ();
	}
	long int Size = -1;

	if (!checkFileStructure (&file)) {
		throw InvalidVoxFileFormat ();
	}


	while (!file.eof ()) {
		VoxChunk c;
		parseChunk (&file, c);

		switch (c.chunk_id) {
			case VoxChunkId::NONE:
				throw InvalidVoxFileFormat ();
			case VoxChunkId::MAIN:
				file.seekg (c.chunk_content);
				break;
			case VoxChunkId::PACK:
				throw InvalidVoxFileFormat ();
			case VoxChunkId::SIZE:
				parseVoxelite (&file, c, data);
				break;
			case VoxChunkId::RGBA:
			case VoxChunkId::MATT:
				return;
		}
	}

	throw UnsupportedVoxFile ();
}

void VoxFileParser::LoadPalette (std::string const& filename, std::array<int, 256>& palette) {
	std::ifstream file (filename, std::ios::in | std::ios::binary);
	if (!fileHasValidSignature (filename, &file)) {
		for (int i = 0; i < 256; i++) {
			palette[i] = default_palette[i];
		}
		throw InvalidVoxFile ();
	}
	long int Size = -1;

	if (!checkFileStructure (&file)) {
		throw InvalidVoxFileFormat ();
	}

	Voxelite data;

	while (!file.eof ()) {
		VoxChunk c;
		parseChunk (&file, c);

		switch (c.chunk_id) {
			case VoxChunkId::NONE:
				throw InvalidVoxFileFormat ();
			case VoxChunkId::MAIN:
				file.seekg (c.chunk_content);
				break;
			case VoxChunkId::PACK:
				throw InvalidVoxFileFormat ();
			case VoxChunkId::SIZE:
				parseVoxelite (&file, c, data);
				break;
			case VoxChunkId::RGBA:
				file.seekg (c.chunk_content);
				for (int i = 0; i < 256; i++) {
					int tmp;
					file.read ((char*)(&tmp), 4);
					palette[i] = tmp;
				}
			case VoxChunkId::MATT:
				return;
		}
	}

	throw UnsupportedVoxFile ();
}

void VoxFileParser::align (std::ifstream* const& file, size_t n) {
	file->ignore (n);
}

void VoxFileParser::parseChunk (std::ifstream* const& file, VoxChunk & c) {
	char buffer[5] = {0};

	file->read (buffer, 4);

	c.chunk_id = VoxChunkId::NONE;
	if (buffer[0] == 'M' && buffer[1] == 'A' && buffer[2] == 'I' && buffer[3] == 'N') c.chunk_id = VoxChunkId::MAIN;
	if (buffer[0] == 'P' && buffer[1] == 'A' && buffer[2] == 'C' && buffer[3] == 'K') c.chunk_id = VoxChunkId::PACK;
	if (buffer[0] == 'S' && buffer[1] == 'I' && buffer[2] == 'Z' && buffer[3] == 'E') c.chunk_id = VoxChunkId::SIZE;
	if (buffer[0] == 'X' && buffer[1] == 'Y' && buffer[2] == 'Z' && buffer[3] == 'I') c.chunk_id = VoxChunkId::XYZI;
	if (buffer[0] == 'R' && buffer[1] == 'G' && buffer[2] == 'B' && buffer[3] == 'A') c.chunk_id = VoxChunkId::RGBA;
	if (buffer[0] == 'M' && buffer[1] == 'A' && buffer[2] == 'T' && buffer[3] == 'T') c.chunk_id = VoxChunkId::MATT;

	file->read ((char*) &c.size_chunk, 4);
	file->read ((char*) &c.child_chunk, 4);
	c.chunk_content = file->tellg ();
}

void VoxFileParser::parseVoxelite (std::ifstream* const& file, VoxChunk& c, Voxelite& data) {
	file->seekg (c.chunk_content);

	int x;
	int y;
	int z;
	int x_offset;
	int y_offset;

	file->read ((char*)& x, 4);
	file->read ((char*)& y, 4);
	file->read ((char*)& z, 4);

	x_offset = static_cast<int>((float)x / 2);
	y_offset = static_cast<int>((float)y / 2);

	data.setDims (x, y, z);

	VoxChunk xyzi;
	parseChunk (file, xyzi);
	if (xyzi.chunk_id == VoxChunkId::XYZI) {
		int n_voxel = -1;

		file->read ((char*)& n_voxel, 4);
		for (int i = 0; i < n_voxel; i++) {
			char v_x = 0, v_y = 0, v_z = 0, v_c = 0;

			file->read (&v_x, 1);
			file->read (&v_y, 1);
			file->read (&v_z, 1);
			file->read (&v_c, 1);

			data.addVoxel (v_x - x_offset, v_y - y_offset, v_z, v_c - 1);
		}
	}
}

bool VoxFileParser::checkFileStructure (std::ifstream* const& file) {
	char buffer[9];
	file->get (buffer, 9);

	//std::cout << c8_buffer[0] << '\n';
	//std::cout << c8_buffer[1] << '\n';
	//std::cout << c8_buffer[2] << '\n';
	//std::cout << (int)c8_buffer[3] << '\n';
	//std::cout << *(int*)(c8_buffer + 4) << '\n';

	return !(*file) || (buffer[0] == 'V' && buffer[1] == 'O' && buffer[2] == 'X' && *(int*)(buffer + 4) == 150);
}

bool VoxFileParser::fileHasValidSignature (std::string const& filename, std::ifstream* const& file) {
	if (filename.substr (filename.length () - 4, 4).rfind (".vox") == std::string::npos) {
		return false;
	}

	if (!file->is_open ()) {
		return false;
	}
	return true;
}