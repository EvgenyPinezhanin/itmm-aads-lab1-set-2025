// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tset.cpp - Copyright (c) Гергель В.П. 04.10.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Множество - реализация через битовые поля

#include "tset.h"
#include <stdexcept>
#include <algorithm>

TSet::TSet(int mp) : MaxPower(mp), BitField(mp)
{
    if (mp <= 0) {
        throw std::invalid_argument("Set power must be positive");
    }
}

// конструктор копирования
TSet::TSet(const TSet& s) : MaxPower(s.MaxPower), BitField(s.BitField)
{
}

// конструктор преобразования типа
TSet::TSet(const TBitField& bf) : MaxPower(bf.GetLength()), BitField(bf)
{
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

TSet& TSet::operator=(const TSet& s)
{
    if (this != &s) {
        MaxPower = s.MaxPower;
        BitField = s.BitField;
    }
    return *this;
}

int TSet::operator==(const TSet& s) const
{
    if (MaxPower != s.MaxPower) {
        return 0;
    }
    return BitField == s.BitField;
}

int TSet::operator!=(const TSet& s) const
{
    return !(*this == s);
}

TSet TSet::operator+(const TSet& s)
{
    int maxPower = std::max(MaxPower, s.MaxPower);
    TSet result(maxPower);

    result.BitField = BitField | s.BitField;
    return result;
}

TSet TSet::operator+(const int Elem)
{
    if (Elem < 0) {
        throw std::out_of_range("Element index out of range");
    }
 
    if (Elem >= MaxPower) {
        throw std::out_of_range("Element index out of range");
    }

    TSet result(*this);
    result.InsElem(Elem);
    return result;
}

TSet TSet::operator-(const int Elem)
{
    if (Elem < 0) {
        throw std::out_of_range("Element index out of range");
    }

    if (Elem >= MaxPower) {
        return *this;
    }

    TSet result(*this);
    result.DelElem(Elem);
    return result;
}

TSet TSet::operator*(const TSet& s)
{
    int maxPower = std::max(MaxPower, s.MaxPower);
    TSet result(maxPower);

    int minPower = std::min(MaxPower, s.MaxPower);
    for (int i = 0; i < minPower; i++) {
        if (IsMember(i) && s.IsMember(i)) {
            result.InsElem(i);
        }
    }

    return result;
}

TSet TSet::operator~(void)
{
    TSet result(MaxPower);
    result.BitField = ~BitField;

    // Корректировка последних битов
    for (int i = MaxPower; i < result.MaxPower; i++) {
        result.BitField.ClrBit(i);
    }

    return result;
}

// перегрузка ввода/вывода
istream& operator>>(istream& istr, TSet& s)
{
    int maxPower;
    istr >> maxPower;

    if (maxPower <= 0 || istr.fail()) {
        istr.setstate(ios::failbit);
        return istr;
    }

    int elementCount;
    istr >> elementCount;

    if (elementCount < 0 || elementCount > maxPower || istr.fail()) {
        istr.setstate(ios::failbit);
        return istr;
    }

    TSet temp(maxPower);
    for (int i = 0; i < elementCount; i++) {
        int elem;
        istr >> elem;

        if (elem < 0 || elem >= maxPower || istr.fail()) {
            istr.setstate(ios::failbit);
            return istr;
        }
        temp.InsElem(elem);
    }

    s = temp;
    return istr;
}

ostream& operator<<(ostream& ostr, const TSet& s)
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