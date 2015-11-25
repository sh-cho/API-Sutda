#pragma comment(lib, "msimg32.lib") //TransparentBlt 함수 지원 라이브러리
#include <windows.h>
#include <tchar.h>
#include <ctime>
#include "resource.h" //리소스 추가
#include <fstream> //파일입출력
using namespace std;

#include "Card.h"
#include "Deck.h"
//#include "Hand.h"
#include "Player.h"


//game state
#define GAMESTATE_NEW			0  //새게임 시작
#define	GAMESTATE_WAIT			1  //대기상태
#define GAMESTATE_ONGOING		2  //진행중
#define GAMESTATE_MYTURN		22 //내턴
#define GAMESTATE_PRINTCARD		23 //겜끝나고 컴퓨터들의 카드를 출력
#define GAMESTATE_END			3  //끝난상태


//판돈(엔티, 참가비)
#define BET		10000


//check part
#define PLAYER_NUMBER	5


static int gameState = GAMESTATE_NEW;
static int pandon = 0; //판돈
static int turn = 0; //
static int startStatus = 0; //WAIT일때 0, 버튼눌리면 1, Update에서 감지해서 2로 바꿈

static int cardn = 0; //뿌린 카드의 갯수. 0~2
static int dieman = 0; //다이 한 사람의 수
static int callman = 0;
static int maxbet = 0;
static int cnt = 0;

bool regame = false;
static int drawman = 0;


//플레이어 생성
Player player1; //me
Player player2;
Player player3;
Player player4;
Player player5;
//플레이어 기본셋팅. 돈설정


//낸돈. 콜 계산할때 쓰임
static int player1_bet = 0;
static int player2_bet = 0;
static int player3_bet = 0;
static int player4_bet = 0;
static int player5_bet = 0;


static int tempCount; //패 돌릴 시점을 알기 위한 변수
//내 차례가 되었을 때마다 1 올림
//게임은 무조건 내가 선부터 시작


bool isAvailableHalf;
bool isAvailableCall; //하프와 콜을 할 수 없는 때가 있기 때문에 변수만듬
Card tempCard; //뿌리는 카드 임시저장
bool isHalfClicked = false;
bool isCallClicked = false;
bool isDieClicked = false;


//덱생성
Deck deck;


//입출력
ifstream fin;
ofstream fout;


//render에서 써먹을 수 있게 변수를 전역으로 설정
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
	HINSTANCE hInstance, //현재 실행되고 있는 프로그램의 인스턴스 핸들
	HINSTANCE hPrevInstance, //바로 앞에 실행된 프로그램의 인스턴스 핸들, 보통 null
	LPSTR lpszCmdParam, //명령행 인자 (argv)
	int nCmdShow) //윈도우를 보여주는 형태의 플래그
{
	gameState = GAMESTATE_NEW;
	_CRT_SECURE_NO_WARNINGS; //wsccat warning 없애려고 씀

	srand((unsigned int)time(NULL));

	HWND hWnd;
	MSG Message;
	//MSG 구조체
	//HWND hwnd, UINT message, WPARAM wparam, LPARAM lParam, DWORD time, POINT pt
	//이벤트 발생한 윈도우핸들, 이벤트 메시지 종류, 부가정보 부가정보, 이벤트가 접수된 시간, 이벤트가 발생한 위치(좌표)
	WNDCLASS WndClass;
	g_hInst=hInstance;

	WndClass.cbClsExtra=0;							//클래스 여분 바이트 수
	WndClass.cbWndExtra=0;							//윈도우 여분 바이트 수
	//WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH); //윈도우의 배경색
	WndClass.hbrBackground = CreatePatternBrush(LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BG)));
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);	//프로그램에서 사용할 커서
	WndClass.hIcon=LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON));	//프로그램에서 사용할 아이콘
	WndClass.hInstance=hInstance;					//프로그램의 인스턴스 핸들
	WndClass.lpfnWndProc=(WNDPROC)WndProc;			//프로시저의 함수명
	WndClass.lpszClassName=lpszClass;				//구조체로 만들어질 클래스명
	WndClass.lpszMenuName=MAKEINTRESOURCE(IDR_MYMENU);						//프로그램에서 사용할 메뉴
	WndClass.style=CS_HREDRAW | CS_VREDRAW;			//윈도우 스타일
	RegisterClass(&WndClass);

	//CreateWindowEx -> 말그대로 Ex
	hWnd=CreateWindowEx(
		WS_EX_TOPMOST,	//dwExStyle. 창 스타일 설정. 저건 '항상 위' 속성
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
	//	lpszClass,			//lpClassName : 윈도우의 클래스를 지정하는 문자열
	//	lpszClass,			//lpWindowName : 윈도우의 제목 표시줄에 보여줄 문자열
	//	WS_BORDER | WS_CAPTION | WS_SYSMENU,	//dwStyle : 윈도우의 스타일을 지정
	//	CW_USEDEFAULT,		//x
	//	CW_USEDEFAULT,		//y : 윈도우의 좌표를 지정
	//	806,		//nWidth    800->806, 600->648
	//	648,		//nHeight : 윈도우의 폭과 높이를 픽셀로 지정
	//	NULL,				//hWndParent : 부모 윈도우의 핸들을 지정		(?)
	//	(HMENU)NULL,		//hMenu : 겹쳐진 윈도우나 팝업 윈도우의 경우 메뉴의 핸들을 지정
	//	hInstance,			//hInstance : 윈도우를 생성하는 인스턴스 핸들을 지정
	//	NULL);				//lpParam : WM_CREATE 메시지의 인수 lParam으로 전달될 CREATESTRUCT 구조체의 포인터

	ShowWindow(hWnd, nCmdShow);
	//SetWindowPos(hWnd, HWND_TOPMOST, nCmdShow, nCmdShow, nCmdShow, nCmdShow, SWP_NOSIZE);

	//글로벌핸들 지정
	global_hWnd = hWnd;

	HACCEL hMyAccel; //액셀러레이터 사용하기 위해 추가. 154p
	hMyAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MYACCEL));

	while(GetMessage(&Message,0,0,0))
	{
		Update();
		Render();

		if (!TranslateAccelerator(hWnd, hMyAccel, &Message)) //액셀러레이터 사용하기 위해
		{
			TranslateMessage(&Message); //문자 입력을 처리하는 함수
			DispatchMessage(&Message); //GetMessage 함수로붜 전달된 메시지를 윈도우 프로시저로 보냄
		}
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	//open 부분
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
		//하프 버튼 만듬, ID:100
		CreateWindow(	//자식윈도우 생성
			L"button",	//자식윈도우 이름, 버튼이니 Button으로
			L"하프",	//자식윈도우에 출력할 테스트
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,	//자식 윈도우 스타일
			453,			//시작좌표 x
			425,			//시작좌표 y
			71,			//윈도우의 넓이 cx
			35,			//윈도우의 높이 cy
			hWnd,		//부모 윈도우 핸들
			(HMENU)100,	//아이디 번호
			g_hInst,	//인스턴스
			NULL);		//윈도우 생성시 필요한 데이터

		//콜 버튼 만듬, ID:200
		CreateWindow(	//자식윈도우 생성
			L"button",	//자식윈도우 이름, 버튼이니 Button으로
			L"콜",	//자식윈도우에 출력할 테스트
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,	//자식 윈도우 스타일
			453,			//시작좌표 x
			474,			//시작좌표 y
			71,			//윈도우의 넓이 cx
			35,			//윈도우의 높이 cy
			hWnd,		//부모 윈도우 핸들
			(HMENU)200,	//아이디 번호
			g_hInst,	//인스턴스
			NULL);		//윈도우 생성시 필요한 데이터

		//다이 버튼 만듬, ID:300
		CreateWindow(	//자식윈도우 생성
			L"button",	//자식윈도우 이름, 버튼이니 Button으로
			L"다이",	//자식윈도우에 출력할 테스트
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,	//자식 윈도우 스타일
			453,			//시작좌표 x
			523,			//시작좌표 y
			71,			//윈도우의 넓이 cx
			35,			//윈도우의 높이 cy
			hWnd,		//부모 윈도우 핸들
			(HMENU)300,	//아이디 번호
			g_hInst,	//인스턴스
			NULL);		//윈도우 생성시 필요한 데이터

		//시작 버튼 만듬, ID:400
		CreateWindow(	//자식윈도우 생성
			L"button",	//자식윈도우 이름, 버튼이니 Button으로
			L"시작",	//자식윈도우에 출력할 테스트
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,	//자식 윈도우 스타일
			365,			//시작좌표 x
			213,			//시작좌표 y
			71,			//윈도우의 넓이 cx
			35,			//윈도우의 높이 cy
			hWnd,		//부모 윈도우 핸들
			(HMENU)400,	//아이디 번호
			g_hInst,	//인스턴스
			NULL);		//윈도우 생성시 필요한 데이터

		return 0;


	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		if (gameState != GAMESTATE_NEW)
		{
			//돈 계속 print 하도록
		}

		EndPaint(hWnd, &ps); 
		return 0;


	//case WM_TIMER:
	//	BringWindowToTop(hWnd);
	//	return 0;


	case WM_LBUTTONDOWN:
		return 0;


	case WM_RBUTTONDOWN: 
		//치트키
		player1.addMoney(100);
		return 0;


	case WM_COMMAND:				//메뉴 선택시 이 메시지가 발생
		switch(LOWORD(wParam))		//메뉴의 ID
		{
			//************************************************************************************
			//  File Menu
			//************************************************************************************

		case ID_MYFILE_NEW:			//파일-새로 만들기
			if (MessageBox(hWnd, L"새로 시작하시겠습니까? 저장하지 않은 데이터는 날아갑니다.", L"새로 만들기", MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				if (gameState == GAMESTATE_WAIT)
				{
					gameState = GAMESTATE_NEW;
					MessageBox(hWnd, L"새로 시작합니다.", L"새로 만들기", MB_OK);
					MessageBox(hWnd, L"플레이어들의 돈이 재설정되었습니다", L"새로 만들기", MB_OK);
				} 
				else
				{
					MessageBeep(-1);
					MessageBox(hWnd, L"게임중에는 새로 시작할 수 없습니다.", L"새로 만들기", MB_OK);
				}
			}
			break;

		case ID_MYFILE_OPEN:		//파일-열기
			if(MessageBox(hWnd, L"진행사항을 불러오시겠습니까?", L"열기", MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				if (gameState == GAMESTATE_WAIT)
				{
					//열기파트
					fin.open("savedata.txt");
					if (!fin)
					{
						MessageBox(hWnd, L"저장된게 없네요.", L"열기", MB_OK);
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
						MessageBox(hWnd, L"진행상태를 불러왔습니다.", L"열기", MB_OK);
					}
				} 
				else
				{
					MessageBeep(-1);
					MessageBox(hWnd, L"게임중에는 불러올 수 없습니다.", L"열기", MB_OK);
				}
			}

			break;

		case ID_MYFILE_SAVE:		//파일-저장
			if(MessageBox(hWnd, L"저장하시겠습니까?", L"저장", MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				if (gameState == GAMESTATE_WAIT)
				{
					//저장파트
					fout.open("savedata.txt");

					fout << player1.getMoney() << endl;
					fout << player2.getMoney() << endl;
					fout << player3.getMoney() << endl;
					fout << player4.getMoney() << endl;
					fout << player5.getMoney();

					fout.close();

					//MessageBeep(MB_ICONASTERISK); //또롱~ 소리
					//MessageBeep(MB_ICONEXCLAMATION); //뽕방~ 소리
					//MessageBeep(MB_ICONHAND); //오류같은소리
					//MessageBeep(MB_ICONQUESTION); //소리 x

					MessageBeep(MB_ICONASTERISK);
					MessageBox(hWnd, L"저장되었습니다", L"저장", MB_OK);
				} 
				else
				{
					MessageBeep(-1);
					MessageBox(hWnd, L"게임중에는 저장할 수 없습니다.", L"저장", MB_OK);
				}

			}

			break;

		case ID_MYFILE_CLOSE:		//파일-종료
			if(MessageBox(hWnd, L"종료하시겠습니까?", L"종료", MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				//free(pfh);
				//KillTimer(hWnd, 1); //삭제
				PostQuitMessage(0);
				return 0;;
			}
			break;


			//************************************************************************************
			//  Button
			//************************************************************************************

		case 100: //하프 버튼 눌렀을 때
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				if (gameState == GAMESTATE_MYTURN && isAvailableHalf)
				{
					isHalfClicked = true;
					//MessageBox(hWnd, L"내 턴일때 하프버튼 클릭", L"하프", MB_OK);
				} 
				else if (!isAvailableHalf)
				{
					//MessageBox(hWnd, L"콜이나 다이 하셔야 됩니다", L"하프", MB_OK);
				}
				else
				{
					//MessageBox(hWnd, L"내 턴이 아닐때 하프버튼 클릭", L"하프", MB_OK);
				}
			}
			break;

		case 200: //콜 버튼 눌렀을 때
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				if (gameState == GAMESTATE_MYTURN && isAvailableCall)
				{
					isCallClicked = true;
					//MessageBox(hWnd, L"내 턴일때 콜버튼 클릭", L"콜", MB_OK);
				} 
				else if (!isAvailableCall)
				{
					MessageBox(hWnd, L"하프나 다이를 하셔야 합니다 (선 플레이어)", L"콜", MB_OK);
				}
				else
				{
					//MessageBox(hWnd, L"내 턴이 아닐때 하프버튼 클릭", L"하프", MB_OK);
				}
			}
			break;

		case 300: //다이 버튼 눌렀을 때
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				if (gameState == GAMESTATE_MYTURN)
				{
					//isAvailableHalf = false;
					//isAvailableCall = false;
					isDieClicked = true;
					//MessageBox(hWnd, L"내 턴일때 다이버튼 클릭", L"다이", MB_OK);
				} 
				else
				{
					//MessageBox(hWnd, L"내 턴이 아닐때 하프버튼 클릭", L"하프", MB_OK);
				}
			}
			break;

		case 400: //시작버튼 눌렀을 때
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				if (startStatus == 0)
				{
					startStatus = 1;
					//MessageBox(hWnd, L"게임을 시작하겠습니다.", L"시작", MB_ICONQUESTION | MB_OK);
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






//update, render에서 쓸 변수들 모음
bool isSecondCard;
int CurrPlayer; //1~5
int NextPlayer; //1~5
bool isBack = false; //카드 뒷면인지 아닌지
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
			//덱만들고 셔플
			deck.newDeck();
			deck.shuffle();

			//돈설정
			player1.setMoney(10000000);
			player2.setMoney(20000000);
			player3.setMoney(30000000);
			player4.setMoney(40000000);
			player5.setMoney(50000000);


			//판돈
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

				//변수 초기화
				if (regame)
				{
					//판돈 알아서 잘 설정
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

					//엔티(ante)
					pandon = (BET * 5);
				}

				deck.newDeck();
				deck.shuffle();

				isAvailableHalf = true;
				isAvailableCall = false; //맨처음사람은 콜 불가

				CurrPlayer = 1;

				//InvalidateRect(global_hWnd, NULL, TRUE);

				gameState = GAMESTATE_MYTURN;
			}
		}
		break;




	case GAMESTATE_MYTURN:
		{
			if( (cardn == 3) || dieman == 4 ) //한명빼고 다 다이 or (베팅끝 && 카드2장)
			{ 
				//종료처리
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

			//한장씩받고 베팅시작
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
						//카드한장씩 뿌리는걸로 넘어가게.
						//cardn = 2; 나중에 추가
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
						//하프 눌렸을 때
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
							// 여기 안들어옴

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


					//die 상태일 때 내 턴이 오면
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
					//게임 끝났을 때
					gameState = GAMESTATE_PRINTCARD;
				}

				if (tempCount == 0)
				{
					//하프 눌렸을 때
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
						// 여기 안들어옴

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


				//die 상태일 때 내 턴이 오면
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
					{ //90% 확률로 하프, 10%확률로 콜
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
						//call만 가능
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
						//call만 가능
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
						//call만 가능
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
						//call만 가능
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
				//카드 두장씩 받고 베팅
				switch (tempCount)
				{ //90% 확률로 하프, 10%확률로 콜
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
					//call만 가능
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
					//call만 가능
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
					//call만 가능
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
					//call만 가능
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



	//첫번째카드 오픈
	case GAMESTATE_PRINTCARD:
		{
			//이거 전역에 설정해야됨
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
			int winPlayer = 1; //이긴사람. 1=나, 2~~ = 나머지

			//승패처리
			//이걸 어떻게 구현 ? 흐음


			player1.evalJokbo();
			player2.evalJokbo();
			player3.evalJokbo();
			player4.evalJokbo();
			player5.evalJokbo();


			//숫자 낮을수록 높은족보임 !!!
			//1 = 38광땡
			int maxJokbo;
			bool maxPlayer[5] = {0};
			bool isDraw = false;
			int tempJokbo[5] = {player1.getJokbo(), player2.getJokbo(), player3.getJokbo(), player4.getJokbo(), player5.getJokbo()};


			//Draw 감지


			
			//win player 감지
			if (isDraw) //같은 족보가 있을때 or 구사
			{
			}
			else
			{
				winPlayer = 1;
				maxJokbo = tempJokbo[0];

				for (int i=1; i<5; i++)
				{
					//특수패일때는 continue하도록
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


			////특수패 처리
			//암행어사
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

			//땡잡이
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





			//끗땡 대결 할수있도록 수정
			//버튼으로 흠,...

			
			//regame


			//암행어사 구현

			//땡잡이 구현



			//94 나왔을 때
			/*if(regame)
			{
				gameState = GAMESTATE_WAIT;
				regame = false;
			}*/
			//이 아래부분을 else로 묶기?
			//재경기면 다들 판돈 1/2씩 내도록
			//GAMESTATE_REGAME 추가?



			wchar_t win_buffer[256];
			//무슨 족보로 승리했는지도 나오게 했으면 좋겠음 근데 어떻게하징

			switch (winPlayer)
			{
			case 1:
				player1.addMoney(pandon);
				wsprintfW(win_buffer, L"1 플레이어 승. ( + %d 원 )", pandon);
				break;

			case 2:
				player2.addMoney(pandon);
				wsprintfW(win_buffer, L"2 플레이어 승. ( + %d 원)", pandon);
				break;

			case 3:
				player3.addMoney(pandon);
				wsprintfW(win_buffer, L"3 플레이어 승. ( + %d 원)", pandon);
				break;

			case 4:
				player4.addMoney(pandon);
				wsprintfW(win_buffer, L"4 플레이어 승. ( + %d 원)", pandon);
				break;

			case 5:
				player5.addMoney(pandon);
				wsprintfW(win_buffer, L"5 플레이어 승. ( + %d 원)", pandon);
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


			if (MessageBox(global_hWnd, win_buffer, L"결과", MB_OK | MB_ICONINFORMATION) == IDOK)
			{
				InvalidateRect(global_hWnd, NULL, TRUE);
			}


			//1p의 오링 감지
			if (player1.getMoney() <= 0)
			{
				if(MessageBox(global_hWnd, L"올인당했습니다. 새로 시작하시겠습니까?", L"패배", MB_ICONQUESTION | MB_YESNO) == IDYES)
				{
					gameState = GAMESTATE_NEW;
				}
				else
				{
					//종료
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


	//여기에 돈, 카드 등 render 하던지 WM_PAINT에 출력하는거 넣어야 안지워짐


	return;
} //update


HFONT hFont, hOldFont;
HBRUSH MyBrush, OldBrush;
HPEN MyPen, OldPen;

void Render()
{
	//돈 출력

	//폰트설정
	hFont = CreateFont(
		14,				//높이
		0,				//폭. 0이면 자동
		0, 0,			//각도
		FW_NORMAL,		//두께
		0, 0, 0, //기울임, 밑줄, 관통선
		HANGEUL_CHARSET,		//charset
		OUT_DEFAULT_PRECIS,		//출력 정확도
		CLIP_DEFAULT_PRECIS,	//클리핑 정확도
		DEFAULT_QUALITY,		//폰트의 질
		VARIABLE_PITCH | FF_ROMAN,	//폰트의 피치와 그룹
		L"돋움"					//폰트의 이름
		);

	//
	// int to LPWSTR(LPCWSTR) 대충 아래처럼 하면 됨
	//
	//int a = 16;
	//wchar_t str_buffer[256];
	//wsprintfW(str_buffer, L"%d", a);
	//

	//돈을 string으로 저장하고있는 버퍼 만듬
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


		//카드설정
		//Card cd(5, false);
		int tempCd;
		int tempSpec; //0 -> 노말, 1 -> 멍텅구리
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

		case 2: //뒷면
			tempCd = IDB_CARD_00;
			break;
		}




		//tempCd 설정


		hBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(tempCd));
		//hBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CARD_101));

		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

		GetObject(hBitmap, sizeof(BITMAP), &Bitmap);

		nWidth = Bitmap.bmWidth;
		nHeight = Bitmap.bmHeight;

		//이거 bitblt로 수정
		//좌표도.. 구현
		int nX = 365;
		int nY = 279;

		//2번째 카드일 경우
		//좌표에 x축을 플러스

		//bool isSecondCard; 추가
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


		//이건 특정 색 투명하게 출력하는거
		//TransparentBlt(hdc, 365, 279, nWidth, nHeight, hMemDC, 0, 0, nWidth, nHeight, RGB(0, 128, 128)); //특정 색 투명하게 설정해서 출력

		if (CurrPlayer != 1 || player1.getJokbo() != 99999)
		{
			BitBlt(hdc, nX, nY, nWidth, nHeight, hMemDC, 0, 0, SRCCOPY);
		}

		//BitBlt(hdc, nX, nY, nWidth, nHeight, hMemDC, 0, 0, SRCCOPY);
		//패가 보내질 때 효과음 682p

		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
		DeleteObject(hBitmap);

		isPrintCard = false;
	}

	return;
} //render