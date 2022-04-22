#include<iostream>
#include "kuznechic.h"
using namespace::std;

#define BLOCK_SIZE 16 
#define BUFF_SIZE 1024
typedef uint8_t vect[BLOCK_SIZE];

vect iter_key[10]; //итерационные ключи шифрования

vect iter_C[32];// Итерационные константы C

vect iter_num[32]; // Сюда будем писать номер итерации от 1 до 32 (вообще хз зачем это)


static const uint8_t l_vec[16] = {
	148, 32, 133, 16, 194, 192, 1, 251, 1, 192, 194, 16, 133, 32, 148, 1
};


const uint8_t kuz_pi[0x100] = {
		252, 238, 221, 17, 207, 110, 49, 22, 251, 196, 250, 218, 35, 197, 4, 77, 233,
		119, 240, 219, 147, 46, 153, 186, 23, 54, 241, 187, 20, 205, 95, 193, 249, 24, 101,
		90, 226, 92, 239, 33, 129, 28, 60, 66, 139, 1, 142, 79, 5, 132, 2, 174, 227, 106, 143,
		160, 6, 11, 237, 152, 127, 212, 211, 31, 235, 52, 44, 81, 234, 200, 72, 171, 242, 42,
		104, 162, 253, 58, 206, 204, 181, 112, 14, 86, 8, 12, 118, 18, 191, 114, 19, 71, 156,
		183, 93, 135, 21, 161, 150, 41, 16, 123, 154, 199, 243, 145, 120, 111, 157, 158, 178,
		177, 50, 117, 25, 61, 255, 53, 138, 126, 109, 84, 198, 128, 195, 189, 13, 87, 223,
		245, 36, 169, 62, 168, 67, 201, 215, 121, 214, 246, 124, 34, 185, 3, 224, 15, 236,
		222, 122, 148, 176, 188, 220, 232, 40, 80, 78, 51, 10, 74, 167, 151, 96, 115, 30, 0,
		98, 68, 26, 184, 56, 130, 100, 159, 38, 65, 173, 69, 70, 146, 39, 94, 85, 47, 140, 163,
		165, 125, 105, 213, 149, 59, 7, 88, 179, 64, 134, 172, 29, 247, 48, 55, 107, 228, 136,
		217, 231, 137, 225, 27, 131, 73, 76, 63, 248, 254, 141, 83, 170, 144, 202, 216, 133,
		97, 32, 113, 103, 164, 45, 43, 9, 91, 203, 155, 37, 208, 190, 229, 108, 82, 89, 166,
		116, 210, 230, 244, 180, 192, 209, 102, 175, 194, 57, 75, 99, 182
};

static const uint8_t kuz_pi_inv[0x100] = {
		165, 45, 50, 143, 14, 48, 56, 192, 84, 230, 158, 57, 85, 126, 82, 145,
		100, 3, 87, 90, 28, 96, 7, 24, 33, 114, 168, 209, 41, 198, 164, 63,
		224, 39, 141, 12, 130, 234, 174, 180, 154, 99, 73, 229, 66, 228, 21, 183,
		200, 6, 112, 157, 65, 117, 25, 201, 170, 252, 77, 191, 42, 115, 132, 213,
		195, 175, 43, 134, 167, 177, 178, 91, 70, 211, 159, 253, 212, 15, 156, 47,
		155, 67, 239, 217, 121, 182, 83, 127, 193, 240, 35, 231, 37, 94, 181, 30,
		162, 223, 166, 254, 172, 34, 249, 226, 74, 188, 53, 202, 238, 120, 5, 107,
		81, 225, 89, 163, 242, 113, 86, 17, 106, 137, 148, 101, 140, 187, 119, 60,
		123, 40, 171, 210, 49, 222, 196, 95, 204, 207, 118, 44, 184, 216, 46, 54,
		219, 105, 179, 20, 149, 190, 98, 161, 59, 22, 102, 233, 92, 108, 109, 173,
		55, 97, 75, 185, 227, 186, 241, 160, 133, 131, 218, 71, 197, 176, 51, 250,
		150, 111, 110, 194, 246, 80, 255, 93, 169, 142, 23, 27, 151, 125, 236, 88,
		247, 31, 251, 124, 9, 13, 122, 103, 69, 135, 220, 232, 79, 29, 78, 4,
		235, 248, 243, 62, 61, 189, 138, 136, 221, 205, 11, 19, 152, 2, 147, 128,
		144, 208, 36, 52, 203, 237, 244, 206, 153, 16, 68, 64, 146, 58, 1, 38,
		18, 26, 72, 104, 245, 129, 139, 199, 214, 32, 10, 8, 0, 76, 215, 116
};
	
void Print(vect data) {
	for (auto i = 0; i < 16; i++) {
		cout << hex << (int)data[i] << " ";
	}
	cout << endl;
}

static void GOST_Kuz_X(const uint8_t* a, const uint8_t* b, uint8_t* c)
{
	int i;
	for (i = 0; i < BLOCK_SIZE; i++) {
		c[i] = a[i] ^ b[i];
	}
}

static void GOST_Kuz_S(const uint8_t* in_data, uint8_t* out_data) {
	int i;
	for (i = 0; i < 16; i++) {
		out_data[i] = kuz_pi[in_data[i]];
	}
}

int multiply_ints_as_polynomials(int x, int y) {
	int z = 0;                              
	while (x != 0) {                    
		if ((x & 1) == 1) {            
			z ^= y;                    
		}
		y <<= 1;                        
		x >>= 1;                        
	}
	return z;
}

// считаем количество бит 
int number_bits(int x) {
	int nb = 0;
	while (x != 0) {
		nb += 1;                 
		x >>= 1;                 
	}
	return nb;
}

int mod_int_as_polynomial(int x, int m) {
	int nbm = number_bits(m);              
	while (1) {                            
		int nbx = number_bits(x);
		if (nbx < nbm) {                   
			return x;
		}
		int mshift = m << (nbx - nbm);    
		x ^= mshift;                       
	}
}

//Основная функция умножения:
int kuznyechik_multiplication(int x, int y) {
	int z = multiply_ints_as_polynomials(x, y);     
	int m = 0b111000011;                            
	return mod_int_as_polynomial(z, m);             
}

static void GOST_Kuz_R(uint8_t state[16]) {
		int i;
		uint8_t tmp = 0;
		vect internal;
		tmp = state[15];	
		for (i = 0; i < 15; i++) {
			internal[i + 1] = state[i];
			tmp ^= kuznyechik_multiplication(state[i], l_vec[i]);
		}		
		internal[0] = tmp;
		memcpy(state, internal, BLOCK_SIZE);
}

static void GOST_Kuz_L(const uint8_t* in_data, uint8_t* out_data){
	int i;
	vect internal;
	memcpy(internal, in_data, BLOCK_SIZE);
	for (i = 0; i < 16; i++) {
		GOST_Kuz_R(internal);
	}
	memcpy(out_data, internal, BLOCK_SIZE);
}

void GOST_Kuz_Get_C()
{
	int i;
	 uint8_t iter_num [32][16];
	for (i = 0; i < 32; i++){
		for (int j = 0; j < BLOCK_SIZE; j++)
			iter_num[i][j] = 0;
		iter_num[i][BLOCK_SIZE-1] = (uint8_t)(i + 1);
	}
	for (i = 0; i < 32; i++){
		GOST_Kuz_L(iter_num[i], iter_C[i]);
	}
}

static void GOST_Kuz_F(const uint8_t* in_key_1, const uint8_t* in_key_2,
						uint8_t* out_key_1, uint8_t* out_key_2,
						uint8_t* iter_const){
	vect internal;
	memcpy(out_key_2, in_key_1, BLOCK_SIZE);
	GOST_Kuz_X(in_key_1, iter_const, internal);
	GOST_Kuz_S(internal, internal);
	GOST_Kuz_L(internal, internal);
	GOST_Kuz_X(internal, in_key_2, out_key_1);
}

void GOST_Kuz_Expand_Key(const uint8_t* key_1, const uint8_t* key_2) {
	int i;
	uint8_t iter_1[64];
	uint8_t iter_2[64];

	uint8_t iter_3[64];
	uint8_t iter_4[64];
	GOST_Kuz_Get_C(); 

	memcpy(iter_key[0], key_1, 64);
	memcpy(iter_key[1], key_2, 64);
	memcpy(iter_1, key_1, 64);
	memcpy(iter_2, key_2, 64);

	for (i = 0; i < 4; i++) {
		GOST_Kuz_F(iter_1, iter_2, iter_3, iter_4, iter_C[0 + 8 * i]);
		GOST_Kuz_F(iter_3, iter_4, iter_1, iter_2, iter_C[1 + 8 * i]);
		GOST_Kuz_F(iter_1, iter_2, iter_3, iter_4, iter_C[2 + 8 * i]);
		GOST_Kuz_F(iter_3, iter_4, iter_1, iter_2, iter_C[3 + 8 * i]);
		GOST_Kuz_F(iter_1, iter_2, iter_3, iter_4, iter_C[4 + 8 * i]);
		GOST_Kuz_F(iter_3, iter_4, iter_1, iter_2, iter_C[5 + 8 * i]);
		GOST_Kuz_F(iter_1, iter_2, iter_3, iter_4, iter_C[6 + 8 * i]);
		GOST_Kuz_F(iter_3, iter_4, iter_1, iter_2, iter_C[7 + 8 * i]);
		memcpy(iter_key[2 * i + 2], iter_1, 64);
		memcpy(iter_key[2 * i + 3], iter_2, 64);
	}
}

void GOST_Kuz_Encript(const uint8_t* blk, uint8_t* out_blk){
	int i;
	memcpy(out_blk, blk, BLOCK_SIZE);

	for (i = 0; i < 9; i++){
		GOST_Kuz_X(iter_key[i], out_blk, out_blk);
		GOST_Kuz_S(out_blk, out_blk);
		GOST_Kuz_L(out_blk, out_blk);
	}
	GOST_Kuz_X(out_blk, iter_key[9], out_blk);
}

static void GOST_Kuz_reverse_R(uint8_t* state) {
	int i;
	uint8_t tmp;
	vect internal;
	tmp = state[0];
	for (i = 0; i < 15; i++) {
		internal[i] = state[i + 1];
		tmp ^= kuznyechik_multiplication(state[i + 1], l_vec[i]);
	}
	internal[15] = tmp;
	memcpy(state, internal, BLOCK_SIZE);
}

static void GOST_Kuz_reverse_L(const uint8_t* in_data, uint8_t* out_data){
	int i;
	vect internal;
	memcpy(internal, in_data, BLOCK_SIZE);
	for (i = 0; i < 16; i++) {
		GOST_Kuz_reverse_R(internal);
	}
	memcpy(out_data, internal, BLOCK_SIZE);
}

static void GOST_Kuz_reverse_S(const uint8_t* in_data, uint8_t* out_data){
	int i;
	for (i = 0; i < BLOCK_SIZE; i++) {
		out_data[i] = kuz_pi_inv[in_data[i]];
	}
}

 void GOST_Kuz_Decript(const uint8_t * blk, uint8_t * out_blk) {
		int i;
		memcpy(out_blk, blk, BLOCK_SIZE);
	
		GOST_Kuz_X(out_blk, iter_key[9], out_blk);
		for (i = 8; i >= 0; i--){
			GOST_Kuz_reverse_L(out_blk, out_blk);
			GOST_Kuz_reverse_S(out_blk, out_blk);
			GOST_Kuz_X(iter_key[i], out_blk, out_blk);
		}
}

int main() {
	uint8_t  ch, outBlock[16];
	size_t position = 16;
	uint8_t text[16] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x00, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88 };
	uint8_t key1[16] = { 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
	uint8_t key2[16] = { 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };

	uint8_t test[16] = { 0xa5, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	uint8_t encripted[16], decripted[16];

	Print(text);
	GOST_Kuz_Expand_Key(key1, key2);
	GOST_Kuz_Encript(text, encripted);
	Print(encripted);
	GOST_Kuz_Decript(encripted, decripted);
	Print(decripted);
}
