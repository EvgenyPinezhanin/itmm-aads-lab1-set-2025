#include "BitField.h"
#include <iostream>
static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);
static const int cBITS = sizeof(TELEM) * 8;

TBitField::TBitField(int len)
{
    if (len < 0)
        throw std::invalid_argument("Negative length");
    BitLen = len;
    MemLen = (BitLen + cBITS - 1) / cBITS;
    pMem = new TELEM[MemLen];
    for (int i = 0; i < MemLen; i++) {
        pMem[i] = 0;
    }
   
}
 

TBitField::TBitField(const TBitField& bf) // конструктор копирования
{
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    pMem = new TELEM[MemLen];
    for (int i = 0; i < MemLen; i++) {
        pMem[i] = bf.pMem[i];
    }
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
    TELEM bek = 1;
    bek <<= (n % cBITS);
    return bek;
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
    return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
    if (n < 0 || n >= BitLen)
        throw std::out_of_range("Bit position out of range");

    int index = GetMemIndex(n);
    TELEM bek = GetMemMask(n);

    pMem[index] |= bek;
}

void TBitField::ClrBit(const int n) // очистить бит
{
    if (n < 0 || n >= BitLen)
        throw std::out_of_range("Bit position out of range");

    int index = GetMemIndex(n);
    TELEM bek = GetMemMask(n);

    pMem[index] &= ~bek;
}

int TBitField::GetBit(const int n) const // получить значение бита
{
    if (n < 0 || n >= BitLen)
        throw std::out_of_range("Bit position out of range");

    int index = GetMemIndex(n);
    TELEM bek = GetMemMask(n);

    return (pMem[index] & bek) ? 1 : 0;
}

// битовые операции

TBitField& TBitField::operator=(const TBitField& bf) // присваивание
{
    if (this == &bf)
        return *this;

    if (MemLen != bf.MemLen) {
        delete[] pMem;
        MemLen = bf.MemLen;
        pMem = new TELEM[MemLen];
    }

    BitLen = bf.BitLen;
    for (int i = 0; i < MemLen; i++) {
        pMem[i] = bf.pMem[i];
    }

    return *this;
}

int TBitField::operator==(const TBitField& bf) const // сравнение
{
    if (BitLen != bf.BitLen)
        return 0;

    for (int i = 0; i < MemLen; i++) {
        if (pMem[i] != bf.pMem[i])
            return 0;
    }

    return 1;
}

int TBitField::operator!=(const TBitField& bf) const // сравнение
{
    return !(*this == bf);
}

TBitField TBitField::operator|(const TBitField& bf) // операция "или"
{
    int resultLen = (BitLen > bf.BitLen) ? BitLen : bf.BitLen;
    int resultMemLen = (resultLen + cBITS - 1) / cBITS;
    if (resultMemLen == 0) resultMemLen = 1;

    TBitField result(resultLen);

    for (int i = 0; i < resultMemLen; i++) {
        TELEM a = (i < MemLen) ? pMem[i] : 0;
        TELEM b = (i < bf.MemLen) ? bf.pMem[i] : 0;
        result.pMem[i] = a | b;
    }

    return result;
}

TBitField TBitField::operator&(const TBitField& bf) // операция "и"
{
    int resultLen = (BitLen > bf.BitLen) ? BitLen : bf.BitLen;
    int resultMemLen = (resultLen + cBITS - 1) / cBITS;
    if (resultMemLen == 0) resultMemLen = 1;

    TBitField result(resultLen);

    for (int i = 0; i < resultMemLen; i++) {
        TELEM a = (i < MemLen) ? pMem[i] : 0;
        TELEM b = (i < bf.MemLen) ? bf.pMem[i] : 0;
        result.pMem[i] = a & b;
    }

    return result;
}

TBitField TBitField::operator~(void) // отрицание
{
    TBitField result(BitLen);

    for (int i = 0; i < MemLen; i++) {
        result.pMem[i] = ~pMem[i];
    }

    int extraBits = (MemLen * (sizeof(TELEM) * 8)) - BitLen;
    if (extraBits > 0) {
        TELEM mask = (TELEM)(-1);
        mask >>= extraBits;
        result.pMem[MemLen - 1] &= mask;
    }

    return result;
}

// ввод/вывод

istream& operator>>(istream& istr, TBitField& bf) // ввод
{
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
    int BitLen = bf.GetLength();

    for (int i = 0; i < strLen && i < BitLen; i++) {
        if (input[i] == '1') {
            int pos = BitLen - 1 - i;
            bf.SetBit(pos);
        }
    }

    return istr;
}

ostream& operator<<(ostream& ostr, const TBitField& bf) // вывод
{
    for (int i = bf.GetLength() - 1; i >= 0; i--) {
        ostr << (bf.GetBit(i) ? '1' : '0');
    }
    return ostr;
}