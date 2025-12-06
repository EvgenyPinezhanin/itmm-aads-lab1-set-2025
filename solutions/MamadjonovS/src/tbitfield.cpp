#include "tbitfield.h"
#include <stdexcept>
#include <cstring>

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

    for (int i = 0; i < MemLen; i++) {
        pMem[i] = 0;
    }
}


TBitField::TBitField(const TBitField& bf) : BitLen(bf.BitLen), MemLen(bf.MemLen), pMem(nullptr) // конструктор копировани€
{
    pMem = new TELEM[MemLen];
    for (int i = 0; i < MemLen; i++) {
        pMem[i] = bf.pMem[i];
    }
}

TBitField::~TBitField()
{
    delete[] pMem;
    pMem = nullptr;
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
        return 0;
    }

    for (int i = 0; i < MemLen; i++) {
        if (pMem[i] != bf.pMem[i]) {
            return 0;
        }
    }

    return 1;
}

int TBitField::operator!=(const TBitField& bf) const // сравнение
{
    return !(*this == bf);
}

TBitField TBitField::operator|(const TBitField& bf) // операци€ "или"
{
    int maxLen = (BitLen > bf.BitLen) ? BitLen : bf.BitLen;
    TBitField result(maxLen);

    //  опируем биты из первого пол€
    for (int i = 0; i < BitLen; i++) {
        if (GetBit(i)) {
            result.SetBit(i);
        }
    }

    // ƒобавл€ем биты из второго пол€
    for (int i = 0; i < bf.BitLen; i++) {
        if (bf.GetBit(i)) {
            result.SetBit(i);
        }
    }

    return result;
}

TBitField TBitField::operator&(const TBitField& bf) // операци€ "и"
{
    int minLen = (BitLen < bf.BitLen) ? BitLen : bf.BitLen;
    TBitField result(minLen);

    for (int i = 0; i < minLen; i++) {
        if (GetBit(i) && bf.GetBit(i)) {
            result.SetBit(i);
        }
    }

    return result;
}

TBitField TBitField::operator~(void) // отрицание
{
    TBitField result(BitLen);

    for (int i = 0; i < MemLen - 1; i++) {
        result.pMem[i] = ~pMem[i];
    }

    // ќсобый случай дл€ последнего элемента
    if (MemLen > 0) {
        TELEM mask = (BitLen % cBITS == 0) ?
            static_cast<TELEM>(-1) :
            (static_cast<TELEM>(1) << (BitLen % cBITS)) - 1;
        result.pMem[MemLen - 1] = (~pMem[MemLen - 1]) & mask;
    }

    return result;
}

// ввод/вывод

istream& operator>>(istream& istr, TBitField& bf) // ввод
{
    int length;
    istr >> length;

    if (length <= 0) {
        istr.setstate(std::ios::failbit);
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
            istr.setstate(std::ios::failbit);
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
        try {
            ostr << (bf.GetBit(i) ? '1' : '0');
        }
        catch (...) {
            ostr << '?';
        }
    }

    return ostr;
}