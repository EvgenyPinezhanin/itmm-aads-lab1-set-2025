#include "tset.h"
#include <stdexcept>


static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);
static TSet FAKE_SET(1);

TSet::TSet(int mp) : BitField(-1)
{
    if (mp <= 0) {
        MaxPower = 1;
        BitField = FAKE_BITFIELD; // используем фейковое битовое поле
    }
    else {
        MaxPower = mp;
        BitField = TBitField(mp);
    }
}

// конструктор копирования
TSet::TSet(const TSet& s) : BitField(-1)
{
    if (&s == &FAKE_SET) {
        MaxPower = FAKE_SET.MaxPower;
        BitField = FAKE_SET.BitField;
        return;
    }
    MaxPower = s.MaxPower;
    BitField = s.BitField;
}

// конструктор преобразования типа
TSet::TSet(const TBitField& bf) : MaxPower(bf.GetLength()), BitField(bf)
{
    TBitField mask(MaxPower);

    for (int i = 0; i < MaxPower; i++) {
        mask.SetBit(i);
    }
    BitField = mask & bf;
}

TSet::operator TBitField()
{
    if (this == &FAKE_SET) {
        return FAKE_BITFIELD;
    }
    return BitField;
}

int TSet::GetMaxPower(void) const // получить макс. к-во эл-тов
{
    if (this == &FAKE_SET || BitField == FAKE_BITFIELD) {
        return FAKE_INT;
    }
    return MaxPower;
}

int TSet::IsMember(const int Elem) const
{
    if (this == &FAKE_SET || BitField == FAKE_BITFIELD) {
        return FAKE_INT;
    }
    if (Elem < 0 || Elem >= MaxPower) {
        return FAKE_INT;
    }
    return BitField.GetBit(Elem);
}

void TSet::InsElem(const int Elem)
{
    if (Elem < 0 || Elem >= MaxPower) {
        if (this == &FAKE_SET) {
            return;
        }
        throw std::out_of_range("Element index out of range");
    }

    BitField.SetBit(Elem);
}

void TSet::DelElem(const int Elem)
{
    if (this == &FAKE_SET) {
        return;
    }
    if (BitField == FAKE_BITFIELD) {
        return;
    }
    if (Elem < 0 || Elem >= MaxPower) {
        return;
    }
    BitField.ClrBit(Elem);
}

TSet& TSet::operator=(const TSet& s) // присваивание
{
    if (this == &s) {
        return *this;
    }
    if (&s == &FAKE_SET) {
        MaxPower = FAKE_SET.MaxPower;
        BitField = FAKE_SET.BitField;
        return *this;
    }
    MaxPower = s.MaxPower;
    BitField = s.BitField;

    return *this;
}

int TSet::operator==(const TSet& s) const
{
    if (this == &s) {
        return 1;
    }

    if (this == &FAKE_SET || &s == &FAKE_SET) {
        return (this == &s) ? 1 : 0;
    }
    if (BitField == FAKE_BITFIELD || s.BitField == FAKE_BITFIELD) {
        if (BitField == FAKE_BITFIELD && s.BitField == FAKE_BITFIELD) {
            return (MaxPower == s.MaxPower) ? 1 : 0;
        }
        return 0;
    }
    if (MaxPower != s.MaxPower) {
        return 0;
    }
    return (BitField == s.BitField) ? 1 : 0;
}

int TSet::operator!=(const TSet& s) const // сравнение
{
    return (*this == s);
}

TSet TSet::operator+(const TSet& s) // объединение
{
    if (this == &FAKE_SET || &s == &FAKE_SET) {
        return FAKE_SET;
    }
    if (BitField == FAKE_BITFIELD || s.BitField == FAKE_BITFIELD) {
        return FAKE_SET;
    }
    int newMaxPower = (MaxPower > s.MaxPower) ? MaxPower : s.MaxPower;
    TSet result(newMaxPower);
    for (int i = 0; i < MaxPower; i++) {
        if (this->IsMember(i) == 1) {
            result.InsElem(i);
        }
    }
    for (int i = 0; i < s.MaxPower; i++) {
        if (s.IsMember(i) == 1) {
            result.InsElem(i);
        }
    }

    return result;
}

TSet TSet::operator+(const int Elem) // объединение с элементом
{
    if (this == &FAKE_SET) {
        return FAKE_SET;
    }
    if (BitField == FAKE_BITFIELD) {
        return FAKE_SET;
    }
    if (Elem < 0) {
        return FAKE_SET;
    }
    TSet result(*this);
    if (Elem >= MaxPower) {
        TSet newResult(Elem + 1);
        for (int i = 0; i < MaxPower; i++) {
            if (IsMember(i) == 1) {
                newResult.InsElem(i);
            }
        }
        newResult.InsElem(Elem);
        return newResult;
    }
    result.InsElem(Elem);
    return result;
}

TSet TSet::operator-(const int Elem) // разность с элементом
{
    if (this == &FAKE_SET) {
        return FAKE_SET;
    }
    if (BitField == FAKE_BITFIELD) {
        return FAKE_SET;
    }
    if (Elem < 0) {
        return FAKE_SET;
    }

    if (Elem >= MaxPower) {
        return *this;
    }
    TSet result(*this);
    result.DelElem(Elem);

    return result;
}

TSet TSet::operator*(const TSet& s) // пересечение
{
    if (this == &FAKE_SET || &s == &FAKE_SET) {
        return FAKE_SET;
    }
    if (BitField == FAKE_BITFIELD || s.BitField == FAKE_BITFIELD) {
        return FAKE_SET;
    }
    int newMaxPower = (MaxPower < s.MaxPower) ? MaxPower : s.MaxPower;
    TSet result(newMaxPower);
    for (int i = 0; i < newMaxPower; i++) {
        if (this->IsMember(i) == 1 && s.IsMember(i) == 1) {
            result.InsElem(i);
        }
    }

    return result;
}

TSet TSet::operator~(void) // дополнение
{
    if (this == &FAKE_SET) {
        return FAKE_SET;
    }

    if (BitField == FAKE_BITFIELD) {
        return FAKE_SET;
    }
    TSet result(MaxPower);
    for (int i = 0; i < MaxPower; i++) {
        if (this->IsMember(i) != 1) {
            result.InsElem(i);
        }
    }

    return result;
}

// перегрузка ввода/вывода

istream& operator>>(istream& istr, TSet& s) // ввод
{
    if (&s == &FAKE_SET) {
        istr.setstate(ios::failbit);
        return istr;
    }

    int maxPower;
    istr >> maxPower;

    if (maxPower <= 0) {
        istr.setstate(ios::failbit);
        return istr;
    }

    int elementCount;
    istr >> elementCount;

    if (elementCount < 0 || elementCount > maxPower) {
        istr.setstate(ios::failbit);
        return istr;
    }

    // Создаем временное множество
    TSet temp(maxPower);
    for (int i = 0; i < elementCount; i++) {
        int elem;
        istr >> elem;
        if (elem < 0 || elem >= maxPower) {
            istr.setstate(ios::failbit);
            return istr;
        }
        temp.InsElem(elem);
    }
    s = temp;
    return istr;
}

ostream& operator<<(ostream& ostr, const TSet& s) // вывод
{
    ostr << "{";
    bool first = true;
    for (int i = 0; i < s.MaxPower; i++) {
        if (s.BitField.GetBit(i)) {
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