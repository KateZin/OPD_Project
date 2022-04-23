//  from хакер.ру
#include <stdint.h>
#include <stdio.h>
#define BUFF_SIZE 1024

// мы используем блоки по 16 байт (128 бит) и для этого определяем вектор
#define BLOCK_SIZE 16                 
typedef uint8_t vect[BLOCK_SIZE];

const uint8_t kuz_pi[0x100] = {
	0xFC, 0xEE, 0xDD, 0x11, 0xCF, 0x6E, 0x31, 0x16, 	// 00..07
	0xFB, 0xC4, 0xFA, 0xDA, 0x23, 0xC5, 0x04, 0x4D, 	// 08..0F
	0xE9, 0x77, 0xF0, 0xDB, 0x93, 0x2E, 0x99, 0xBA, 	// 10..17
	0x17, 0x36, 0xF1, 0xBB, 0x14, 0xCD, 0x5F, 0xC1, 	// 18..1F
	0xF9, 0x18, 0x65, 0x5A, 0xE2, 0x5C, 0xEF, 0x21, 	// 20..27
	0x81, 0x1C, 0x3C, 0x42, 0x8B, 0x01, 0x8E, 0x4F, 	// 28..2F
	0x05, 0x84, 0x02, 0xAE, 0xE3, 0x6A, 0x8F, 0xA0, 	// 30..37
	0x06, 0x0B, 0xED, 0x98, 0x7F, 0xD4, 0xD3, 0x1F, 	// 38..3F
	0xEB, 0x34, 0x2C, 0x51, 0xEA, 0xC8, 0x48, 0xAB, 	// 40..47
	0xF2, 0x2A, 0x68, 0xA2, 0xFD, 0x3A, 0xCE, 0xCC, 	// 48..4F
	0xB5, 0x70, 0x0E, 0x56, 0x08, 0x0C, 0x76, 0x12, 	// 50..57
	0xBF, 0x72, 0x13, 0x47, 0x9C, 0xB7, 0x5D, 0x87, 	// 58..5F
	0x15, 0xA1, 0x96, 0x29, 0x10, 0x7B, 0x9A, 0xC7, 	// 60..67
	0xF3, 0x91, 0x78, 0x6F, 0x9D, 0x9E, 0xB2, 0xB1, 	// 68..6F
	0x32, 0x75, 0x19, 0x3D, 0xFF, 0x35, 0x8A, 0x7E, 	// 70..77
	0x6D, 0x54, 0xC6, 0x80, 0xC3, 0xBD, 0x0D, 0x57, 	// 78..7F
	0xDF, 0xF5, 0x24, 0xA9, 0x3E, 0xA8, 0x43, 0xC9, 	// 80..87
	0xD7, 0x79, 0xD6, 0xF6, 0x7C, 0x22, 0xB9, 0x03, 	// 88..8F
	0xE0, 0x0F, 0xEC, 0xDE, 0x7A, 0x94, 0xB0, 0xBC, 	// 90..97
	0xDC, 0xE8, 0x28, 0x50, 0x4E, 0x33, 0x0A, 0x4A, 	// 98..9F
	0xA7, 0x97, 0x60, 0x73, 0x1E, 0x00, 0x62, 0x44, 	// A0..A7
	0x1A, 0xB8, 0x38, 0x82, 0x64, 0x9F, 0x26, 0x41, 	// A8..AF
	0xAD, 0x45, 0x46, 0x92, 0x27, 0x5E, 0x55, 0x2F, 	// B0..B7
	0x8C, 0xA3, 0xA5, 0x7D, 0x69, 0xD5, 0x95, 0x3B, 	// B8..BF
	0x07, 0x58, 0xB3, 0x40, 0x86, 0xAC, 0x1D, 0xF7, 	// C0..C7
	0x30, 0x37, 0x6B, 0xE4, 0x88, 0xD9, 0xE7, 0x89, 	// C8..CF
	0xE1, 0x1B, 0x83, 0x49, 0x4C, 0x3F, 0xF8, 0xFE, 	// D0..D7
	0x8D, 0x53, 0xAA, 0x90, 0xCA, 0xD8, 0x85, 0x61, 	// D8..DF
	0x20, 0x71, 0x67, 0xA4, 0x2D, 0x2B, 0x09, 0x5B, 	// E0..E7
	0xCB, 0x9B, 0x25, 0xD0, 0xBE, 0xE5, 0x6C, 0x52, 	// E8..EF
	0x59, 0xA6, 0x74, 0xD2, 0xE6, 0xF4, 0xB4, 0xC0, 	// F0..F7
	0xD1, 0x66, 0xAF, 0xC2, 0x39, 0x4B, 0x63, 0xB6, 	// F8..FF
};

// Inverse S-Box

static const uint8_t kuz_pi_inv[0x100] = {
	0xA5, 0x2D, 0x32, 0x8F, 0x0E, 0x30, 0x38, 0xC0, 	// 00..07
	0x54, 0xE6, 0x9E, 0x39, 0x55, 0x7E, 0x52, 0x91, 	// 08..0F
	0x64, 0x03, 0x57, 0x5A, 0x1C, 0x60, 0x07, 0x18, 	// 10..17
	0x21, 0x72, 0xA8, 0xD1, 0x29, 0xC6, 0xA4, 0x3F, 	// 18..1F
	0xE0, 0x27, 0x8D, 0x0C, 0x82, 0xEA, 0xAE, 0xB4, 	// 20..27
	0x9A, 0x63, 0x49, 0xE5, 0x42, 0xE4, 0x15, 0xB7, 	// 28..2F
	0xC8, 0x06, 0x70, 0x9D, 0x41, 0x75, 0x19, 0xC9, 	// 30..37
	0xAA, 0xFC, 0x4D, 0xBF, 0x2A, 0x73, 0x84, 0xD5, 	// 38..3F
	0xC3, 0xAF, 0x2B, 0x86, 0xA7, 0xB1, 0xB2, 0x5B, 	// 40..47
	0x46, 0xD3, 0x9F, 0xFD, 0xD4, 0x0F, 0x9C, 0x2F, 	// 48..4F
	0x9B, 0x43, 0xEF, 0xD9, 0x79, 0xB6, 0x53, 0x7F, 	// 50..57
	0xC1, 0xF0, 0x23, 0xE7, 0x25, 0x5E, 0xB5, 0x1E, 	// 58..5F
	0xA2, 0xDF, 0xA6, 0xFE, 0xAC, 0x22, 0xF9, 0xE2, 	// 60..67
	0x4A, 0xBC, 0x35, 0xCA, 0xEE, 0x78, 0x05, 0x6B, 	// 68..6F
	0x51, 0xE1, 0x59, 0xA3, 0xF2, 0x71, 0x56, 0x11, 	// 70..77
	0x6A, 0x89, 0x94, 0x65, 0x8C, 0xBB, 0x77, 0x3C, 	// 78..7F
	0x7B, 0x28, 0xAB, 0xD2, 0x31, 0xDE, 0xC4, 0x5F, 	// 80..87
	0xCC, 0xCF, 0x76, 0x2C, 0xB8, 0xD8, 0x2E, 0x36, 	// 88..8F
	0xDB, 0x69, 0xB3, 0x14, 0x95, 0xBE, 0x62, 0xA1, 	// 90..97
	0x3B, 0x16, 0x66, 0xE9, 0x5C, 0x6C, 0x6D, 0xAD, 	// 98..9F
	0x37, 0x61, 0x4B, 0xB9, 0xE3, 0xBA, 0xF1, 0xA0, 	// A0..A7
	0x85, 0x83, 0xDA, 0x47, 0xC5, 0xB0, 0x33, 0xFA, 	// A8..AF
	0x96, 0x6F, 0x6E, 0xC2, 0xF6, 0x50, 0xFF, 0x5D, 	// B0..B7
	0xA9, 0x8E, 0x17, 0x1B, 0x97, 0x7D, 0xEC, 0x58, 	// B8..BF
	0xF7, 0x1F, 0xFB, 0x7C, 0x09, 0x0D, 0x7A, 0x67, 	// C0..C7
	0x45, 0x87, 0xDC, 0xE8, 0x4F, 0x1D, 0x4E, 0x04, 	// C8..CF
	0xEB, 0xF8, 0xF3, 0x3E, 0x3D, 0xBD, 0x8A, 0x88, 	// D0..D7
	0xDD, 0xCD, 0x0B, 0x13, 0x98, 0x02, 0x93, 0x80, 	// D8..DF
	0x90, 0xD0, 0x24, 0x34, 0xCB, 0xED, 0xF4, 0xCE, 	// E0..E7
	0x99, 0x10, 0x44, 0x40, 0x92, 0x3A, 0x01, 0x26, 	// E8..EF
	0x12, 0x1A, 0x48, 0x68, 0xF5, 0x81, 0x8B, 0xC7, 	// F0..F7
	0xD6, 0x20, 0x0A, 0x08, 0x00, 0x4C, 0xD7, 0x74	 	// F8..FF
};

// Х преобразование: сложение по модуля 2 векторов a b в вектор с
static void GOST_Kuz_X(const uint8_t* a, const uint8_t* b, uint8_t* c)
{
	int i;
	for (i = 0; i < BLOCK_SIZE; i++)
		c[i] = a[i] ^ b[i];
}

// нелинейное преобразование S. Берем массив pi и по нему считаем выходные данные
// данные 128 бит (16 байт байта) 
// каждый байт преобразуем по массиву pi и получаем выходной вектор
static void GOST_Kuz_S(const uint8_t* in_data, uint8_t* out_data)
{
	int i;
	for (i = 0; i < BLOCK_SIZE; i++)
		out_data[i] = kuz_pi[in_data[i]];
}

//   Умножение полинома
static uint8_t GOST_Kuz_GF_mul(uint8_t a, uint8_t b)
{
	uint8_t c = 0;
	uint8_t hi_bit;
	int i;
	for (i = 0; i < 8; i++)
	{
		if (b & 1)
			c ^= a;
		hi_bit = a & 0x80;
		a <<= 1;
		if (hi_bit)
			a ^= 0xc3; // Полином x^8 + x^7 + x^6 + x + 1
		b >>= 1;
	}
	return c;
}

// Вектор l с коэффициентами для умножения полинома (та самая константа)
static const unsigned char l_vec[16] = {
	1, 148, 32, 133, 16, 194, 192, 1,
	251, 1, 192, 194, 16, 133, 32, 148
};


// Преобразование R, в котором мы полиномиально умножаем каждый байт с исполльзованием одной константы из l вектора, сдвигаем и записываем результат
static void GOST_Kuz_R(uint8_t* state) {
	int i;
	uint8_t a_15 = 0;
	vect internal;
	for (i = 15; i >= 0; i--) {
		internal[i - 1] = state[i];// Двигаем байты в сторону младшего разряда
		a_15 ^= GOST_Kuz_GF_mul(state[i], l_vec[i]);
	}
	// Пишем в последний байт результат сложения
	internal[15] = a_15;
	memcpy(state, internal, BLOCK_SIZE);
}


// L преобразование 16 раз применяет преобразование R
static void GOST_Kuz_L(const uint8_t* in_data, uint8_t* out_data)
{
	int i;
	vect internal;
	memcpy(internal, in_data, BLOCK_SIZE);
	for (i = 0; i < 16; i++) {
		GOST_Kuz_R(internal);
	}
	memcpy(out_data, internal, BLOCK_SIZE);
}


// нам необходимо 10 итерационных ключей и 32 итерациооные константы
vect iter_C[32];// Итерационные константы C

vect iter_num[32];// Сюда будем писать номер итерации от 1 до 32

// описание функции:
//void *memset(void *buf, int ch, size_t count)
//Функция memset() копирует младший байт ch в первые count символов массива, на который указывает buf.Функция возвращает buf.


// Вопрос: не совсем понятно как мы заполняем массив iter_num.... нулями или по возрастанию... что в него пишем?
// массив С заполняем итерационными константами, которые потом будем использовать в 
void GOST_Kuz_Get_C() {
	for (int i = 0; i < 32; i++) {
		memset(iter_num[i], 0, BLOCK_SIZE);
		iter_num[i][0] = i + 1;
	}
	// Делаем преобразование L для каждого элемента массива iter_num
	for (int i = 0; i < 32; i++) {
		GOST_Kuz_L(iter_num[i], iter_C[i]);
	}
}


// Функция F - одна операция развертывания
//Для того чтобы получить нужные нам десять итерационных (раундовых) ключей, нужно прокрутить функцию GOST_Kuz_F 32 раза.
static void GOST_Kuz_F(const uint8_t* in_key_1, const uint8_t* in_key_2,
	uint8_t* out_key_1, uint8_t* out_key_2,
	uint8_t* iter_const) {
	vect internal;
	memcpy(out_key_2, in_key_1, BLOCK_SIZE);
	GOST_Kuz_X(in_key_1, iter_const, internal);
	GOST_Kuz_S(internal, internal);
	GOST_Kuz_L(internal, internal);
	GOST_Kuz_X(internal, in_key_2, out_key_1);
}


// Здесь мы будем хранить итерациооные ключи шифрования. Это векторы по 16 байт. их 10 штук
vect iter_key[10]; // Итерационные ключи шифрования


// void * memcpy( void * destptr, const void * srcptr, size_t num );
//destptr- Указатель на блок памяти назначения(куда будут копироваться байты данных), имеет тип данных void.
//srcptr - Указатель на блок памяти источник(т.е., откуда будут копироваться байты данных), имеет тип данных void.
//num - Количество копируемых байтов.

// Развертывание ключей
void GOST_Kuz_Expand_Key(const uint8_t* key_1, const uint8_t* key_2) {
	int i;
	// на первой итерации мы разбиваем master key надвое и получаем первую пару. Потом мы используем алгоритм, чтобы получать следующие пары ключей

	// Предыдущая пара ключей
	uint8_t iter_1[64];
	uint8_t iter_2[64];

	// Текущая пара ключей
	uint8_t iter_3[64];
	uint8_t iter_4[64];
	GOST_Kuz_Get_C(); // Вычисляем итерационные константы

	// Первые два итерационных ключа равны паре мастер-ключа
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


// Теперь попробуем расшифровать
// Те же функции но в обратную сторону


// Обратное R
static void GOST_Kuz_reverse_R(uint8_t* state)
{
	int i;
	uint8_t a_0;
	a_0 = state[15];
	vect internal;
	for (i = 0; i < 16; i++)
	{
		internal[i] = state[i - 1];// Двигаем все на старые места
		a_0 ^= GOST_Kuz_GF_mul(internal[i], l_vec[i]);
	}
	internal[0] = a_0;
	memcpy(state, internal, BLOCK_SIZE);
}

//Обратное линейное преобразование (обратное преобразование L)
static void GOST_Kuz_reverse_L(const uint8_t* in_data, uint8_t* out_data)
{
	int i;
	vect internal;
	memcpy(internal, in_data, BLOCK_SIZE);
	for (i = 0; i < 16; i++)
		GOST_Kuz_reverse_R(internal);
	memcpy(out_data, internal, BLOCK_SIZE);
}

//Обратное нелинейное биективное преобразование (обратное преобразование S)
static void GOST_Kuz_reverse_S(const uint8_t* in_data, uint8_t* out_data)
{
	int i;
	for (i = 0; i < BLOCK_SIZE; i++)
		out_data[i] = kuz_pi_inv[in_data[i]];
}




// И о чудо мы наконец-то начали зашифровывать....
// Что мы делаем?
// 1. Вызываем  GOST_Kuz_Expand_Key, чтобы заполнить массив итерационными ключами iter_key[10]
// 2. На вход получаем блок, который надо зашифровать
// 3. У нас 9 раундов, поэтому в цикле мы поочередно применяем преобразование Х (сложение по модуля 2), S (нелинейное пр-е с массивм pi)  и L (в котором мы 16 раз применям функцию R с умножением полиномов)
// Вопрос: не понятно зачем нам последняя строчка сложения....?

void GOST_Kuz_Encript(const uint8_t* blk, uint8_t* out_blk, uint8_t* key256b) {
	int i;
	uint8_t* key1 = NULL, key2 = NULL;
	memcpy(out_blk, blk, BLOCK_SIZE);
	// Вот тут не уверена как мы должны передать первые ключи. По сути это первый мастре ключ пополам но хз
	// тут надо заполнить массив ключей
	static uint8_t key_1[16] =
	{ 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00, (uint8_t)0xff, (uint8_t)0xee,
	(uint8_t)0xdd, (uint8_t)0xcc, (uint8_t)0xbb, (uint8_t)0xaa, (uint8_t)0x99, (uint8_t)0x88 };

	static uint8_t key_2[16] =
	{ (uint8_t)0xef, (uint8_t)0xcd, (uint8_t)0xab, (uint8_t)0x89, 0x67, 0x45, 0x23, 0x01,
	0x10, 0x32, 0x54, 0x76, (uint8_t)0x98, (uint8_t)0xba, (uint8_t)0xdc, (uint8_t)0xfe };
	uint8_t encrypted[BUFF_SIZE], decrypted[BUFF_SIZE];

	GOST_Kuz_Expand_Key(key_1, key_2);
	for (int i = 0; i < 16; i++) {
		printf("%0x ", key1[i]);
	}

	for (i = 0; i < 9; i++) {
		GOST_Kuz_X(iter_key[i], out_blk, out_blk);
		GOST_Kuz_S(out_blk, out_blk);
		GOST_Kuz_L(out_blk, out_blk);
	}
	GOST_Kuz_X(out_blk, iter_key[9], out_blk);
}


// Расшифровываем
void GOST_Kuz_Decript(const uint8_t* blk, uint8_t* out_blk) {
	int i;
	memcpy(out_blk, blk, BLOCK_SIZE);

	GOST_Kuz_X(out_blk, iter_key[9], out_blk);
	for (i = 8; i >= 0; i--)
	{
		GOST_Kuz_reverse_L(out_blk, out_blk);
		GOST_Kuz_reverse_S(out_blk, out_blk);
		GOST_Kuz_X(iter_key[i], out_blk, out_blk);
	}
}

static inline void print_array_dec(uint8_t* array, size_t length) {
	printf("[ ");
	for (size_t i = 0; i < length; ++i)
		printf("%d ", array[i]);
	printf("]\n");
}

static inline void print_array_hex(uint8_t* array, size_t length) {
	printf("[ ");
	for (size_t i = 0; i < length; ++i)
		printf("%h ", array[i]);
	printf("]\n");
}

int main() {
	uint8_t key256b[32] = "this_is_a_pasw_for_GOST_28147_89";
	uint8_t buffer[BUFF_SIZE], ch, outBlock[BUFF_SIZE];
	size_t position = 0;
	while ((ch = getchar()) != '\n' && position < BUFF_SIZE - 1)
		buffer[position++] = ch;
	buffer[position] = '\0';

	printf("Open message:\n");
	print_array_dec(buffer, position);
	printf("%s\n", buffer);
	putchar('\n');

	GOST_Kuz_Encript(buffer, outBlock, key256b);

	printf("Encrypted message:\n");
	print_array_dec(outBlock, position);
	printf("%s\n", outBlock);
	putchar('\n');




	//const uint8_t block[32] = {
	//0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
	//0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
	//0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
	//0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
	//};
	//vect outBlock[BLOCK_SIZE];

	//GOST_Kuz_Encript(block, outBlock, key256b);
}