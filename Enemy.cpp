#include "DxLib.h"
#include "Enemy.h"

static int g_Image;
static int g_y;

void Enemy_Initialize()
{
	g_Image = LoadGraph("CHARA/Enemy.png");
	g_y = 0;
}

void Enemy_Update()
{
	
}

void Enemy_Draw()
{
	DrawGraph(0, g_y, g_Image, TRUE);
	DrawRotaGraph(0, g_y, 0.7, 0, g_Image, FALSE);

}

void Enemy_Finalize()
{
	DeleteGraph(g_Image);
}