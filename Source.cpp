#include "DxLib.h"
#include "Player.h"
#include "Enemy.h"

#define GAME_WIDTH 800
#define GAME_HEIGHT 600
#define GAME_COLOR 32

#define GAME_BACKGROUND "BACK\\space.jpg"

#define GAME_WINDOW_NAME "INVADER VS INVADER"
#define GAME_WINDOW_MODECHANGE FALSE

#define SET_WINDOW_ST_MODE_DEFAULT 0
#define SET_WINDOW_ST_MODE_TITLE_NONE 1
#define SET_WINDOW_ST_MODE_TITLE_FLAME_NONE 2
#define SET_WINDOW_ST_MODE_FLAME_NONE 3

struct STRUCT_GAZOU {
	int Handle;
	char FilePath[128];
	int X;
	int Y;
	int Width;
	int Height;
	int C_Width;
	int C_Height;

	int MoveSpeed;
};

typedef STRUCT_GAZOU GAZOU;

GAZOU BackGround;

#define GAME_FPS_SPEED 60

enum GAME_SCENE {
	GAME_SCENE_TITLE,
	GAME_SCENE_PLAY,
	GAME_SCENE_END
};

int StartTimeFps;
int CountFps;
float CalcFps;
int SampleNumFps = GAME_FPS_SPEED;

WNDPROC WndProc;
char AllKeyState[256];//全てのキーの状態を入れる

int GameSceneNow = (int)GAME_SCENE_TITLE;

BOOL IsWM_CREATE = FALSE;

int DrawX = 0;
int DrawY = 0;
double ExRateX = 0.0;
double ExRateY = 0.0;
double RotAngle = 0.0;
double rad = 6.28;

VOID MY_FPS_UPDATE(VOID);
VOID MY_FPS_DRAW(VOID);
VOID MY_FPS_WAIT(VOID);

VOID MY_GAZOU_LOAD(GAZOU *, int, int, const char *);

VOID MY_ALL_KEYDOWN_UPDATE(VOID);

VOID MY_GAME_TITLE(VOID);
VOID MY_GAME_PLAY(VOID);
VOID MY_GAME_END(VOID);

LRESULT CALLBACK MY_WNDPROC(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(GAME_WINDOW_MODECHANGE);
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);
	SetWindowStyleMode(SET_WINDOW_ST_MODE_DEFAULT);
	SetMainWindowText(TEXT(GAME_WINDOW_NAME));
	SetHookWinProc(MY_WNDPROC);

	if (DxLib_Init() == -1) { return -1; }
	SetDrawScreen(DX_SCREEN_BACK); //ダブルバッファリング

	MY_GAZOU_LOAD(&BackGround, 0, 0, GAME_BACKGROUND);

	while (TRUE)
	{
		if (ProcessMessage() != 0) { break; }//メッセージ処理の結果がエラーの時、強制終了
		if (ClearDrawScreen() != 0) { break; }//画面を消去できなかった時、強制終了
		
		MY_ALL_KEYDOWN_UPDATE();

		switch (GameSceneNow)
		{
		case(int)GAME_SCENE_TITLE:
			MY_GAME_TITLE();
			break;
		case(int)GAME_SCENE_PLAY:
			MY_GAME_PLAY();
			break;
		case(int)GAME_SCENE_END:
			MY_GAME_END();
			break;
		default:
			break;

		}
		if (DrawX < GAME_WIDTH / 2 - 180)
		{
			DrawX++;			//横位置を移動
			DrawY++;			//縦位置を移動
			ExRateX += 0.01;
			ExRateY += 0.01;
		}
		if (RotAngle <= rad * 4)
		{
			RotAngle += 0.1;
		}
		
		MY_FPS_UPDATE();	//ゲーム画面を描画
		MY_FPS_DRAW();		//fPSの処理（描画）
		ScreenFlip();		//モニタのリフレッシュレートの速さで裏画面を再描画
		MY_FPS_WAIT();		//FPSの処理（待つ）
	}
	DeleteGraph(BackGround.Handle);
	Player_Finalize();
	Enemy_Finalize();
	DxLib_End();
	return 0;

}

VOID MY_FPS_UPDATE(VOID)
{
	if (CountFps == 0)//1フレーム目なら
	{
		StartTimeFps = GetNowCount();//現在のカウントをこの変数に代入
	}

	if (CountFps == SampleNumFps) //60フレーム目なら平均を計算
	{
		int now = GetNowCount();
		CalcFps = 1000.f / ((now - StartTimeFps) / (float)SampleNumFps);
		CountFps = 0;
		StartTimeFps = now;
	}
	CountFps++;
	return;
}

VOID MY_FPS_DRAW(VOID)
{
	//FPS値を描画
	DrawFormatString(0, GAME_HEIGHT - 20, GetColor(255, 255, 255), "FPS:%.1f", CalcFps);
	return;
}

VOID MY_FPS_WAIT(VOID)
{
	int resultTime = GetNowCount() - StartTimeFps;//かかった時間
	int waitTime = CountFps * 1000 / GAME_FPS_SPEED - resultTime;//待つべき時間
	
	if (waitTime > 0)//指定のFPS値よりも早い場合
	{
		Sleep(waitTime);//待機
	}
	return;
}

VOID MY_ALL_KEYDOWN_UPDATE(VOID)
{
	char TempKey[256];
	GetHitKeyStateAll(TempKey);
	for (int i = 0; i < 256; i++)
	{
		if (TempKey[i] != 0)
		{
			AllKeyState[i]++;
		}
		else
		{
			AllKeyState[i] = 0;
		}
	}
	return;
}

VOID MY_GAME_TITLE(VOID)
{
	if (AllKeyState[KEY_INPUT_SPACE] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_PLAY;
	}
	//タイトル画面の文字列を描画
	DrawGraph(BackGround.X, BackGround.Y, BackGround.Handle, TRUE);
	DrawRotaGraph(GAME_WIDTH / 2, GAME_HEIGHT / 2, 0.7, 0, BackGround.Handle, FALSE);
	//int Font = CreateFontToHandle("メイリオ", 20, 5, -1);
	DrawRotaString(DrawX, DrawY - 30, ExRateX, ExRateY, 0, 0, RotAngle, GetColor(255, 0, 0),GetColor(255, 0, 0), FALSE, GAME_WINDOW_NAME);
	DrawString(0, 0, "タイトル画面（スペースキーを押してください）", GetColor(255, 255, 255));
	return;
}

VOID MY_GAME_PLAY(VOID)
{
	Player_Initialize();
	Enemy_Initialize();
	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0) 
	{
		Player_Draw();
		Enemy_Draw();
		Player_Update();
		Enemy_Update();
	}

	/*if (Player.life <= 0)
	{
		MY_GAME_END(2);
		GameSceneNow = (int)GAME_SCENE_END;
	}
	if(Enemy.life <= 0)
	{
		MY_GAME_END(1);
		GameSceneNow = (int)GAME_SCENE_END;
	}*/
	return;
}

VOID MY_GAME_END(VOID)//勝ったほうを引数として取得
{
	if (AllKeyState[KEY_INPUT_RETURN] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}
	DrawString(0, 0, "エンド画面（エンターキーを押してください）", GetColor(255, 255, 255));
	DrawFormatString(GAME_WIDTH / 2, GAME_HEIGHT / 2, GetColor(255, 255, 255), "%sP WIN!\n");
	return;
}

LRESULT CALLBACK MY_WNDPROC(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CREATE:
		IsWM_CREATE = TRUE;
		return 0;
	case WM_CLOSE:
		MessageBox(hwnd, TEXT("ゲームを終了します"), TEXT("終了メッセージ"), MB_OK);
		break;
	case WM_RBUTTONDOWN:
		SendMessage(hwnd, WM_CLOSE, 0, 0);
		break;
	case WM_LBUTTONDOWN:
		PostMessage(hwnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, lp);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

VOID MY_GAZOU_LOAD(GAZOU *g, int x, int y, const char *path)
{
	wsprintf(g->FilePath, path);
	g->Handle = LoadGraph(g->FilePath);
	g->X = x;
	g->Y = y;
	g->C_Width = g->Width / 2;
	g->C_Height = g->Height / 2;
	return;
}