#pragma once

class Card
{
private:
	//���ֱ��� or �� ī���
	//1�� : 1��
	//2�� : ����
	//3�� : 3��
	//4�� : �ΰ߻�
	//5�� : ���ʿ� �ٸ�
	//6�� : ����� ����
	//7�� : ȫ�θ��� �����
	//8�� : 8��,	����� �ⷯ��
	//9�� : ����
	//10��: ��ǳ�� �罿

	int month; //1~10��
	bool isSpecial; //���ֱ��� or �� ����
					//8���� ���������� �� + ���ֱ����̹Ƿ�
					//�̰� false�� ����

public:
	//������, �Ҹ���
	Card(void);
	~Card(void);
	Card(int mnth, bool sp);

	//getter setter
	int getMonth();
	void setMonth(int mnth);

	bool getIsSpecial();
	void setIsSpecial(bool sp);


	//�޼ҵ�

};