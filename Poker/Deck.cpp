#include "Deck.h"
#include <time.h>
#include <iostream>
using namespace std;


//********************************
//	������ �Ҹ���
//********************************

Deck::Deck(void)
{
	for (int i=0; i<10; i++)
	{
		deck[i] = Card(i+1, true);
	}

	for (int i=10; i<20; i++)
	{
		deck[i] = Card(i-9, false);
	}

	cardNum = 20;
}


Deck::~Deck(void)
{
}






//********************************
//	�޼ҵ�
//********************************

void Deck::shuffle()
{
	srand((unsigned int)time(NULL));
	int count = 0;

	while (count < 100)
	{
		for (int i=0; i<20; i++)
		{
			int temp = rand() % 20;

			Card cd = deck[i];
			deck[i] = deck[temp];
			deck[temp] = cd;
		}

		count++;
	}
}

void Deck::newDeck()
{
	for (int i=0; i<10; i++)
	{
		deck[i] = Card(i+1, true);
	}

	for (int i=10; i<20; i++)
	{
		deck[i] = Card(i-9, false);
	}

	cardNum = 20;
}

void Deck::deal(Player& plyr)
{
	if (cardNum <= 0)
	{
		//����
	}

	plyr.addCard(deck[cardNum-1]);
	cardNum--;
}