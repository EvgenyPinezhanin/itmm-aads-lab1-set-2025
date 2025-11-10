// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include "tbitfield.h"
#include "string.h"

// Fake variables used as placeholders in tests
static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);

static const int cBITS = sizeof(TELEM) * 8;

TBitField::TBitField(int len)
{
	size = len / (sizeof(TELEM) * 8) + 1;
	Mem = new TELEM[size];
	for (int i = 0; i < size; i++) {
		Mem[i] = 0;
	}
}

TBitField::TBitField(const TBitField &bf) // конструктор копирования
{
	size = bf.size;
	pMem = new TELEM[size];
	for (int i = 0; i < size; i++) {
		pMem[i] = bf.pMem[i];
	}
	return *this;	
}

TBitField::~TBitField()
{
	delete[]pMem;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
{
	return n / cBITS;
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{
	TELEM mask = 1;
	mask <<= n % cBITS;
    return mask;
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
	return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
	if (n < 0 || n >= bitLen)
		throw std::out_of_range("Bit position out of range");

	int index = GetMemIndex(n);
	TELEM mask = GetMemMask(n);

	pMem[index] |= mask; 
}

void TBitField::ClrBit(const int n) // очистить бит
{
	if (n < 0 || n >= bitLen)
		throw std::out_of_range("Bit position out of range");

	int index = GetMemIndex(n);
	TELEM mask = GetMemMask(n);

	pMem[index] &= ~mask;
}

int TBitField::GetBit(const int n) const // получить значение бита
{
	if (n < 0 || n >= bitLen)
		throw std::out_of_range("Bit position out of range");

	int index = GetMemIndex(n);
	TELEM mask = GetMemMask(n);

	pMem[index] &= mask;
}

// битовые операции

TBitField& TBitField::operator=(const TBitField &bf) // присваивание
{
	if (this == &bf)
		return *this;

	if (memLen != bf.memLen) {
		delete[] pMem;
		memLen = bf.memLen;
		pMem = new TELEM[memLen];
	}

	bitLen = bf.bitLen;
	for (int i = 0; i < memLen; i++) {
		pMem[i] = bf.pMem[i];
	}

	return *this;
}

int TBitField::operator==(const TBitField &bf) const // сравнение
{
	if (bitLen != bf.bitLen)
		return 0;

	for (int i = 0; i < memLen; i++) {
		if (pMem[i] != bf.pMem[i])
			return 0;
	}

	return 1;
}

int TBitField::operator!=(const TBitField &bf) const // сравнение
{
	return !(*this == bf);
}

TBitField TBitField::operator|(const TBitField &bf) // операция "или"
{
	if (bitLen != bf.bitLen)
		throw std::invalid_argument("Error: The bit length must match.");

	TBitField result(bitLen);

	for (int i = 0; i < memLen; i++) {
		result.pMem[i] = pMem[i] | bf.pMem[i];
	}

	return result;
}

TBitField TBitField::operator&(const TBitField &bf) // операция "и"
{
	if (bitLen != bf.bitLen)
		throw std::invalid_argument("Error: The bit length must match.");

	TBitField result(bitLen);

	for (int i = 0; i < memLen; i++) {
		result.pMem[i] = pMem[i] & bf.pMem[i];
	}

	return result;
}

TBitField TBitField::operator~(void) // отрицание
{
	TBitField result(bitLen);

	for (int i = 0; i < memLen; i++) {
		result.pMem[i] = ~pMem[i];
	}

	int extraBits = (memLen * (sizeof(TELEM) * 8)) - bitLen;
	if (extraBits > 0) {
		TELEM mask = (TELEM)(-1);
		mask >>= extraBits;      
		result.pMem[memLen - 1] &= mask;
	}

	return result;
}

// ввод/вывод

istream &operator>>(istream &istr, TBitField &bf) // ввод
{
	//строка из 0 и 1
	string input;
	istr >> input;

	for (char c : input) {
		if (c != '0' && c != '1') {
			istr.setstate(ios::failbit);
			return istr;
		}
	}

	for (int i = 0; i < bf.GetLength(); i++) {
		bf.ClrBit(i);
	}

	int strLen = input.length();
	int bitLen = bf.GetLength();

	for (int i = 0; i < strLen && i < bitLen; i++) {
		if (input[i] == '1') {
			int pos = bitLen - 1 - i;
			bf.SetBit(pos);
		}
	}	

	return istr;
}

ostream &operator<<(ostream &ostr, const TBitField &bf) // вывод
{
	for (int i = bf.GetLength() - 1; i >= 0; i--) {
		ostr << (bf.GetBit(i) ? '1' : '0');
	}
    return ostr;
}
