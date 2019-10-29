#include "DxLib.h"
#include "Player.h"

static int m_Image;
static int m_y;

void Player_Initialize()
{
	m_Image = LoadGraph("CHARA/Player.png");
	m_y = 500;
}

void Player_Update()
{
	
}

void Player_Draw() 
{
	DrawGraph(0, m_y, m_Image, TRUE);
	DrawRotaGraph(0, m_y, 0.7, 0, m_Image, FALSE);

}

void Player_Finalize() 
{
	DeleteGraph(m_Image);
}