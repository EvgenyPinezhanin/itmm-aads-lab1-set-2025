#include "tset.h"
#include <stdexcept>


static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);
static TSet FAKE_SET(1);

TSet::TSet(int mp) : BitField(mp), MaxPower(mp)
{
    if (mp <= 0) {
        throw std::invalid_argument("Set max power must be positive");
    }
}

TSet::TSet(const TSet& s) : BitField(s.BitField), MaxPower(s.MaxPower)
{
}

TSet::TSet(const TBitField& bf) :
    BitField(bf.GetLength()), MaxPower(bf.GetLength())
{
    for (int i = 0; i < MaxPower; i++) {
        if (bf.GetBit(i)) {
            BitField.SetBit(i);
        }
    }
}

TSet::operator TBitField()
{
    return BitField;
}

int TSet::GetMaxPower(void) const
{
    return MaxPower;
}

int TSet::IsMember(const int Elem) const  
{
    if (Elem < 0 || Elem >= MaxPower) {
        throw std::out_of_range("Element index out of range");
    }

    return BitField.GetBit(Elem);
}

void TSet::InsElem(const int Elem)  
{
    if (Elem < 0 || Elem >= MaxPower) {
        throw std::out_of_range("Element index out of range");
    }

    BitField.SetBit(Elem);
}

void TSet::DelElem(const int Elem)  
{
    if (Elem < 0 || Elem >= MaxPower) {
        throw std::out_of_range("Element index out of range");
    }

    BitField.ClrBit(Elem);
}

TSet& TSet::operator=(const TSet& s) // присваивание
{
    if (this == &s) {
        return *this;
    }

    MaxPower = s.MaxPower;
    BitField = s.BitField;

    return *this;
}

int TSet::operator==(const TSet& s) const  
{
    return (MaxPower == s.MaxPower && BitField == s.BitField);
}

int TSet::operator!=(const TSet& s) const // сравнение
{
    return (*this == s);
}

TSet TSet::operator+(const TSet& s) // объединение
{
    int newMaxPower = (MaxPower > s.MaxPower) ? MaxPower : s.MaxPower;
    TSet result(newMaxPower);

    for (int i = 0; i < MaxPower; i++) {
        if (IsMember(i)) {
            result.InsElem(i);
        }
    }

    for (int i = 0; i < s.MaxPower; i++) {
        if (s.IsMember(i)) {
            result.InsElem(i);
        }
    }

    return result;
}

TSet TSet::operator+(const int Elem) // объединение с элементом
{
    if (Elem < 0) {
        throw std::invalid_argument("Element cannot be negative");
    }

    int newMaxPower = (Elem >= MaxPower) ? (Elem + 1) : MaxPower;
    TSet result(newMaxPower);

    for (int i = 0; i < MaxPower; i++) {
        if (IsMember(i)) {
            result.InsElem(i);
        }
    }

    result.InsElem(Elem);

    return result;
}

TSet TSet::operator-(const int Elem) // разность с элементом
{
    if (Elem < 0 || Elem >= MaxPower) {
        throw std::out_of_range("Element index out of range");
    }

    TSet result(*this);
    result.DelElem(Elem);

    return result;
}

TSet TSet::operator*(const TSet& s) // пересечение
{
    int minMaxPower = (MaxPower < s.MaxPower) ? MaxPower : s.MaxPower;
    TSet result(minMaxPower);

    for (int i = 0; i < minMaxPower; i++) {
        if (IsMember(i) && s.IsMember(i)) {
            result.InsElem(i);
        }
    }

    return result;
}

TSet TSet::operator~(void) // дополнение
{
    TSet result(MaxPower);

    for (int i = 0; i < MaxPower; i++) {
        if (!IsMember(i)) {
            result.InsElem(i);
        }
    }

    return result;
}

// перегрузка ввода/вывода

std::istream& operator>>(istream& istr, TSet& s) // ввод
{
    int maxPower;
    istr >> maxPower;

    if (maxPower <= 0) {
        istr.setstate(std::ios::failbit);
        return istr;
    }

    TSet temp(maxPower);
    int element;

    while (istr >> element) {
        if (element < 0 || element >= maxPower) {
            istr.setstate(std::ios::failbit);
            return istr;
        }
        temp.InsElem(element);
    }

    if (!istr.eof()) {
        istr.clear();
    }

    s = temp;
    return istr;
}

ostream& operator<<(ostream& ostr, const TSet& s) // вывод
{
    ostr << "{";
    bool first = true;

    for (int i = 0; i < s.MaxPower; i++) {
        if (s.IsMember(i)) {
            if (!first) {
                ostr << ", ";
            }
            ostr << i;
            first = false;
        }
    }

    ostr << "}";
    return ostr;
}