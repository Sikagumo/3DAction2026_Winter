#include "SceneBase.h"
#include "../Application.h"
#include "../Utility/UtilityMath.h"
#include "../Utility/UtilityCollision.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Object/Actor/SkyDome.h"
#include "../Object/Collider/ColliderBase.h"
#include "../Object/Actor/Character/Player.h"
#include "../Object/Actor/Character/Boss.h"
#include "../Object/Actor/Stage.h"
#include "../Object/Collider/ColliderCapsule.h"
#include "../Object/Collider/ColliderSphere.h"
#include "../Object/Common/AnimationController.h"
#include <DxLib.h>
#include "GameScene.h"

namespace
{
	constexpr float TIME_PERFORM_PREMOVIE = 7.5f;
};

GameScene::GameScene(void)
	: SceneBase()
	, mode_(MODE::MAIN)
{
}

GameScene::~GameScene(void)
{
	//DeleteGraph(postEffectScreen_);
}

void GameScene::Initialize(void)
{
	// フォグ割り当て
	constexpr int FOG_COLOR_R = 0;
	constexpr int FOG_COLOR_G = 0;
	constexpr int FOG_COLOR_B = 0;
	constexpr float FOG_POS_START = 1000.0f;
	constexpr float FOG_POS_END = 1500.0f;
	SetFogEnable(TRUE);
	SetFogColor(FOG_COLOR_R, FOG_COLOR_G, FOG_COLOR_B);
	SetFogStartEnd(FOG_POS_START, FOG_POS_END);


	// プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize();

	// プレイヤー
	boss_ = std::make_unique<Boss>();
	boss_->Initialize();

	// ステージ
	stage_ = std::make_unique<Stage>(player_);
	stage_->Initialize();

	// ステージモデルのコライダーをキャラクターに登録
	player_->AddHitCollider(stage_->GetOwnCollider(0));
	boss_->AddHitCollider(stage_->GetOwnCollider(0));

	// 相手の攻撃範囲を取得
	player_->AddHitCollider(boss_->GetOwnCollider(static_cast<int>(CharaBase::COLLIDER_TYPE::ATTACK_AREA)));
	boss_->AddHitCollider(player_->GetOwnCollider(static_cast<int>(CharaBase::COLLIDER_TYPE::ATTACK_AREA)));

	// スカイドーム
	skyDome_ = std::make_unique<SkyDome>(player_->GetTransform());
	skyDome_->Initialize();

	// カメラ登録
	SceneManager::GetInstance().GetCamera().SetFollow(&player_->GetTransform());
	SceneManager::GetInstance().GetCamera().ChangeMode(Camera::MODE::FOLLOW);

	// 演出時間割り当て
	SceneManager::GetInstance().SetPerformTime(TIME_PERFORM_PREMOVIE);

	/*	
	// ポストエフェクト用スクリーン
	postEffectScreen_ = MakeScreen(
		Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, true);

	// ポストエフェクト用(モノトーン)
	monoMaterial_ = std::make_unique<PixelMaterial>("Monotone.cso", 1);
	monoMaterial_->AddConstBuf({ 1.0f, 1.0f, 1.0f, 1.0f });
	monoMaterial_->AddTextureBuf(SceneManager::GetInstance().GetMainScreen());
	monoRenderer_ = std::make_unique<PixelRenderer>(*monoMaterial_);
	monoRenderer_->MakeSquereVertex(
		Vector2(0, 0),
		Vector2(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y)
	);

	// ポストエフェクト用(走査線)
	scanMaterial_ = std::make_unique<PixelMaterial>("ScanLine.cso", 2);
	scanMaterial_->AddConstBuf({ 1.0f, 1.0f, 1.0f, 1.0f });
	scanMaterial_->AddConstBuf({ 0.0f, 0.0f, 0.0f, 0.0f });
	scanMaterial_->AddTextureBuf(SceneManager::GetInstance().GetMainScreen());
	scanRenderer_ = std::make_unique<PixelRenderer>(*scanMaterial_);
	scanRenderer_->MakeSquereVertex(
		Vector2(0, 0),
		Vector2(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y)
	);

	// ポストエフェクト用(ビネット)
	vineMaterial_ = std::make_unique<PixelMaterial>("Vignette.cso", 1);
	vineMaterial_->AddConstBuf({ 3.5f, 0.0f, 0.0f, 0.0f });
	vineMaterial_->AddTextureBuf(SceneManager::GetInstance().GetMainScreen());
	vineRenderer_ = std::make_unique<PixelRenderer>(*vineMaterial_);
	vineRenderer_->MakeSquereVertex(
		Vector2(0, 0),
		Vector2(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y)
	);

	// ポストエフェクト用(レンズの歪み)
	lensMaterial_ = std::make_unique<PixelMaterial>("LensDistortion.cso", 1);
	lensMaterial_->AddConstBuf({ 3.5f, 0.0f, 0.0f, 0.0f });
	lensMaterial_->AddTextureBuf(SceneManager::GetInstance().GetMainScreen());
	lensRenderer_ = std::make_unique<PixelRenderer>(*lensMaterial_);
	lensRenderer_->MakeSquereVertex(
		Vector2(0, 0),
		Vector2(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y)
	);*/


	// 初期モード
	mode_ = MODE::MAIN;

}

void GameScene::Update(void)
{
	// シーン遷移
	if (InputManager::GetInstance().IsTrgDown(InputManager::TYPE::PAUSE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	// モード切替
	/*
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_BACKSLASH))
	{
		int tmp = (int)mode_;
		tmp++;
		if (tmp >= (int)MODE::MAX)
		{
			tmp = 0;
		}
		mode_ = (MODE)tmp;
	}
	*/

	skyDome_->Update();

	stage_->Update();

	player_->Update();

	boss_->Update();

	if (!SceneManager::GetInstance().IsActivePerformance())
	{
		if (boss_->GetHp() <= 0)
		{
			SceneManager::GetInstance().SetPerformTime(3.0f);
		}
		else if (player_->GetHp() <= 0)
		{
			SceneManager::GetInstance().SetPerformTime(3.0f);
		}
	}
	else if (boss_->GetHp() <= 0 && boss_->GetAnimation()->IsEnd()
		|| player_->GetHp() <= 0 && player_->GetAnimation()->IsEnd())
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	Collision();
}

void GameScene::Draw(void)
{
	// 背景
	skyDome_->Draw();
	stage_->Draw();
	player_->Draw();
	boss_->Draw();

#ifdef _DEBUG

	player_->DrawDebug();

	boss_->DrawDebug();

#endif

	int mainScreen = SceneManager::GetInstance().GetMainScreen();

	// ポストエフェクト(モノクロ)
	//-----------------------------------------
	if (mode_ < MODE::MONO) { return; }
	
	//SetDrawScreen(postEffectScreen_);

	// 画面を初期化
	ClearDrawScreen();

	// 画面シェーダーを反映
	SetShaderScreen();
}

void GameScene::Collision(void)
{
	constexpr int COLLIDER_TYPE_ATTACK_AREA = static_cast<int>(CharaBase::COLLIDER_TYPE::ATTACK_AREA);
	constexpr int COLLIDER_TYPE_CAPSULE = static_cast<int>(CharaBase::COLLIDER_TYPE::CAPSULE);

	const auto& playerAttack = dynamic_cast<const ColliderSphere*>(player_->GetOwnCollider(COLLIDER_TYPE_ATTACK_AREA));
	const auto& bossCapsule = dynamic_cast<const ColliderCapsule*>(boss_->GetOwnCollider(COLLIDER_TYPE_CAPSULE));

	if (UtilityCollision::IsHitSphereToCapsule(*playerAttack, *bossCapsule))
	{
		boss_->SetDamage(player_->GetAttackPower(), 0);
	}
}

void GameScene::SetShaderScreen(void)
{
	/*
	monoRenderer_->Draw();

	// メインに戻す
	SetDrawScreen(mainScreen);
	DrawGraph(0, 0, postEffectScreen_, false);
	//-----------------------------------------

	// ポストエフェクト(走査線)
	//-----------------------------------------
	if (mode_ < MODE::SCAN) { return; }

	SetDrawScreen(postEffectScreen_);

	// 画面を初期化
	ClearDrawScreen();

	scanMaterial_->SetConstBuf(
		1,
		{ SceneManager::GetInstance().GetTotalTime(), 0.0f, 0.0f, 0.0f }
	);
	scanRenderer_->Draw();

	// メインに戻す
	SetDrawScreen(mainScreen);
	DrawGraph(0, 0, postEffectScreen_, false);
	//-----------------------------------------


	// ポストエフェクト(レンズの歪み)
	//-----------------------------------------
	if (mode_ < MODE::LENS) { return; }

	SetDrawScreen(postEffectScreen_);

	// 画面を初期化
	ClearDrawScreen();

	lensRenderer_->Draw();

	// メインに戻す
	SetDrawScreen(mainScreen);
	DrawGraph(0, 0, postEffectScreen_, false);
	//-----------------------------------------


	// ポストエフェクト(ビネット)
	//-----------------------------------------
	if (mode_ < MODE::VINE) { return; }

	SetDrawScreen(postEffectScreen_);

	// 画面を初期化
	ClearDrawScreen();

	vineRenderer_->Draw();

	// メインに戻す
	SetDrawScreen(mainScreen);
	DrawGraph(0, 0, postEffectScreen_, false);
	//-----------------------------------------
	*/
}
