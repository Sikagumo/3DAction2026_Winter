#include "TitleScene.h"
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/UtilityMath.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/InputManager.h"

TitleScene::TitleScene(void)
	: SceneBase()
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Initialize(void)
{
	// ‰æ‘œ“Ç‚İ‚İ
	imageTitle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMAGE_TITLE);

	// ƒJƒƒ‰“o˜^
	SceneManager::GetInstance().GetCamera().ChangeMode(Camera::MODE::FIXED_POINT);
}

void TitleScene::Update(void)
{
	// ƒV[ƒ“‘JˆÚ
	if (InputManager::GetInstance().IsTrgDown(InputManager::TYPE::GAME_STATE_CHANGE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

void TitleScene::Draw(void)
{
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 250, 1.0, 0.0, imageTitle_, true);
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 500, 1.0, 0.0, imgPush_, true);
}
