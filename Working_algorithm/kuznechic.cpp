#pragma once
#include<iostream>
#include"Kuznechic.hpp"

vect iter_key[10]; //итерационные ключи шифрования

vect iter_C[32];// Итерационные константы C

vect iter_num[32];
	
void Print(vect data) {
	for (auto i = 0; i < 16; i++) {
		cout << hex << (int)data[i] << " ";
	}
	cout << endl;
}

static void GOST_Kuz_X(const uint8_t* a, const uint8_t* b, uint8_t* c){
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

void GOST_Kuz_Get_C(){
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