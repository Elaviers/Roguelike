#include "Hashing.hpp"

const uint32 cube_roots[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };

inline void Uint32ToBytes(uint32 value, byte bytes[4]) {
	bytes[0] = (value & 0xFF000000) >> 24;
	bytes[1] = (value & 0x00FF0000) >> 16;
	bytes[2] = (value & 0x0000FF00) >> 8;
	bytes[3] = value & 0x000000FF;
}

inline uint32 BytesToUInt32(const byte bytes[4]) {
	return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

inline uint32 RightRotate(uint32 x, size_t amount) {
	return (x << (32 - amount)) | (x >> amount);
}

void Hashing::HashSha256(const void* in, size_t length, byte out[32]) {
	//Initialise hashvalues with square roots of first 8 primes
	uint32 hashvalues[8] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };

	size_t chunks = (((length + 9) / 64) + 1);
	size_t full_length = chunks * 64;

	byte* data = new byte[full_length]();
	for (size_t i = 0; i < length; ++i)
		data[i] = ((byte*)in)[i];

	data[length] = 0b10000000;

	uint64 stringbitlength = (uint64)length * 8;
	data[full_length - 8] = (byte)((stringbitlength & 0xFF00000000000000) >> 56);
	data[full_length - 7] = (byte)((stringbitlength & 0x00FF000000000000) >> 48);
	data[full_length - 6] = (byte)((stringbitlength & 0x0000FF0000000000) >> 40);
	data[full_length - 5] = (byte)((stringbitlength & 0x000000FF00000000) >> 32);
	data[full_length - 4] = (byte)((stringbitlength & 0x00000000FF000000) >> 24);
	data[full_length - 3] = (byte)((stringbitlength & 0x0000000000FF0000) >> 16);
	data[full_length - 2] = (byte)((stringbitlength & 0x000000000000FF00) >> 8);
	data[full_length - 1] = (byte)(stringbitlength & 0x00000000000000FF);

	uint32 msa[64];
	for (size_t chunk = 0; chunk < chunks; ++chunk) {
		for (size_t i = 0; i < 16; ++i)
			msa[i] = BytesToUInt32(data + chunk * 64 + i * 4);

		for (size_t i = 16; i < 64; ++i) {
			uint32 s0 = RightRotate(msa[i - 15], 7) ^ RightRotate(msa[i - 15], 18) ^ (msa[i - 15] >> 3);
			uint32 s1 = RightRotate(msa[i - 2], 17) ^ RightRotate(msa[i - 2], 19) ^ (msa[i - 2] >> 10);
			msa[i] = msa[i - 16] + s0 + msa[i - 7] + s1;
		}

		uint32 a = hashvalues[0];
		uint32 b = hashvalues[1];
		uint32 c = hashvalues[2];
		uint32 d = hashvalues[3];
		uint32 e = hashvalues[4];
		uint32 f = hashvalues[5];
		uint32 g = hashvalues[6];
		uint32 h = hashvalues[7];

		for (int i = 0; i < 64; ++i) {
			uint32 s1 = RightRotate(e, 6) ^ RightRotate(e, 11) ^ RightRotate(e, 25);
			uint32 ch = (e & f) ^ ((~e) & g);
			uint32 temp1 = h + s1 + ch + cube_roots[i] + msa[i];
			uint32 s0 = RightRotate(a, 2) ^ RightRotate(a, 13) ^ RightRotate(a, 22);
			uint32 maj = (a & b) ^ (a & c) ^ (b & c);
			uint32 temp2 = s0 + maj;

			h = g;
			g = f;
			f = e;
			e = d + temp1;
			d = c;
			c = b;
			b = a;
			a = temp1 + temp2;
		}

		hashvalues[0] += a;
		hashvalues[1] += b;
		hashvalues[2] += c;
		hashvalues[3] += d;
		hashvalues[4] += e;
		hashvalues[5] += f;
		hashvalues[6] += g;
		hashvalues[7] += h;
	}

	delete[] data;

	for (int i = 0; i < 8; ++i)
	{
		int offset = i * 4;
		Uint32ToBytes(hashvalues[i], out + offset);
	}
}
