#include "SceneBase.h"
#include "../Application.h"
#include "../Utility/UtilityMath.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/InputManager.h"
#include "../Camera/Camera.h"
#include <DxLib.h>
#include "TitleScene.h"

namespace
{
	constexpr int UI_POS_TITLE = 325;
	constexpr int UI_POS_PUSH = 750;

	constexpr float TITLE_SCALE = 0.55f;
	constexpr float PUSH_SCALE = 1.0f;
};
TitleScene::TitleScene(void)
	: SceneBase::SceneBase()
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Initialize(void)
{
	// âÊëúì«Ç›çûÇ›
	imageTitle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMAGE_TITLE);
	imagePush_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMAGE_UI_PUSH_SPACE);

	// ÉJÉÅÉâìoò^
	SceneManager::GetInstance().GetCamera().ChangeMode(Camera::MODE::FIXED_POINT);
}

void TitleScene::Update(void)
{
	// ÉVÅ[ÉìëJà⁄
	if (InputManager::GetInstance().IsTrgDown(InputManager::TYPE::SELECT_DECISION))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

void TitleScene::Draw(void)
{
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, UI_POS_TITLE
		, TITLE_SCALE, 0.0, imageTitle_, true);

	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, UI_POS_PUSH
		, PUSH_SCALE, 0.0, imagePush_, true);
}
