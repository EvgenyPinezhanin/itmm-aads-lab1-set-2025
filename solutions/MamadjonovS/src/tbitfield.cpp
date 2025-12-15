// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле
#include "tbitfield.h"
#include <stdexcept>
#include <cstring>
#include <algorithm>

static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);
static const int cBITS = sizeof(TELEM) * 8;

TBitField::TBitField(int len) : BitLen(0), MemLen(0), pMem(nullptr)
{
    if (len <= 0) {
        throw std::invalid_argument("Bit field length must be positive");
    }

    BitLen = len;
    MemLen = (len + cBITS - 1) / cBITS;
    pMem = new TELEM[MemLen];

    std::fill(pMem, pMem + MemLen, 0);
}

TBitField::TBitField(const TBitField& bf) : BitLen(bf.BitLen), MemLen(bf.MemLen), pMem(nullptr)
{
    pMem = new TELEM[MemLen];
    std::copy(bf.pMem, bf.pMem + MemLen, pMem);
}

TBitField::~TBitField()
{
    delete[] pMem;
}

int TBitField::GetMemIndex(const int n) const
{
    if (n < 0 || n >= BitLen) {
        throw std::out_of_range("Bit index out of range");
    }
    return n / cBITS;
}

TELEM TBitField::GetMemMask(const int n) const
{
    if (n < 0 || n >= BitLen) {
        throw std::out_of_range("Bit index out of range");
    }
    int bitIndex = n % cBITS;
    return static_cast<TELEM>(1) << bitIndex;
}

int TBitField::GetLength(void) const
{
    return BitLen;
}

void TBitField::SetBit(const int n)
{
    if (n < 0 || n >= BitLen) {
        throw std::out_of_range("Bit index out of range");
    }

    int memIndex = n / cBITS;
    int bitIndex = n % cBITS;
    TELEM mask = static_cast<TELEM>(1) << bitIndex;
    pMem[memIndex] |= mask;
}

void TBitField::ClrBit(const int n)
{
    if (n < 0 || n >= BitLen) {
        throw std::out_of_range("Bit index out of range");
    }

    int memIndex = n / cBITS;
    int bitIndex = n % cBITS;
    TELEM mask = static_cast<TELEM>(1) << bitIndex;
    pMem[memIndex] &= ~mask;
}

int TBitField::GetBit(const int n) const
{
    if (n < 0 || n >= BitLen) {
        throw std::out_of_range("Bit index out of range");
    }

    int memIndex = n / cBITS;
    int bitIndex = n % cBITS;
    TELEM mask = static_cast<TELEM>(1) << bitIndex;
    return (pMem[memIndex] & mask) ? 1 : 0;
}

// битовые операции
TBitField& TBitField::operator=(const TBitField& bf)
{
    if (this != &bf) {
        if (BitLen != bf.BitLen) {
            delete[] pMem;
            BitLen = bf.BitLen;
            MemLen = bf.MemLen;
            pMem = new TELEM[MemLen];
        }
        std::copy(bf.pMem, bf.pMem + MemLen, pMem);
    }
    return *this;
}

int TBitField::operator==(const TBitField& bf) const
{
    if (BitLen != bf.BitLen) {
        return 0;
    }

    for (int i = 0; i < MemLen; i++) {
        if (pMem[i] != bf.pMem[i]) {
            return 0;
        }
    }

    return 1;
}

int TBitField::operator!=(const TBitField& bf) const
{
    return !(*this == bf);
}

TBitField TBitField::operator|(const TBitField& bf)
{
    int maxLen = std::max(BitLen, bf.BitLen);
    TBitField result(maxLen);

    for (int i = 0; i < MemLen; i++) {
        result.pMem[i] = pMem[i];
    }

    for (int i = 0; i < bf.MemLen; i++) {
        result.pMem[i] |= bf.pMem[i];
    }

    return result;
}

TBitField TBitField::operator&(const TBitField& bf)
{
    int maxLen = std::max(BitLen, bf.BitLen);
    TBitField result(maxLen);

    int minMemLen = std::min(MemLen, bf.MemLen);
    for (int i = 0; i < minMemLen; i++) {
        result.pMem[i] = pMem[i] & bf.pMem[i];
    }

    return result;
}

TBitField TBitField::operator~(void)
{
    TBitField result(BitLen);

    for (int i = 0; i < MemLen; i++) {
        result.pMem[i] = ~pMem[i];
    }

    // Обнуление лишних битов в последнем элементе
    if (BitLen % cBITS != 0) {
        TELEM mask = (static_cast<TELEM>(1) << (BitLen % cBITS)) - 1;
        result.pMem[MemLen - 1] &= mask;
    }

    return result;
}

// ввод/вывод
istream& operator>>(istream& istr, TBitField& bf)
{
    int length;
    istr >> length;

    if (length <= 0 || istr.fail()) {
        istr.setstate(ios::failbit);
        return istr;
    }

    TBitField temp(length);

    for (int i = 0; i < length; i++) {
        char ch;
        istr >> ch;

        if (!istr) {
            istr.setstate(ios::failbit);
            return istr;
        }

        if (ch == '1') {
            temp.SetBit(i);
        }
        else if (ch != '0') {
            istr.setstate(ios::failbit);
            return istr;
        }
    }

    bf = temp;
    return istr;
}

ostream& operator<<(ostream& ostr, const TBitField& bf)
{
    ostr << bf.BitLen << " ";

    for (int i = 0; i < bf.BitLen; i++) {
        ostr << bf.GetBit(i);
    }

    return ostr;
}