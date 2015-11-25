#pragma comment(lib, "msimg32.lib") //TransparentBlt �Լ� ���� ���̺귯��
#include <windows.h>
#include <tchar.h>
#include <ctime>
#include "resource.h" //���ҽ� �߰�
#include <fstream> //���������
using namespace std;

#include "Card.h"
#include "Deck.h"
//#include "Hand.h"
#include "Player.h"


//game state
#define GAMESTATE_NEW			0  //������ ����
#define	GAMESTATE_WAIT			1  //������
#define GAMESTATE_ONGOING		2  //������
#define GAMESTATE_MYTURN		22 //����
#define GAMESTATE_PRINTCARD		23 //�׳����� ��ǻ�͵��� ī�带 ���
#define GAMESTATE_END			3  //��������


//�ǵ�(��Ƽ, ������)
#define BET		10000


//check part
#define PLAYER_NUMBER	5


static int gameState = GAMESTATE_NEW;
static int pandon = 0; //�ǵ�
static int turn = 0; //
static int startStatus = 0; //WAIT�϶� 0, ��ư������ 1, Update���� �����ؼ� 2�� �ٲ�

static int cardn = 0; //�Ѹ� ī���� ����. 0~2
static int dieman = 0; //���� �� ����� ��
static int callman = 0;
static int maxbet = 0;
static int cnt = 0;

bool regame = false;
static int drawman = 0;


//�÷��̾� ����
Player player1; //me
Player player2;
Player player3;
Player player4;
Player player5;
//�÷��̾� �⺻����. ������


//����. �� ����Ҷ� ����
static int player1_bet = 0;
static int player2_bet = 0;
static int player3_bet = 0;
static int player4_bet = 0;
static int player5_bet = 0;


static int tempCount; //�� ���� ������ �˱� ���� ����
//�� ���ʰ� �Ǿ��� ������ 1 �ø�
//������ ������ ���� ������ ����


bool isAvailableHalf;
bool isAvailableCall; //������ ���� �� �� ���� ���� �ֱ� ������ ��������
Card tempCard; //�Ѹ��� ī�� �ӽ�����
bool isHalfClicked = false;
bool isCallClicked = false;
bool isDieClicked = false;


//������
Deck deck;


//�����
ifstream fin;
ofstream fout;


//render���� ����� �� �ְ� ������ �������� ����
HWND global_hWnd;
HDC hdc;

HDC hMemDC;
PAINTSTRUCT ps;
static HBITMAP hBitmap;
HBITMAP hOldBitmap;
BITMAP Bitmap;
int nWidth, nHeight;



void Update();
void Render();

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPWSTR lpszClass = L"Sutda v1.0";

int APIENTRY WinMain(
	HINSTANCE hInstance, //���� ����ǰ� �ִ� ���α׷��� �ν��Ͻ� �ڵ�
	HINSTANCE hPrevInstance, //�ٷ� �տ� ����� ���α׷��� �ν��Ͻ� �ڵ�, ���� null
	LPSTR lpszCmdParam, //����� ���� (argv)
	int nCmdShow) //�����츦 �����ִ� ������ �÷���
{
	gameState = GAMESTATE_NEW;
	_CRT_SECURE_NO_WARNINGS; //wsccat warning ���ַ��� ��

	srand((unsigned int)time(NULL));

	HWND hWnd;
	MSG Message;
	//MSG ����ü
	//HWND hwnd, UINT message, WPARAM wparam, LPARAM lParam, DWORD time, POINT pt
	//�̺�Ʈ �߻��� �������ڵ�, �̺�Ʈ �޽��� ����, �ΰ����� �ΰ�����, �̺�Ʈ�� ������ �ð�, �̺�Ʈ�� �߻��� ��ġ(��ǥ)
	WNDCLASS WndClass;
	g_hInst=hInstance;

	WndClass.cbClsExtra=0;							//Ŭ���� ���� ����Ʈ ��
	WndClass.cbWndExtra=0;							//������ ���� ����Ʈ ��
	//WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH); //�������� ����
	WndClass.hbrBackground = CreatePatternBrush(LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BG)));
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);	//���α׷����� ����� Ŀ��
	WndClass.hIcon=LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON));	//���α׷����� ����� ������
	WndClass.hInstance=hInstance;					//���α׷��� �ν��Ͻ� �ڵ�
	WndClass.lpfnWndProc=(WNDPROC)WndProc;			//���ν����� �Լ���
	WndClass.lpszClassName=lpszClass;				//����ü�� ������� Ŭ������
	WndClass.lpszMenuName=MAKEINTRESOURCE(IDR_MYMENU);						//���α׷����� ����� �޴�
	WndClass.style=CS_HREDRAW | CS_VREDRAW;			//������ ��Ÿ��
	RegisterClass(&WndClass);

	//CreateWindowEx -> ���״�� Ex
	hWnd=CreateWindowEx(
		WS_EX_TOPMOST,	//dwExStyle. â ��Ÿ�� ����. ���� '�׻� ��' �Ӽ�
		lpszClass,
		lpszClass,
		WS_BORDER | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		806,
		648,
		NULL,
		(HMENU)NULL,
		hInstance,
		NULL);

	//hWnd=CreateWindow(
	//	lpszClass,			//lpClassName : �������� Ŭ������ �����ϴ� ���ڿ�
	//	lpszClass,			//lpWindowName : �������� ���� ǥ���ٿ� ������ ���ڿ�
	//	WS_BORDER | WS_CAPTION | WS_SYSMENU,	//dwStyle : �������� ��Ÿ���� ����
	//	CW_USEDEFAULT,		//x
	//	CW_USEDEFAULT,		//y : �������� ��ǥ�� ����
	//	806,		//nWidth    800->806, 600->648
	//	648,		//nHeight : �������� ���� ���̸� �ȼ��� ����
	//	NULL,				//hWndParent : �θ� �������� �ڵ��� ����		(?)
	//	(HMENU)NULL,		//hMenu : ������ �����쳪 �˾� �������� ��� �޴��� �ڵ��� ����
	//	hInstance,			//hInstance : �����츦 �����ϴ� �ν��Ͻ� �ڵ��� ����
	//	NULL);				//lpParam : WM_CREATE �޽����� �μ� lParam���� ���޵� CREATESTRUCT ����ü�� ������

	ShowWindow(hWnd, nCmdShow);
	//SetWindowPos(hWnd, HWND_TOPMOST, nCmdShow, nCmdShow, nCmdShow, nCmdShow, SWP_NOSIZE);

	//�۷ι��ڵ� ����
	global_hWnd = hWnd;

	HACCEL hMyAccel; //�׼��������� ����ϱ� ���� �߰�. 154p
	hMyAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MYACCEL));

	while(GetMessage(&Message,0,0,0))
	{
		Update();
		Render();

		if (!TranslateAccelerator(hWnd, hMyAccel, &Message)) //�׼��������� ����ϱ� ����
		{
			TranslateMessage(&Message); //���� �Է��� ó���ϴ� �Լ�
			DispatchMessage(&Message); //GetMessage �Լ��κ� ���޵� �޽����� ������ ���ν����� ����
		}
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	//open �κ�
	//	OPENFILENAME ofn;
	TCHAR strFile[MAX_PATH] = L"";
	//TCHAR strTemp[MAX_PATH];

	//HDC hdc, hMemDC;
	/*HDC hMemDC;
	PAINTSTRUCT ps;
	static HBITMAP hBitmap;
	HBITMAP hOldBitmap;
	BITMAP Bitmap;
	int nWidth, nHeight;*/


	switch(iMessage) {
	case WM_CREATE:
		//���� ��ư ����, ID:100
		CreateWindow(	//�ڽ������� ����
			L"button",	//�ڽ������� �̸�, ��ư�̴� Button����
			L"����",	//�ڽ������쿡 ����� �׽�Ʈ
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,	//�ڽ� ������ ��Ÿ��
			453,			//������ǥ x
			425,			//������ǥ y
			71,			//�������� ���� cx
			35,			//�������� ���� cy
			hWnd,		//�θ� ������ �ڵ�
			(HMENU)100,	//���̵� ��ȣ
			g_hInst,	//�ν��Ͻ�
			NULL);		//������ ������ �ʿ��� ������

		//�� ��ư ����, ID:200
		CreateWindow(	//�ڽ������� ����
			L"button",	//�ڽ������� �̸�, ��ư�̴� Button����
			L"��",	//�ڽ������쿡 ����� �׽�Ʈ
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,	//�ڽ� ������ ��Ÿ��
			453,			//������ǥ x
			474,			//������ǥ y
			71,			//�������� ���� cx
			35,			//�������� ���� cy
			hWnd,		//�θ� ������ �ڵ�
			(HMENU)200,	//���̵� ��ȣ
			g_hInst,	//�ν��Ͻ�
			NULL);		//������ ������ �ʿ��� ������

		//���� ��ư ����, ID:300
		CreateWindow(	//�ڽ������� ����
			L"button",	//�ڽ������� �̸�, ��ư�̴� Button����
			L"����",	//�ڽ������쿡 ����� �׽�Ʈ
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,	//�ڽ� ������ ��Ÿ��
			453,			//������ǥ x
			523,			//������ǥ y
			71,			//�������� ���� cx
			35,			//�������� ���� cy
			hWnd,		//�θ� ������ �ڵ�
			(HMENU)300,	//���̵� ��ȣ
			g_hInst,	//�ν��Ͻ�
			NULL);		//������ ������ �ʿ��� ������

		//���� ��ư ����, ID:400
		CreateWindow(	//�ڽ������� ����
			L"button",	//�ڽ������� �̸�, ��ư�̴� Button����
			L"����",	//�ڽ������쿡 ����� �׽�Ʈ
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,	//�ڽ� ������ ��Ÿ��
			365,			//������ǥ x
			213,			//������ǥ y
			71,			//�������� ���� cx
			35,			//�������� ���� cy
			hWnd,		//�θ� ������ �ڵ�
			(HMENU)400,	//���̵� ��ȣ
			g_hInst,	//�ν��Ͻ�
			NULL);		//������ ������ �ʿ��� ������

		return 0;


	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		if (gameState != GAMESTATE_NEW)
		{
			//�� ��� print �ϵ���
		}

		EndPaint(hWnd, &ps); 
		return 0;


	//case WM_TIMER:
	//	BringWindowToTop(hWnd);
	//	return 0;


	case WM_LBUTTONDOWN:
		return 0;


	case WM_RBUTTONDOWN: 
		//ġƮŰ
		player1.addMoney(100);
		return 0;


	case WM_COMMAND:				//�޴� ���ý� �� �޽����� �߻�
		switch(LOWORD(wParam))		//�޴��� ID
		{
			//************************************************************************************
			//  File Menu
			//************************************************************************************

		case ID_MYFILE_NEW:			//����-���� �����
			if (MessageBox(hWnd, L"���� �����Ͻðڽ��ϱ�? �������� ���� �����ʹ� ���ư��ϴ�.", L"���� �����", MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				if (gameState == GAMESTATE_WAIT)
				{
					gameState = GAMESTATE_NEW;
					MessageBox(hWnd, L"���� �����մϴ�.", L"���� �����", MB_OK);
					MessageBox(hWnd, L"�÷��̾���� ���� �缳���Ǿ����ϴ�", L"���� �����", MB_OK);
				} 
				else
				{
					MessageBeep(-1);
					MessageBox(hWnd, L"�����߿��� ���� ������ �� �����ϴ�.", L"���� �����", MB_OK);
				}
			}
			break;

		case ID_MYFILE_OPEN:		//����-����
			if(MessageBox(hWnd, L"��������� �ҷ����ðڽ��ϱ�?", L"����", MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				if (gameState == GAMESTATE_WAIT)
				{
					//������Ʈ
					fin.open("savedata.txt");
					if (!fin)
					{
						MessageBox(hWnd, L"����Ȱ� ���׿�.", L"����", MB_OK);
					}
					else
					{
						int tempInt;

						fin >> tempInt;
						player1.setMoney(tempInt);
						fin >> tempInt;
						player2.setMoney(tempInt);
						fin >> tempInt;
						player3.setMoney(tempInt);
						fin >> tempInt;
						player4.setMoney(tempInt);
						fin >> tempInt;
						player5.setMoney(tempInt);

						fin.close();

						MessageBeep(MB_ICONASTERISK);
						MessageBox(hWnd, L"������¸� �ҷ��Խ��ϴ�.", L"����", MB_OK);
					}
				} 
				else
				{
					MessageBeep(-1);
					MessageBox(hWnd, L"�����߿��� �ҷ��� �� �����ϴ�.", L"����", MB_OK);
				}
			}

			break;

		case ID_MYFILE_SAVE:		//����-����
			if(MessageBox(hWnd, L"�����Ͻðڽ��ϱ�?", L"����", MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				if (gameState == GAMESTATE_WAIT)
				{
					//������Ʈ
					fout.open("savedata.txt");

					fout << player1.getMoney() << endl;
					fout << player2.getMoney() << endl;
					fout << player3.getMoney() << endl;
					fout << player4.getMoney() << endl;
					fout << player5.getMoney();

					fout.close();

					//MessageBeep(MB_ICONASTERISK); //�Ƿ�~ �Ҹ�
					//MessageBeep(MB_ICONEXCLAMATION); //�͹�~ �Ҹ�
					//MessageBeep(MB_ICONHAND); //���������Ҹ�
					//MessageBeep(MB_ICONQUESTION); //�Ҹ� x

					MessageBeep(MB_ICONASTERISK);
					MessageBox(hWnd, L"����Ǿ����ϴ�", L"����", MB_OK);
				} 
				else
				{
					MessageBeep(-1);
					MessageBox(hWnd, L"�����߿��� ������ �� �����ϴ�.", L"����", MB_OK);
				}

			}

			break;

		case ID_MYFILE_CLOSE:		//����-����
			if(MessageBox(hWnd, L"�����Ͻðڽ��ϱ�?", L"����", MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				//free(pfh);
				//KillTimer(hWnd, 1); //����
				PostQuitMessage(0);
				return 0;;
			}
			break;


			//************************************************************************************
			//  Button
			//************************************************************************************

		case 100: //���� ��ư ������ ��
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				if (gameState == GAMESTATE_MYTURN && isAvailableHalf)
				{
					isHalfClicked = true;
					//MessageBox(hWnd, L"�� ���϶� ������ư Ŭ��", L"����", MB_OK);
				} 
				else if (!isAvailableHalf)
				{
					//MessageBox(hWnd, L"���̳� ���� �ϼž� �˴ϴ�", L"����", MB_OK);
				}
				else
				{
					//MessageBox(hWnd, L"�� ���� �ƴҶ� ������ư Ŭ��", L"����", MB_OK);
				}
			}
			break;

		case 200: //�� ��ư ������ ��
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				if (gameState == GAMESTATE_MYTURN && isAvailableCall)
				{
					isCallClicked = true;
					//MessageBox(hWnd, L"�� ���϶� �ݹ�ư Ŭ��", L"��", MB_OK);
				} 
				else if (!isAvailableCall)
				{
					MessageBox(hWnd, L"������ ���̸� �ϼž� �մϴ� (�� �÷��̾�)", L"��", MB_OK);
				}
				else
				{
					//MessageBox(hWnd, L"�� ���� �ƴҶ� ������ư Ŭ��", L"����", MB_OK);
				}
			}
			break;

		case 300: //���� ��ư ������ ��
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				if (gameState == GAMESTATE_MYTURN)
				{
					//isAvailableHalf = false;
					//isAvailableCall = false;
					isDieClicked = true;
					//MessageBox(hWnd, L"�� ���϶� ���̹�ư Ŭ��", L"����", MB_OK);
				} 
				else
				{
					//MessageBox(hWnd, L"�� ���� �ƴҶ� ������ư Ŭ��", L"����", MB_OK);
				}
			}
			break;

		case 400: //���۹�ư ������ ��
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				if (startStatus == 0)
				{
					startStatus = 1;
					//MessageBox(hWnd, L"������ �����ϰڽ��ϴ�.", L"����", MB_ICONQUESTION | MB_OK);
				}
			}
			break;

		} //switch
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}






//update, render���� �� ������ ����
bool isSecondCard;
int CurrPlayer; //1~5
int NextPlayer; //1~5
bool isBack = false; //ī�� �޸����� �ƴ���
bool isPrintCard = false;
bool isPrintText = false;
	bool isHalf = false;
	bool isCall = false;
	bool isDie = false;


//********************************
// Update, Render
//********************************
void Update()
{
	maxbet = max(player1_bet, max(player2_bet, max(player3_bet, max(player4_bet, player5_bet))));

	if (tempCount == 0)
	{
		isAvailableHalf = true;
		isAvailableCall = false;
	}
	else if (tempCount < 5)
	{
		isAvailableHalf = true;
		isAvailableCall = true;
	}
	else
	{
		isAvailableHalf = false;
		isAvailableCall = true;
	}


	switch(gameState)
	{
	case GAMESTATE_NEW:
		{
			//������� ����
			deck.newDeck();
			deck.shuffle();

			//������
			player1.setMoney(10000000);
			player2.setMoney(20000000);
			player3.setMoney(30000000);
			player4.setMoney(40000000);
			player5.setMoney(50000000);


			//�ǵ�
			pandon = 0;

			player1_bet = 0;
			player2_bet = 0;
			player3_bet = 0;
			player4_bet = 0;
			player5_bet = 0;

			gameState = GAMESTATE_WAIT;
		}
		break;

	case GAMESTATE_WAIT:
		{
			if (startStatus == 1)
			{
				startStatus = 2;

				tempCount = 0;
				cardn = 0;
				dieman = 0;
				callman = 0;
				maxbet = 0;

				drawman = 0;

				isBack = false;
				isPrintCard = false;
				isPrintText = false;

				isHalf = false;
				isCall = false;
				isDie = false;

				//���� �ʱ�ȭ
				if (regame)
				{
					//�ǵ� �˾Ƽ� �� ����
					regame = false;
				} 
				else
				{
					player1.subMoney(BET);
					player2.subMoney(BET);
					player3.subMoney(BET);
					player4.subMoney(BET);
					player5.subMoney(BET);
					player1_bet = BET;
					player2_bet = BET;
					player3_bet = BET;
					player4_bet = BET;
					player5_bet = BET;

					//��Ƽ(ante)
					pandon = (BET * 5);
				}

				deck.newDeck();
				deck.shuffle();

				isAvailableHalf = true;
				isAvailableCall = false; //��ó������� �� �Ұ�

				CurrPlayer = 1;

				//InvalidateRect(global_hWnd, NULL, TRUE);

				gameState = GAMESTATE_MYTURN;
			}
		}
		break;




	case GAMESTATE_MYTURN:
		{
			if( (cardn == 3) || dieman == 4 ) //�Ѹ��� �� ���� or (���ó� && ī��2��)
			{ 
				//����ó��
				gameState = GAMESTATE_PRINTCARD;
			}


			if (cardn == 0)
			{
				deck.deal(player1);
				tempCard = (player1.getHand())[0];
				isPrintCard = true;
				CurrPlayer = 1;
				NextPlayer = 2;
				gameState = GAMESTATE_ONGOING;
			}

			CurrPlayer = 1;

			//���徿�ް� ���ý���
			if (cardn == 1)
			{
				if (tempCount == 10 || (5-dieman) == callman)
				{
					isSecondCard = true;
					isBack = false;
					isPrintCard = true;
					callman = 0;

					if ( player1.getJokbo() != 99999 )
					{
						//ī�����徿 �Ѹ��°ɷ� �Ѿ��.
						//cardn = 2; ���߿� �߰�
						deck.deal(player1);
						tempCard = (player1.getHand())[1];
					}

					NextPlayer = 2;
					gameState = GAMESTATE_ONGOING;
				}
				else
				{
					if (tempCount == 0)
					{
						//���� ������ ��
						if (isHalfClicked)
						{
							int temp = pandon/2;
							player1.half(pandon);
							player1_bet += temp;

							NextPlayer = 2;
							callman = 0;

							tempCount++;
							isHalfClicked = false;
							gameState = GAMESTATE_ONGOING;

							isPrintText = true;
							isHalf = true;
						}
						else if(isCallClicked)
						{
							// ���� �ȵ���

							isCallClicked = false;
						}
						else if(isDieClicked)
						{
							player1.setJokbo(99999);
							dieman++;
							tempCount++;
							NextPlayer = 2;

							isDieClicked = false;
							gameState = GAMESTATE_ONGOING;

							isPrintText = true;
							isDie = true;
						}
					}
					else if (tempCount == 5)
					{
						if(isCallClicked)
						{
							player1.call(pandon, maxbet, player1_bet);
							player1_bet = maxbet;

							callman++;
							NextPlayer = 2;

							tempCount++;
							isCallClicked = false;
							gameState = GAMESTATE_ONGOING;

							isPrintText = true;
							isCall = true;
						}
						else if(isDieClicked)
						{
							player1.setJokbo(99999);
							dieman++;
							tempCount++;

							NextPlayer = 2;

							isDieClicked = false;
							gameState = GAMESTATE_ONGOING;

							isPrintText = true;
							isDie = true;
						}
					}


					//die ������ �� �� ���� ����
					if (player1.getJokbo() == 99999 && gameState != GAMESTATE_ONGOING && gameState != GAMESTATE_PRINTCARD)
					{
						tempCount++;
						NextPlayer = 2;
						gameState = GAMESTATE_ONGOING;
					}
				}
			} //if (cardn == 1)
			else if (cardn == 2)
			{
				if (tempCount == 10 || (5-dieman) == callman)
				{
					//���� ������ ��
					gameState = GAMESTATE_PRINTCARD;
				}

				if (tempCount == 0)
				{
					//���� ������ ��
					if (isHalfClicked)
					{
						int temp = pandon/2;
						player1.half(pandon);
						player1_bet += temp;

						NextPlayer = 2;
						callman = 0;

						tempCount++;
						isHalfClicked = false;
						gameState = GAMESTATE_ONGOING;

						isHalf = true;
						isPrintText = true;
					}
					else if(isCallClicked)
					{
						// ���� �ȵ���

						isCallClicked = false;
					}
					else if(isDieClicked)
					{
						player1.setJokbo(99999);
						dieman++;
						tempCount++;
						NextPlayer = 2;

						isDieClicked = false;
						gameState = GAMESTATE_ONGOING;

						isDie = true;
						isPrintText = true;
					}
				}
				else if (tempCount == 5)
				{
					if(isCallClicked)
					{
						player1.call(pandon, maxbet, player1_bet);
						player1_bet = maxbet;

						callman++;
						NextPlayer = 2;

						tempCount++;
						isCallClicked = false;
						gameState = GAMESTATE_ONGOING;

						isCall = true;
						isPrintText = true;
					}
					else if(isDieClicked)
					{
						player1.setJokbo(99999);
						dieman++;
						tempCount++;
						NextPlayer = 2;

						isDieClicked = false;
						gameState = GAMESTATE_ONGOING;

						isDie = true;
						isPrintText = true;
					}
				}


				//die ������ �� �� ���� ����
				if (player1.getJokbo() == 99999 && gameState != GAMESTATE_ONGOING && gameState != GAMESTATE_PRINTCARD)
				{
					tempCount++;
					NextPlayer = 2;
					gameState = GAMESTATE_ONGOING;
				}
			} //else if (cardn == 2)
		} //case GAMESTATE_MYTURN
		break;



	case GAMESTATE_ONGOING:
		{
			if (cardn == 0)
			{
				switch (NextPlayer)
				{
				case 2:
					deck.deal(player2);
					tempCard = (player2.getHand())[0];
					isPrintCard = true;
					CurrPlayer = 2;
					NextPlayer = 3;
					isBack = true;
					Sleep(500);
					break;

				case 3:
					deck.deal(player3);
					tempCard = (player3.getHand())[0];
					isPrintCard = true;
					CurrPlayer = 3;
					NextPlayer = 4;
					Sleep(500);
					break;

				case 4:
					deck.deal(player4);
					tempCard = (player4.getHand())[0];
					isPrintCard = true;
					CurrPlayer = 4;
					NextPlayer = 5;
					Sleep(500);
					break;

				case 5:
					deck.deal(player5);
					tempCard = (player5.getHand())[0];
					isPrintCard = true;
					CurrPlayer = 5;
					NextPlayer = 1;
					cardn = 1;
					gameState = GAMESTATE_MYTURN;
					Sleep(500);
					break;
				} //switch
			}
			else if (cardn == 1)
			{
				if (!isSecondCard)
				{
					switch (tempCount)
					{ //90% Ȯ���� ����, 10%Ȯ���� ��
					case 1: //2p
						{
							int temp = rand() % 100 + 1;
							if (temp <= 10)
							{
								//call
								callman++;

								player2.call(pandon, maxbet, player2_bet);
								player2_bet = maxbet;

								isCall = true;
								isPrintText = true;
							}
							else
							{
								//half
								int temp = pandon/2;
								player2.half(pandon);
								player2_bet += temp;

								callman = 0;

								isHalf = true;
								isPrintText = true;
							}
							CurrPlayer = 2;
							NextPlayer = 3;
							tempCount++;
							//Sleep(500);
						}
						break;

					case 2: //3p
						{
							int temp = rand() % 100 + 1;
							if (temp <= 10)
							{
								//call
								callman++;

								player3.call(pandon, maxbet, player3_bet);
								player3_bet = maxbet;

								isCall = true;
								isPrintText = true;
							}
							else
							{
								//half
								int temp = pandon/2;
								player3.half(pandon);
								player3_bet += temp;

								callman = 0;

								isHalf = true;
								isPrintText = true;
							}
							CurrPlayer = 3;
							NextPlayer = 4;
							tempCount++;
							//Sleep(500);
						}
						break;

					case 3: //4p
						{
							int temp = rand() % 100 + 1;
							if (temp <= 10)
							{
								//call
								callman++;

								player4.call(pandon, maxbet, player4_bet);
								player4_bet = maxbet;

								isCall = true;
								isPrintText = true;
							}
							else
							{
								//half
								int temp = pandon/2;
								player4.half(pandon);
								player4_bet += temp;

								callman = 0;

								isHalf = true;
								isPrintText = true;
							}
							CurrPlayer = 4;
							NextPlayer = 5;
							tempCount++;
							//Sleep(500);
						}
						break;

					case 4: //5p
						{
							int temp = rand() % 100 + 1;
							if (temp <= 10)
							{
								//call
								callman++;

								player5.call(pandon, maxbet, player5_bet);
								player5_bet = maxbet;

								isCall = true;
								isPrintText = true;
							}
							else
							{
								//half
								int temp = pandon/2;
								player5.half(pandon);
								player5_bet += temp;

								callman = 0;

								isHalf = true;
								isPrintText = true;
							}
							CurrPlayer = 5;
							NextPlayer = 1;
							tempCount++;
							//Sleep(500);
							gameState = GAMESTATE_MYTURN;
						}
						break;



					case 6: //2p
						//call�� ����
						callman++;

						player2.call(pandon, maxbet, player2_bet);
						player2_bet = maxbet;
						//Sleep(500);
						tempCount++;

						CurrPlayer = 2;
						isCall = true;
						isPrintText = true;
						break;

					case 7: //3p
						//call�� ����
						callman++;

						player3.call(pandon, maxbet, player3_bet);
						player3_bet = maxbet;
						//Sleep(500);
						tempCount++;

						CurrPlayer = 3;
						isCall = true;
						isPrintText = true;
						break;

					case 8: //4p
						//call�� ����
						callman++;

						player4.call(pandon, maxbet, player4_bet);
						player4_bet = maxbet;
						//Sleep(500);
						tempCount++;

						CurrPlayer = 4;
						isCall = true;
						isPrintText = true;
						break;

					case 9: //5p
						//call�� ����
						callman++;

						player5.call(pandon, maxbet, player5_bet);
						player5_bet = maxbet;
						//Sleep(500);
						tempCount++;

						CurrPlayer = 5;
						isCall = true;
						isPrintText = true;
						break;
					}

					if (tempCount == 10 || (5-dieman) == callman)
					{
						gameState = GAMESTATE_MYTURN;
					}
				}
				else
				{
					switch (NextPlayer)
					{
					case 2:
						deck.deal(player2);
						tempCard = (player2.getHand())[1];
						isPrintCard = true;
						CurrPlayer = 2;
						NextPlayer = 3;
						isBack = false;
						Sleep(500);
						break;

					case 3:
						deck.deal(player3);
						tempCard = (player3.getHand())[1];
						isPrintCard = true;
						CurrPlayer = 3;
						NextPlayer = 4;
						Sleep(500);
						break;

					case 4:
						deck.deal(player4);
						tempCard = (player4.getHand())[1];
						isPrintCard = true;
						CurrPlayer = 4;
						NextPlayer = 5;
						Sleep(500);
						break;

					case 5:
						deck.deal(player5);
						tempCard = (player5.getHand())[1];
						isPrintCard = true;
						CurrPlayer = 5;
						NextPlayer = 1;
						cardn = 2;
						gameState = GAMESTATE_MYTURN;
						tempCount = 0;
						Sleep(500);
						break;
					} //switch
				}
			}
			else if (cardn == 2)
			{
				//ī�� ���徿 �ް� ����
				switch (tempCount)
				{ //90% Ȯ���� ����, 10%Ȯ���� ��
				case 1: //2p
					{
						int temp = rand() % 100 + 1;
						if (temp <= 10)
						{
							//call
							callman++;

							player2.call(pandon, maxbet, player2_bet);
							player2_bet = maxbet;

							isCall = true;
							isPrintText = true;
						}
						else
						{
							//half
							int temp = pandon/2;
							player2.half(pandon);
							player2_bet += temp;

							callman = 0;

							isHalf = true;
							isPrintText = true;
						}
						CurrPlayer = 2;
						NextPlayer = 3;
						tempCount++;
						//Sleep(500);
					}
					break;

				case 2: //3p
					{
						int temp = rand() % 100 + 1;
						if (temp <= 10)
						{
							//call
							callman++;

							player3.call(pandon, maxbet, player3_bet);
							player3_bet = maxbet;

							isCall = true;
							isPrintText = true;
						}
						else
						{
							//half
							int temp = pandon/2;
							player3.half(pandon);
							player3_bet += temp;

							callman = 0;

							isHalf = true;
							isPrintText = true;
						}
						CurrPlayer = 3;
						NextPlayer = 4;
						tempCount++;
						//Sleep(500);
					}
					break;

				case 3: //4p
					{
						int temp = rand() % 100 + 1;
						if (temp <= 10)
						{
							//call
							callman++;

							player4.call(pandon, maxbet, player4_bet);
							player4_bet = maxbet;

							isCall = true;
							isPrintText = true;
						}
						else
						{
							//half
							int temp = pandon/2;
							player4.half(pandon);
							player4_bet += temp;

							callman = 0;

							isHalf = true;
							isPrintText = true;
						}
						CurrPlayer = 4;
						NextPlayer = 5;
						tempCount++;
						//Sleep(500);
					}
					break;

				case 4: //5p
					{
						int temp = rand() % 100 + 1;
						if (temp <= 10)
						{
							//call
							callman++;

							player5.call(pandon, maxbet, player5_bet);
							player5_bet = maxbet;

							isCall = true;
							isPrintText = true;
						}
						else
						{
							//half
							int temp = pandon/2;
							player5.half(pandon);
							player5_bet += temp;

							callman = 0;

							isHalf = true;
							isPrintText = true;
						}
						CurrPlayer = 5;
						NextPlayer = 1;
						tempCount++;
						//Sleep(500);
						gameState = GAMESTATE_MYTURN;
					}
					break;



				case 6: //2p
					//call�� ����
					callman++;

					player2.call(pandon, maxbet, player2_bet);
					player2_bet = maxbet;
					//Sleep(500);
					tempCount++;

					CurrPlayer = 2;
					isCall = true;
					isPrintText = true;

					break;

				case 7: //3p
					//call�� ����
					callman++;

					player3.call(pandon, maxbet, player3_bet);
					player3_bet = maxbet;
					//Sleep(500);
					tempCount++;

					CurrPlayer = 3;
					isCall = true;
					isPrintText = true;

					break;

				case 8: //4p
					//call�� ����
					callman++;

					player4.call(pandon, maxbet, player4_bet);
					player4_bet = maxbet;
					//Sleep(500);
					tempCount++;

					CurrPlayer = 4;
					isCall = true;
					isPrintText = true;

					break;

				case 9: //5p
					//call�� ����
					callman++;

					player5.call(pandon, maxbet, player5_bet);
					player5_bet = maxbet;
					//Sleep(500);
					tempCount++;

					CurrPlayer = 5;
					isCall = true;
					isPrintText = true;

					break;
				}

				if (tempCount == 10 || (5-dieman) == callman)
				{
					gameState = GAMESTATE_MYTURN;
				}
			}
		} //switch
		break;



	//ù��°ī�� ����
	case GAMESTATE_PRINTCARD:
		{
			//�̰� ������ �����ؾߵ�
			//int cnt;

			isSecondCard = false;

			switch (cnt)
			{
			case 0:
				cnt++;
				tempCard = (player2.getHand())[0];
				//Sleep(500);

				CurrPlayer = 2;
				isPrintCard = true;
				break;

			case 1:
				cnt++;
				tempCard = (player3.getHand())[0];
				//Sleep(500);
				
				CurrPlayer = 3;
				isPrintCard = true;
				break;

			case 2:
				cnt++;
				tempCard = (player4.getHand())[0];
				//Sleep(500);

				CurrPlayer = 4;
				isPrintCard = true;
				break;

			case 3:
				cnt++;
				tempCard = (player5.getHand())[0];
				//Sleep(500);

				CurrPlayer = 5;
				isPrintCard = true;
				break;

			case 4:
				Sleep(500);
				gameState = GAMESTATE_END;
				cnt = 0;
				break;
			}

		}
		break;



	case GAMESTATE_END:
		{	
			int winPlayer = 1; //�̱���. 1=��, 2~~ = ������

			//����ó��
			//�̰� ��� ���� ? ����


			player1.evalJokbo();
			player2.evalJokbo();
			player3.evalJokbo();
			player4.evalJokbo();
			player5.evalJokbo();


			//���� �������� ���������� !!!
			//1 = 38����
			int maxJokbo;
			bool maxPlayer[5] = {0};
			bool isDraw = false;
			int tempJokbo[5] = {player1.getJokbo(), player2.getJokbo(), player3.getJokbo(), player4.getJokbo(), player5.getJokbo()};


			//Draw ����


			
			//win player ����
			if (isDraw) //���� ������ ������ or ����
			{
			}
			else
			{
				winPlayer = 1;
				maxJokbo = tempJokbo[0];

				for (int i=1; i<5; i++)
				{
					//Ư�����϶��� continue�ϵ���
					if (tempJokbo[i] > 31)
					{
						continue;
					}


					if (maxJokbo > tempJokbo[i])
					{
						winPlayer = (i+1);
						maxJokbo = tempJokbo[i];
					}
				}
			}


			////Ư���� ó��
			//������
			if (maxJokbo == 2 || maxJokbo == 3)
			{
				for (int i=0; i<5; i++)
				{
					if (tempJokbo[i] == 10001)
					{
						winPlayer = (i+1);
						break;
					}
				}
			}

			//������
			if (maxJokbo <= 13 && maxJokbo >= 5)
			{
				for (int i=0; i<5; i++)
				{
					if (tempJokbo[i] == 10002)
					{
						winPlayer = (i+2);
						break;
					}
				}
			}





			//���� ��� �Ҽ��ֵ��� ����
			//��ư���� ��,...

			
			//regame


			//������ ����

			//������ ����



			//94 ������ ��
			/*if(regame)
			{
				gameState = GAMESTATE_WAIT;
				regame = false;
			}*/
			//�� �Ʒ��κ��� else�� ����?
			//����� �ٵ� �ǵ� 1/2�� ������
			//GAMESTATE_REGAME �߰�?



			wchar_t win_buffer[256];
			//���� ������ �¸��ߴ����� ������ ������ ������ �ٵ� �����¡

			switch (winPlayer)
			{
			case 1:
				player1.addMoney(pandon);
				wsprintfW(win_buffer, L"1 �÷��̾� ��. ( + %d �� )", pandon);
				break;

			case 2:
				player2.addMoney(pandon);
				wsprintfW(win_buffer, L"2 �÷��̾� ��. ( + %d ��)", pandon);
				break;

			case 3:
				player3.addMoney(pandon);
				wsprintfW(win_buffer, L"3 �÷��̾� ��. ( + %d ��)", pandon);
				break;

			case 4:
				player4.addMoney(pandon);
				wsprintfW(win_buffer, L"4 �÷��̾� ��. ( + %d ��)", pandon);
				break;

			case 5:
				player5.addMoney(pandon);
				wsprintfW(win_buffer, L"5 �÷��̾� ��. ( + %d ��)", pandon);
				break;
			}


			startStatus = 0;

			pandon = 0;

			player1_bet = 0;
			player2_bet = 0;
			player3_bet = 0;
			player4_bet = 0;
			player5_bet = 0;

			player1.setJokbo(99998);
			player2.setJokbo(99998);
			player3.setJokbo(99998);
			player4.setJokbo(99998);
			player5.setJokbo(99998);

			player1.clear();
			player2.clear();
			player3.clear();
			player4.clear();
			player5.clear();


			cardn = 0;
			dieman = 0;
			maxbet = 0;
			callman = 0;
			tempCount = 0;
			isSecondCard = false;


			if (MessageBox(global_hWnd, win_buffer, L"���", MB_OK | MB_ICONINFORMATION) == IDOK)
			{
				InvalidateRect(global_hWnd, NULL, TRUE);
			}


			//1p�� ���� ����
			if (player1.getMoney() <= 0)
			{
				if(MessageBox(global_hWnd, L"���δ��߽��ϴ�. ���� �����Ͻðڽ��ϱ�?", L"�й�", MB_ICONQUESTION | MB_YESNO) == IDYES)
				{
					gameState = GAMESTATE_NEW;
				}
				else
				{
					//����
					PostQuitMessage(0);
				}
			}
			else
			{
				gameState = GAMESTATE_WAIT;
			}

		}
		break;
	}


	//���⿡ ��, ī�� �� render �ϴ��� WM_PAINT�� ����ϴ°� �־�� ��������


	return;
} //update


HFONT hFont, hOldFont;
HBRUSH MyBrush, OldBrush;
HPEN MyPen, OldPen;

void Render()
{
	//�� ���

	//��Ʈ����
	hFont = CreateFont(
		14,				//����
		0,				//��. 0�̸� �ڵ�
		0, 0,			//����
		FW_NORMAL,		//�β�
		0, 0, 0, //�����, ����, ���뼱
		HANGEUL_CHARSET,		//charset
		OUT_DEFAULT_PRECIS,		//��� ��Ȯ��
		CLIP_DEFAULT_PRECIS,	//Ŭ���� ��Ȯ��
		DEFAULT_QUALITY,		//��Ʈ�� ��
		VARIABLE_PITCH | FF_ROMAN,	//��Ʈ�� ��ġ�� �׷�
		L"����"					//��Ʈ�� �̸�
		);

	//
	// int to LPWSTR(LPCWSTR) ���� �Ʒ�ó�� �ϸ� ��
	//
	//int a = 16;
	//wchar_t str_buffer[256];
	//wsprintfW(str_buffer, L"%d", a);
	//

	//���� string���� �����ϰ��ִ� ���� ����
	wchar_t str_buffer1[256];
	wchar_t str_buffer2[256];
	wchar_t str_buffer3[256];
	wchar_t str_buffer4[256];
	wchar_t str_buffer5[256];
	wchar_t str_buffer_pandon[256];


	wsprintfW(str_buffer1, L"%d", player1.getMoney());
	wsprintfW(str_buffer2, L"%d", player2.getMoney());
	wsprintfW(str_buffer3, L"%d", player3.getMoney());
	wsprintfW(str_buffer4, L"%d", player4.getMoney());
	wsprintfW(str_buffer5, L"%d", player5.getMoney());
	wsprintfW(str_buffer_pandon, L"%d", pandon);

	RECT rect1 = {277, 542, 277+108, 542+14};
	RECT rect2 = {563, 347, 563+108, 347+14};
	RECT rect3 = {563, 174, 563+108, 174+14};
	RECT rect4 = {105, 173, 105+108, 173+14};
	RECT rect5 = {105, 346, 105+108, 346+14};
	RECT rect_pandon = {347, 70, 347+135, 70+14};

	MyBrush = CreateHatchBrush(HS_BDIAGONAL, RGB(255, 255, 255));
	OldBrush = (HBRUSH)SelectObject(hdc, MyBrush);
	MyPen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
	OldPen = (HPEN)SelectObject(hdc, MyPen);


	hOldFont = (HFONT)SelectObject(hdc, hFont);

	//TextOut(hdc, 10, 10, str_buffer1, wcslen(str_buffer1));
	Rectangle(hdc, 277, 542, 277+108, 542+14);
	DrawText(hdc, str_buffer1, wcslen(str_buffer1), &rect1, DT_NOCLIP | DT_RIGHT);
	Rectangle(hdc, 563, 347, 563+108, 347+14);
	DrawText(hdc, str_buffer2, wcslen(str_buffer2), &rect2, DT_NOCLIP | DT_RIGHT);
	Rectangle(hdc, 563, 174, 563+108, 174+14);
	DrawText(hdc, str_buffer3, wcslen(str_buffer3), &rect3, DT_NOCLIP | DT_RIGHT);
	Rectangle(hdc, 105, 173, 105+108, 173+14);
	DrawText(hdc, str_buffer4, wcslen(str_buffer4), &rect4, DT_NOCLIP | DT_RIGHT);
	Rectangle(hdc, 105, 346, 105+108, 346+14);
	DrawText(hdc, str_buffer5, wcslen(str_buffer5), &rect5, DT_NOCLIP | DT_RIGHT);
	Rectangle(hdc, 347, 70, 347+135, 70+14);
	DrawText(hdc, str_buffer_pandon, wcslen(str_buffer_pandon), &rect_pandon, DT_NOCLIP | DT_RIGHT);

	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);

	SelectObject(hdc, OldBrush);
	SelectObject(hdc, OldPen);
	DeleteObject(MyBrush);
	DeleteObject(MyPen);



	if ( isPrintText )
	{
		hdc = GetDC(global_hWnd);
		hMemDC = CreateCompatibleDC(hdc);
		int tempIDB;

		if (isHalf)
			tempIDB = IDB_HALF;
		else if (isCall)
			tempIDB = IDB_CALL;
		else if (isDie)
			tempIDB = IDB_DIE;

		

		hBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(tempIDB));
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
		GetObject(hBitmap, sizeof(BITMAP), &Bitmap);


		nWidth = Bitmap.bmWidth;
		nHeight = Bitmap.bmHeight;


		int nX, nY;

		switch (CurrPlayer)
		{
		case 1:
			nX = 375;
			nY = 368;
			break;

		case 2:
			nX = 460;
			nY = 293;
			break;

		case 3:
			nX = 460;
			nY = 169;
			break;

		case 4:
			nX = 284;
			nY = 169;
			break;

		case 5:
			nX = 284;
			nY = 293;
			break;
		}

		TransparentBlt(hdc, nX, nY, nWidth, nHeight, hMemDC, 0, 0, nWidth, nHeight, RGB(255, 0, 255));
		//TransparentBlt(hdc, nX, nY, nWidth, nHeight, hMemDC, nX, nY, nWidth, nHeight, RGB(255, 0, 255));
		//TransparentBlt(hdc, nX, nY, nWidth, nHeight, hMemDC, 0, 0, nWidth, nHeight, RGB(255, 0, 255));
		//BitBlt(hdc, nX, nY, nWidth, nHeight, hMemDC, 0, 0, SRCCOPY);

		//1000~1999
		//int tempTime = (rand() % 1000) + 1000;
		int tempTime = 500;
		

		SelectObject(hMemDC, hOldBitmap);

		Sleep(tempTime);

		DeleteDC(hMemDC);
		DeleteObject(hBitmap);

		RECT rt = {nX, nY, nX+52, nY+29};


		InvalidateRect(global_hWnd, &rt, TRUE);


		isHalf = false;
		isCall = false;
		isDie  = false;
		isPrintText = false;
	}



	if ( isPrintCard )
	{
		hdc = GetDC(global_hWnd);
		hMemDC = CreateCompatibleDC(hdc);


		//ī�弳��
		//Card cd(5, false);
		int tempCd;
		int tempSpec; //0 -> �븻, 1 -> ���ֱ���
		int tempMonth = tempCard.getMonth();

		if (tempCard.getIsSpecial())
			tempSpec = 1;
		else
			tempSpec = 0;

		if (isBack)
			tempSpec = 2;


		switch (tempSpec)
		{
		case 0:
			switch (tempMonth)
			{
			case 1:
				tempCd = IDB_CARD_10;
				break;

			case 2:
				tempCd = IDB_CARD_20;
				break;

			case 3:
				tempCd = IDB_CARD_30;
				break;

			case 4:
				tempCd = IDB_CARD_40;
				break;

			case 5:
				tempCd = IDB_CARD_50;
				break;

			case 6:
				tempCd = IDB_CARD_60;
				break;

			case 7:
				tempCd = IDB_CARD_70;
				break;

			case 8:
				tempCd = IDB_CARD_80;
				break;

			case 9:
				tempCd = IDB_CARD_90;
				break;

			case 10:
				tempCd = IDB_CARD_100;
				break;
			}
			break;

		case 1:
			switch (tempMonth)
			{
			case 1:
				tempCd = IDB_CARD_11;
				break;

			case 2:
				tempCd = IDB_CARD_21;
				break;

			case 3:
				tempCd = IDB_CARD_31;
				break;

			case 4:
				tempCd = IDB_CARD_41;
				break;

			case 5:
				tempCd = IDB_CARD_51;
				break;

			case 6:
				tempCd = IDB_CARD_61;
				break;

			case 7:
				tempCd = IDB_CARD_71;
				break;

			case 8:
				tempCd = IDB_CARD_81;
				break;

			case 9:
				tempCd = IDB_CARD_91;
				break;

			case 10:
				tempCd = IDB_CARD_101;
				break;
			}
			break;

		case 2: //�޸�
			tempCd = IDB_CARD_00;
			break;
		}




		//tempCd ����


		hBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(tempCd));
		//hBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CARD_101));

		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

		GetObject(hBitmap, sizeof(BITMAP), &Bitmap);

		nWidth = Bitmap.bmWidth;
		nHeight = Bitmap.bmHeight;

		//�̰� bitblt�� ����
		//��ǥ��.. ����
		int nX = 365;
		int nY = 279;

		//2��° ī���� ���
		//��ǥ�� x���� �÷���

		//bool isSecondCard; �߰�
		switch ( CurrPlayer )
		{
		case 1:
			nX = 286;
			nY = 426;
			if (isSecondCard)
				nX += 60;
			break;

		case 2:
			nX = 566;
			nY = 243;
			if (isSecondCard)
				nX += 60;
			break;

		case 3:
			nX = 566;
			nY = 70;
			if (isSecondCard)
				nX += 60;
			break;

		case 4:
			nX = 108;
			nY = 70;
			if (isSecondCard)
				nX += 60;
			break;

		case 5:
			nX = 108;
			nY = 243;
			if (isSecondCard)
				nX += 60;
			break;
		}


		//�̰� Ư�� �� �����ϰ� ����ϴ°�
		//TransparentBlt(hdc, 365, 279, nWidth, nHeight, hMemDC, 0, 0, nWidth, nHeight, RGB(0, 128, 128)); //Ư�� �� �����ϰ� �����ؼ� ���

		if (CurrPlayer != 1 || player1.getJokbo() != 99999)
		{
			BitBlt(hdc, nX, nY, nWidth, nHeight, hMemDC, 0, 0, SRCCOPY);
		}

		//BitBlt(hdc, nX, nY, nWidth, nHeight, hMemDC, 0, 0, SRCCOPY);
		//�а� ������ �� ȿ���� 682p

		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
		DeleteObject(hBitmap);

		isPrintCard = false;
	}

	return;
} //render