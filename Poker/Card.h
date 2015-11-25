#pragma once

class Card
{
private:
	//멍텅구리 or 광 카드들
	//1월 : 1광
	//2월 : 매조
	//3월 : 3광
	//4월 : 두견새
	//5월 : 난초에 다리
	//6월 : 모란에 나비
	//7월 : 홍싸리에 멧돼지
	//8월 : 8광,	억새에 기러기
	//9월 : 국진
	//10월: 단풍에 사슴

	int month; //1~10월
	bool isSpecial; //멍텅구리 or 광 여부
					//8월만 예외적으로 광 + 멍텅구리이므로
					//이게 false면 멍텅

public:
	//생성자, 소멸자
	Card(void);
	~Card(void);
	Card(int mnth, bool sp);

	//getter setter
	int getMonth();
	void setMonth(int mnth);

	bool getIsSpecial();
	void setIsSpecial(bool sp);


	//메소드

};