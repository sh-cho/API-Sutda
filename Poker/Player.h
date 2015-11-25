#pragma once
#include "Card.h"

class Player
{
private:
	Card hand[2];
	int cardNum;

	int money;
	int jokbo;	//Á·º¸
				//1 = 38±¤¶¯
				//2 = 18±¤¶¯
				//3 = 13±¤¶¯
				//
				//4 = Àå¶¯
				//5 = 9¶¯
				//6 = 8¶¯
				//7 = 7¶¯
				//8 = 6¶¯
				//9 = 5¶¯
				//10 = 4¶¯
				//11 = 3¶¯
				//12 = 2¶¯
				//13 = »æ¶¯
				//
				//14 = ¾Ë¸® (1, 2)
				//15 = µ¶»ç (1, 4)
				//16 = ±¸»æ (1, 9)
				//17 = Àå»æ (1, 10)
				//18 = Àå»ç (4, 10)
				//19 = ¼¼·ú (4, 6)
				//
				//20 = °©¿À (9²ı)
				//21 = 8²ı
				//22 = 7²ı
				//23 = 6²ı
				//24 = 5²ı
				//25 = 6²ı
				//26 = 5²ı
				//27 = 4²ı
				//28 = 3²ı
				//29 = 2²ı
				//30 = 1²ı
				//31 = ¸ÁÅë
				//
				//Æ¯¼öÆĞ
				//10001 = ¾ÏÇà¾î»ç (4, 7) (¿­²ı µÎ°³)
				//10002 = ¶¯ÀâÀÌ (3, 7) (3±¤ + ¸äµÅÁö) -> Àå¶¯ Á¦¿ÜÇÑ ¶¯ ÀâÀ½
				//
				//10003 = ±¸»ç (4, 9) -> ¶¯ ¹ØÀ¸·Î Àç°æ±â
				//10004 = ¸ÛÅÖ±¸¸® ±¸»ç (µÑ´Ù ¸ÛÅÖ±¸¸®) -> Àå¶¯¹Ø ´Ù ¹«È¿, Àç°æ±â
				//
				//99998 = ÆÇº° Àü
				//99999 = ´ÙÀÌ
				//
				//100000 = ¿ÃÀÎ ?

public:
	//»ı¼ºÀÚ ¼Ò¸êÀÚ
	Player(void);
	~Player(void);

	//getter setter
	Card*	getHand();
	//void	setHand(); //»ç¿ë X
	int		getCardNum();
	void	setCardNum(int num);
	int		getMoney();
	void	setMoney(int mny);
	int		getJokbo();
	void	setJokbo(int jkbo);

	//¸Ş¼Òµå
	void addCard(Card cd);
	void clear();

	void addMoney(int mny);
	void subMoney(int mny);

	void evalJokbo();

	void half(int& pandon);
	void call(int& pandon, int callMoney, int myBetMoney);
};