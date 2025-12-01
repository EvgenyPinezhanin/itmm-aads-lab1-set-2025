#include <iostream>
#ifndef __BITFIELD_H__
#define __BITFIELD_H__


using namespace std;
typedef unsigned int TELEM;
class TBitField
{
private:
	int BitLen; // длина битового поля - макс. к-во битов
	TELEM* pMem; // память для представления битового поля
	int MemLen; // к-во эл-тов Мем для представления бит.поля
	// методы реализации
	int GetMemIndex(const int n) const; // индекс в pМем для бита n
	TELEM GetMemMask(const int n) const; // битовая маска для бита n
	friend TBitField operator&(const TBitField&, const TBitField&);
public:
	TBitField(int len);
	TBitField(const TBitField& bf);
	~TBitField();
	// доступ к битам

	int GetLength(void) const; // получить длину (к-во битов)
	void SetBit(const int n); // установить бит
	void ClrBit(const int n); // очистить бит
	int GetBit(const int n) const; // получить значение бита
	// битовые операции
	int operator==(const TBitField& bf) const; // сравнение
	int operator!=(const TBitField& bf) const; // сравнение
	TBitField& operator=(const TBitField& bf); // присваивание
	TBitField operator|(const TBitField& bf); // операция "или"
	TBitField operator&(const TBitField& bf); // операция "и"
	TBitField operator~(void); // отрицание
	friend std::istream& operator>>(std::istream& istr, TBitField& bf);
	friend std::ostream& operator<<(std::ostream& ostr, const TBitField& bf);

};
#endif