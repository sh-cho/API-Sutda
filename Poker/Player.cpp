#include "Player.h"


//********************************
//	»ý¼ºÀÚ ¼Ò¸êÀÚ
//********************************

Player::Player(void)
{
}


Player::~Player(void)
{
}






//********************************
//	getter setter
//********************************
//hand
Card* Player::getHand()
{
	return hand;
}

//void Player::setHand()
//{
//	// »ç¿ë X
//}

//cardNum
int Player::getCardNum()
{
	return cardNum;
}

void Player::setCardNum(int num)
{
	cardNum = num;
}

//money
int Player::getMoney()
{
	return money;
}

void Player::setMoney(int mny)
{
	money = mny;
}

//jokbo
int Player::getJokbo()
{
	return jokbo;
}

void Player::setJokbo(int jkbo)
{
	jokbo = jkbo;
}





//********************************
//	¸Þ¼Òµå
//********************************

void Player::addCard(Card cd)
{
	switch (cardNum)
	{
	case 0:
		hand[0] = cd;
		cardNum++;
		break;

	case 1:
		hand[1] = cd;
		cardNum++;
		break;
	}
}

void Player::clear()
{
	cardNum = 0;
	jokbo = 99998;
}

void Player::addMoney(int mny)
{
	money += mny;
}

void Player::subMoney(int mny)
{
	money -= mny;
}

void Player::evalJokbo()
{
	Card tempHand[2];

	tempHand[0] = getHand()[0];
	tempHand[1] = getHand()[1];

	//¿À¸§Â÷¼øÀ¸·Î Á¤·Ä
	if (tempHand[0].getMonth() > tempHand[1].getMonth())
	{
		Card tempCard = tempHand[0];
		tempHand[0] = tempHand[1];
		tempHand[1] = tempCard;
	}


	//²ýÃ³¸®
	int ggt;
	ggt = tempHand[0].getMonth() + tempHand[1].getMonth();
	ggt = ggt % 10;

	switch (ggt)
	{ 
	case 0: //¸ÁÅë
		setJokbo(31);
		break;

	case 1: //ÇÑ²ý
		setJokbo(30);
		break;

	case 2: //µÎ²ý
		setJokbo(29);
		break;

	case 3: //¼¼²ý
		setJokbo(28);
		break;

	case 4: //³×²ý
		setJokbo(27);
		break;

	case 5: //´Ù¼¸²ý
		setJokbo(26);
		break;

	case 6: //¿©¼¸²ý
		setJokbo(25);
		break;

	case 7: //ÀÏ°ö²ý
		setJokbo(24);
		break;

	case 8: //¿©´ü²ý
		setJokbo(23);
		break;

	case 9: //°©¿À
		setJokbo(22);
		break;
	}
	

	//Á·º¸Ã³¸®
	//¼¼·ú
	if (tempHand[0].getMonth() == 4 && tempHand[1].getMonth() == 6)
		setJokbo(19);

	//Àå»ç
	if (tempHand[0].getMonth() == 4 && tempHand[1].getMonth() == 10)
		setJokbo(18);

	//Àå»æ
	if (tempHand[0].getMonth() == 1 && tempHand[1].getMonth() == 10)
		setJokbo(17);

	//±¸»æ
	if (tempHand[0].getMonth() == 1 && tempHand[1].getMonth() == 9)
		setJokbo(16);

	//µ¶»ç
	if (tempHand[0].getMonth() == 1 && tempHand[1].getMonth() == 4)
		setJokbo(15);

	//¾Ë¸®
	if (tempHand[0].getMonth() == 1 && tempHand[1].getMonth() == 2)
		setJokbo(14);


	//¶¯Ã³¸®
	//1¶¯
	if (tempHand[0].getMonth() == 1 && tempHand[1].getMonth() == 1)
		setJokbo(13);

	//2¶¯
	if (tempHand[0].getMonth() == 2 && tempHand[1].getMonth() == 2)
		setJokbo(12);

	//3¶¯
	if (tempHand[0].getMonth() == 3 && tempHand[1].getMonth() == 3)
		setJokbo(11);

	//4¶¯
	if (tempHand[0].getMonth() == 4 && tempHand[1].getMonth() == 4)
		setJokbo(10);

	//5¶¯
	if (tempHand[0].getMonth() == 5 && tempHand[1].getMonth() == 5)
		setJokbo(9);

	//6¶¯
	if (tempHand[0].getMonth() == 6 && tempHand[1].getMonth() == 6)
		setJokbo(8);

	//7¶¯
	if (tempHand[0].getMonth() == 7 && tempHand[1].getMonth() == 7)
		setJokbo(7);

	//8¶¯
	if (tempHand[0].getMonth() == 8 && tempHand[1].getMonth() == 8)
		setJokbo(6);

	//9¶¯
	if (tempHand[0].getMonth() == 9 && tempHand[1].getMonth() == 9)
		setJokbo(5);

	//10¶¯
	if (tempHand[0].getMonth() == 10 && tempHand[1].getMonth() == 10)
		setJokbo(4);


	//±¤¶¯Ã³¸®
	//13±¤¶¯
	if (tempHand[0].getMonth() == 1 && tempHand[1].getMonth() == 3 && tempHand[0].getIsSpecial() && tempHand[1].getIsSpecial())
		setJokbo(3);

	//18±¤¶¯
	if (tempHand[0].getMonth() == 1 && tempHand[1].getMonth() == 8 && tempHand[0].getIsSpecial() && tempHand[1].getIsSpecial())
		setJokbo(2);

	//¡Ú38±¤¶¯
	if (tempHand[0].getMonth() == 3 && tempHand[1].getMonth() == 8 && tempHand[0].getIsSpecial() && tempHand[1].getIsSpecial())
		setJokbo(3);

	
	//Æ¯¼öÆÐÃ³¸®
	//¾ÏÇà¾î»ç
	if (tempHand[0].getMonth() == 4 && tempHand[1].getMonth() == 7 && tempHand[0].getIsSpecial() && tempHand[1].getIsSpecial())
		setJokbo(10001);
	
	//¶¯ÀâÀÌ
	if (tempHand[0].getMonth() == 3 && tempHand[1].getMonth() == 7 && tempHand[0].getIsSpecial() && tempHand[1].getIsSpecial())
		setJokbo(10002);

	//±×³É 94
	if (tempHand[0].getMonth() == 4 && tempHand[1].getMonth() == 9)
		setJokbo(10003);

	//¸ÛÅÖ±¸¸® 94
	if (tempHand[0].getMonth() == 4 && tempHand[1].getMonth() == 9 && tempHand[0].getIsSpecial() && tempHand[1].getIsSpecial())
		setJokbo(10004);
}

void Player::half(int& pandon)
{
	subMoney(pandon/2);

	//¸¸¾à ¿À¸µÀÌ¸é.... 0¿øµÇ°í ±×¸¸Å­¸¸
	//´õÇØÁöµµ·Ï
	//±¸Çö
	//ÇØ¾ßÇÔ
	//¤Ì
	pandon += (pandon/2);
}

void Player::call(int& pandon, int callMoney, int myBetMoney)
{
	//ÄÝÀ» ÇÏ·Á¸é ÀüºÎ ´Ù °°Àº ±Ý¾×À» ³»¾ßµÊ
	//ex) 1, 2, 3, 4, 5 ¼øÀ¸·Î 10, 20, 30, 40, 50 ÀÌ·¸°Ô ³Â´Ù°í ÇÏ¸é
	//1ÀÌ ÄÝÇÏ·Á¸é 40 ³»°í 2°¡ ÄÝÇÏ·Á¸é 30³»°í.. ÀÌ·±½Ä

	//ÀÌ¶§µµ ¿À¸µÀÏ¶§ ¿¹¿ÜÃ³¸®
	subMoney(callMoney - myBetMoney);

	pandon += (callMoney - myBetMoney);
}