#pragma once
#include "Card.h"
#include "Player.h"

class Deck
{
private:
	Card deck[20];
	int cardNum;

public:
	Deck(void);
	~Deck(void);

	void shuffle();
	void newDeck();
	void deal(Player& plyr);
};