#include "tbitfield.h"
#include <iostream>
static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);
static const int cBITS = sizeof(TELEM) * 8;

TBitField::TBitField(int len)
{
    if (len <= 0) {
        // Ѕросаем исключение вместо создани€ FAKE_BITFIELD
        throw std::invalid_argument("Bit field length must be positive");
    }

    BitLen = len;
    MemLen = (len + cBITS - 1) / cBITS;

    pMem = new TELEM[MemLen];
    for (int i = 0; i < MemLen; i++) {
        pMem[i] = 0;
    }
}


TBitField::TBitField(const TBitField& bf) // конструктор копировани€
{
    BitLen = bf.BitLen;                    
    MemLen = bf.MemLen;                    

    pMem = new TELEM[MemLen];               
    for (int i = 0; i < MemLen; i++)       
        pMem[i] = bf.pMem[i];
}

TBitField::~TBitField()
{
    if (pMem != nullptr) {
        delete[] pMem;
        pMem = nullptr;
    }
}

int TBitField::GetMemIndex(const int n) const  
{
    if (n < 0 || n >= BitLen) {
        return FAKE_INT;  
    }
    return n / cBITS;
}

TELEM TBitField::GetMemMask(const int n) const // битова€ маска дл€ бита n
{
    if (n < 0 || n >= BitLen) {
        return static_cast<TELEM>(FAKE_INT);  
    }
    int bitIndex = n % cBITS;
    return static_cast<TELEM>(1) << bitIndex;
}

int TBitField::GetLength(void) const  
{
    return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
    // ѕровер€ем, что бит n находитс€ в допустимых пределах
    if (n < 0 || n >= BitLen) {
        throw std::out_of_range("Bit index out of range");
    }

    // ¬ычисл€ем индекс элемента массива и позицию бита внутри элемента
    int memIndex = n / cBITS;         // индекс в массиве pMem
    int bitIndex = n % cBITS;         // позици€ бита внутри TELEM

    // —оздаем маску и устанавливаем бит
    TELEM mask = static_cast<TELEM>(1) << bitIndex;
    pMem[memIndex] |= mask;
}

void TBitField::ClrBit(const int n) // очистить бит
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

TBitField& TBitField::operator=(const TBitField& bf) // присваивание
{
    if (this == &bf) {
        return *this;
    }

    if (&bf == &FAKE_BITFIELD) {
        BitLen = FAKE_BITFIELD.BitLen;
        MemLen = FAKE_BITFIELD.MemLen;

        delete[] pMem;
        pMem = new TELEM[MemLen];
        for (int i = 0; i < MemLen; i++) {
            pMem[i] = FAKE_BITFIELD.pMem[i];
        }
        return *this;
    }
    delete[] pMem;
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;

    pMem = new TELEM[MemLen];
    for (int i = 0; i < MemLen; i++) {
        pMem[i] = bf.pMem[i];
    }

    return *this;
}

int TBitField::operator==(const TBitField& bf) const // сравнение
{
    if (BitLen != bf.BitLen) {
        return 0; // не равны
    }

    // —равниваем все элементы массива pMem
    for (int i = 0; i < MemLen; i++) {
        if (pMem[i] != bf.pMem[i]) {
            return 0;  
        }
    }

    return 1;  
}

int TBitField::operator!=(const TBitField& bf) const // сравнение
{
    if (this == &FAKE_BITFIELD || &bf == &FAKE_BITFIELD) {
        return (this == &bf) ? 0 : 1;
    }

    // ≈сли длины не равны, битовые пол€ не равны
    if (BitLen != bf.BitLen) {
        return 1;
    }

    for (int i = 0; i < MemLen; i++) {
        if (pMem[i] != bf.pMem[i]) {
            return 1; // нашли различие - не равны
        }
    }
    return 0;  
}

TBitField TBitField::operator|(const TBitField& bf) // операци€ "или"
{
    if (this == &FAKE_BITFIELD || &bf == &FAKE_BITFIELD) {
        return FAKE_BITFIELD;
    }
    int maxLen = (BitLen > bf.BitLen) ? BitLen : bf.BitLen;
    TBitField result(maxLen);
    int minMemLen = (MemLen < bf.MemLen) ? MemLen : bf.MemLen;
    for (int i = 0; i < minMemLen; i++) {
        result.pMem[i] = pMem[i] | bf.pMem[i];
    }
    if (MemLen > bf.MemLen) {
        for (int i = minMemLen; i < MemLen; i++) {
            result.pMem[i] = pMem[i];
        }
    }
    else if (bf.MemLen > MemLen) {
        for (int i = minMemLen; i < bf.MemLen; i++) {
            result.pMem[i] = bf.pMem[i];
        }
    }

    return result;
}

TBitField TBitField::operator&(const TBitField& bf) // операци€ "и"
{
    if (this == &FAKE_BITFIELD || &bf == &FAKE_BITFIELD) {
        return FAKE_BITFIELD;
    }

    int minLen = (BitLen < bf.BitLen) ? BitLen : bf.BitLen;
    TBitField result(minLen);

    int minMemLen = (MemLen < bf.MemLen) ? MemLen : bf.MemLen;

    for (int i = 0; i < minMemLen; i++) {
        result.pMem[i] = pMem[i] & bf.pMem[i];
    }

    if (minLen > 0 && (minLen % cBITS != 0)) {
        int lastBitIndex = minLen % cBITS;
        TELEM mask = (static_cast<TELEM>(1) << lastBitIndex) - 1;
        result.pMem[minMemLen - 1] &= mask;
    }

    return result;
}

TBitField TBitField::operator~(void) // отрицание
{
    if (this == &FAKE_BITFIELD) {
        return FAKE_BITFIELD;
    }

    TBitField result(BitLen);

    for (int i = 0; i < MemLen; i++) {
        result.pMem[i] = ~pMem[i];
    }

    if (BitLen > 0 && (BitLen % cBITS != 0)) {
        int lastBitIndex = BitLen % cBITS;
        TELEM mask = (static_cast<TELEM>(1) << lastBitIndex) - 1;
        result.pMem[MemLen - 1] &= mask;
    }

    return result;
}

// ввод/вывод

istream& operator>>(istream& istr, TBitField& bf) // ввод
{
    int length;
    istr >> length;

    if (length <= 0) {
        istr.setstate(ios::failbit);
        return istr;
    }

    TBitField temp(length);

    for (int i = 0; i < length; i++) {
        char ch;
        istr >> ch;

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

ostream& operator<<(ostream& ostr, const TBitField& bf) // вывод
{
    ostr << bf.BitLen << " ";

    for (int i = 0; i < bf.BitLen; i++) {
        int bitValue = bf.GetBit(i);

        if (bitValue == FAKE_INT) {
            ostr << '?';
        }
        else {
            ostr << (bitValue ? '1' : '0');
        }
    }
    return ostr;
}