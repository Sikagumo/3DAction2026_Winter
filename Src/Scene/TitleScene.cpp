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
	// âÊëúì«Ç›çûÇ›
	imageTitle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMAGE_TITLE);
}

void TitleScene::Update(void)
{
	// ÉVÅ[ÉìëJà⁄
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

void TitleScene::Draw(void)
{
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 250, 1.0, 0.0, imageTitle_, true);
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 500, 1.0, 0.0, imgPush_, true);
}
