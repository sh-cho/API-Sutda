#pragma once
#include "Card.h"

class Player
{
private:
	Card hand[2];
	int cardNum;

	int money;
	int jokbo;	//����
				//1 = 38����
				//2 = 18����
				//3 = 13����
				//
				//4 = �嶯
				//5 = 9��
				//6 = 8��
				//7 = 7��
				//8 = 6��
				//9 = 5��
				//10 = 4��
				//11 = 3��
				//12 = 2��
				//13 = �涯
				//
				//14 = �˸� (1, 2)
				//15 = ���� (1, 4)
				//16 = ���� (1, 9)
				//17 = ��� (1, 10)
				//18 = ��� (4, 10)
				//19 = ���� (4, 6)
				//
				//20 = ���� (9��)
				//21 = 8��
				//22 = 7��
				//23 = 6��
				//24 = 5��
				//25 = 6��
				//26 = 5��
				//27 = 4��
				//28 = 3��
				//29 = 2��
				//30 = 1��
				//31 = ����
				//
				//Ư����
				//10001 = ������ (4, 7) (���� �ΰ�)
				//10002 = ������ (3, 7) (3�� + �����) -> �嶯 ������ �� ����
				//
				//10003 = ���� (4, 9) -> �� ������ ����
				//10004 = ���ֱ��� ���� (�Ѵ� ���ֱ���) -> �嶯�� �� ��ȿ, ����
				//
				//99998 = �Ǻ� ��
				//99999 = ����
				//
				//100000 = ���� ?

public:
	//������ �Ҹ���
	Player(void);
	~Player(void);

	//getter setter
	Card*	getHand();
	//void	setHand(); //��� X
	int		getCardNum();
	void	setCardNum(int num);
	int		getMoney();
	void	setMoney(int mny);
	int		getJokbo();
	void	setJokbo(int jkbo);

	//�޼ҵ�
	void addCard(Card cd);
	void clear();

	void addMoney(int mny);
	void subMoney(int mny);

	void evalJokbo();

	void half(int& pandon);
	void call(int& pandon, int callMoney, int myBetMoney);
};