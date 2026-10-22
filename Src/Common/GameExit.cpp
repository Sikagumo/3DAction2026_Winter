#include "GameExit.h"
#include "./Font.h"
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/SoundManager.h"
#include <DxLib.h>


GameExit::GameExit(void):
	select_(SELECT::NONE),
	isEnd_(false),
	isActiveMenu_(false),
	sceneMng_(SceneManager::GetInstance()),
	input_(InputManager::GetInstance())
{
	Init();
}


void GameExit::Init(void)
{
	/*　初期化処理　*/

	// 終了無効 状態化
	select_ = SELECT::NO;

	// 終了無効化
	isEnd_ = false;

	// メニュー 無効化
	isActiveMenu_ = false;
}

void GameExit::Update(void)
{
	/*　更新処理　*/

	if (input_.IsTrgDown(InputManager::TYPE::PAUSE))
	{
		isActiveMenu_ = ((!isActiveMenu_) ? true : false);

		if (isActiveMenu_)
		{
			// 音量を通常に戻す
			SoundManager::GetInstance().SetVolumeMaster(SoundManager::VOLUME_MASTER_MAX);
		}
		else
		{
			// 終了無効 状態化
			select_ = SELECT::NO;

			// 音量を半減
			SoundManager::GetInstance().
				SetVolumeMaster(SoundManager::GetInstance().GetVolumeMaster() / 2);
		}

	}

	// 一時停止時、マウスカーソル無効化解除
	input_.SetCursorMode((!isActiveMenu_) ? Input::CURSOR_MODE::CONFINED_SIDE
										 : Input::CURSOR_MODE::NONE);

	if (isActiveMenu_)
	{
		/*　各選択キー 更新処理　*/

		/*　選択状態 遷移処理　*/
		if (IsSelect())
		{
			// YES状態とNO状態を切り替える
			select_ = ((select_ == SELECT::NO) ? SELECT::YES : SELECT::NO);
		}

		if (InputManager::GetInstance().IsTrgDown(InputManager::TYPE::SELECT_DECISION))
		{
			// ゲーム終了処理
			if (select_ == SELECT::YES)
			{
				isActiveMenu_ = false;

				switch (sceneMng_.GetSceneId())
				{
					case SceneManager::SCENE_ID::TITLE:
					{
						// ゲーム終了
						Application::GetInstance().SetIsGameEnd();
					}
					break;

					case SceneManager::SCENE_ID::GAME:
					{
						// タイトルに戻る
						sceneMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
					}
					break;
				}
			}
			else
			{
				// メニュー非表示
				isActiveMenu_ = false;
			}
		}
	}
}

void GameExit::Draw(void)
{
	// メニュー未有効時、処理終了
	if (!isActiveMenu_) { return; }

	Font& font = Font::GetInstance();
	
	// 画面の中央座標
	int midX = Application::SCREEN_HALF_X;
	int midY = Application::SCREEN_HALF_Y;

	// 決定テキスト位置
	int yesX = ((midX - TEXT_OFFSET.x) - font.GetDefaultTextWidth(TEXT_YES));
	int yesY = (midY + TEXT_OFFSET.y);

	// キャンセルテキスト位置
	int noX = ((midX + TEXT_OFFSET.x) - font.GetDefaultTextWidth(TEXT_NO));
	int noY = (midY + TEXT_OFFSET.y);



	// 背景 半透明描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, MENU_COLOR.a);
	DrawBox(0, Application::SCREEN_SIZE_X,
			0, Application::SCREEN_SIZE_Y,
			UtilityCommon::SetColor(MENU_COLOR), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// メニュー画面 半透明描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, MENU_COLOR.a);
	DrawBox((midX - MENU_WIDTH), (midY - MENU_HEIGHT),
			(midX + MENU_WIDTH), (midY + MENU_HEIGHT),
			GetColor(MENU_COLOR.r, MENU_COLOR.g, MENU_COLOR.r), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);


	// メニューテキスト表示
	const char* text = TEXT_LABEL_END;

	if (sceneMng_.GetSceneId() != SceneManager::SCENE_ID::TITLE)
	{
		// タイトル遷移テキスト
		text = TEXT_LABEL_TITLE;
	}

	int x = midX - font.GetDefaultTextWidth(text) + MENU_TEXT_OFFSET.x;
	int y = (midY + MENU_TEXT_OFFSET.y);
	font.DrawTextA("GameFont", x, y, text,
				   UtilityCommon::SetColor(MENU_TEXT_COLOR),
				   50, Font::FONT_TYPE_ANTIALIASING_EDGE);


	if (select_ == SELECT::YES)
	{
		// "YES"選択 通常描画
		font.DrawTextA("GameFont", yesX, yesY, TEXT_YES,
					   UtilityCommon::SetColor(MENU_TEXT_COLOR),
					   -1, Font::FONT_TYPE_ANTIALIASING_EDGE);

		// "NO"選択 半透明描画
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, MENU_TEXT_COLOR.a);

		font.DrawTextA("GameFont", noX, noY, TEXT_NO,
					   GetColor(MENU_TEXT_COLOR.r, MENU_TEXT_COLOR.g, MENU_TEXT_COLOR.b),
					   -1, Font::FONT_TYPE_ANTIALIASING_EDGE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	else if (select_ == SELECT::NO)
	{
		// "NO"選択 通常描画
		font.DrawTextA("GameFont", noX, noY, TEXT_NO,
					   UtilityCommon::SetColor(MENU_TEXT_COLOR),
					   -1, Font::FONT_TYPE_ANTIALIASING_EDGE);

		// "YES"選択 半透明描画
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, MENU_TEXT_COLOR.a);
		font.DrawTextA("GameFont", yesX, yesY, TEXT_YES,
					   UtilityCommon::SetColor(MENU_TEXT_COLOR),
					   -1, Font::FONT_TYPE_ANTIALIASING_EDGE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void GameExit::Release(void)
{
	/*　解放処理　*/
}

bool GameExit::IsSelect(void)
{
	bool ret = false;
	if (input_.IsTrgDown(InputManager::TYPE::SELECT_LEFT) ||
		input_.IsTrgDown(InputManager::TYPE::SELECT_RIGHT) ||
		input_.IsTrgDown(InputManager::TYPE::SELECT_UP) ||
		input_.IsTrgDown(InputManager::TYPE::SELECT_DOWN))
	{
		ret = true;
	}

	return ret;
}
