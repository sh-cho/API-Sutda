#include "Card.h"


//********************************
//	持失切 社瑚切
//********************************


Card::Card(void)
{
}

Card::~Card(void)
{
}

Card::Card(int mnth, bool sp)
{
	month = mnth;
	isSpecial = sp;
}






//********************************
//	getter setter
//********************************
//month
int Card::getMonth()
{
	return month;
}

void Card::setMonth(int mnth)
{
	month = mnth;
}

//isSpecial
bool Card::getIsSpecial()
{
	return isSpecial;
}

void Card::setIsSpecial(bool sp)
{
	isSpecial = sp;
}






//********************************
//	五社球
//********************************

