#include "SceneManager.h"
#include <chrono>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Application.h"
#include "../Common/Fader.h"
#include "../Scene/TitleScene.h"
#include "../Scene/GameScene.h"
#include "./ResourceManager.h"
#include "../Utility/UtilityMath.h"

namespace
{
	constexpr SceneManager::SCENE_ID START_SCENE = SceneManager::SCENE_ID::GAME;

	constexpr COLOR_U8 COLOR_BACK = { 0, 139, 139 };

	constexpr VECTOR LIGHT_DIR = { 0.3f, -0.7f, 0.8f };

	constexpr COLOR_U8 COLOR_FOG = { 0.0f, 0.0f, 0.0f };
	constexpr float FOG_START = 5000.0f;
	constexpr float FOG_END = 20000.0f;
};

SceneManager* SceneManager::instance_ = nullptr;

SceneManager::SceneManager(void)
	: sceneId_(SCENE_ID::NONE)
	, waitSceneId_(SCENE_ID::NONE)
	, mainScreen_(-1)
	, deltaTime_(1.0f / 60.0f)
{
}

void SceneManager::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	instance_->Initialize();
}

SceneManager& SceneManager::GetInstance(void)
{
	return *instance_;
}

void SceneManager::Initialize(void)
{
	sceneId_ = SCENE_ID::TITLE;
	waitSceneId_ = SCENE_ID::NONE;

	fader_ = std::make_unique<Fader>();
	fader_->Initialize();

	// 演出
	performance_ = std::make_unique<Performance>();
	performance_->Initialize();

	// カメラ
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();

	isSceneChanging_ = false;

	// デルタタイム
	preTime_ = std::chrono::system_clock::now();

	// 画面割り当て
	mainScreen_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, true);

	// 3D用の設定
	Init3D();

	// 初期シーンの設定
	DoChangeScene(START_SCENE);
}

void SceneManager::Init3D(void)
{
	// 背景色設定
	SetBackgroundColor(COLOR_BACK.r, COLOR_BACK.g, COLOR_BACK.b);

	// Zバッファを有効にする
	SetUseZBuffer3D(true);

	// Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(true);

	// バックカリングを有効にする
	SetUseBackCulling(true);

	// ライトの設定
	SetUseLighting(true);
	
	// ライトの設定
	ChangeLightTypeDir(UtilityMath::VNormalize(LIGHT_DIR));	

	// フォグ設定
	SetFogEnable(true);
	SetFogColor(COLOR_FOG.r, COLOR_FOG.g, COLOR_FOG.b);
	SetFogStartEnd(FOG_START, FOG_END);
}

void SceneManager::Update(void)
{
	if (scene_ == nullptr) { return; }

	const bool IS_PRE_HIT_STOP = performance_->IsHitStop();

	UpdateDeltaTime();

	performance_->Update();

	// 遅延の間引きフレームでは更新しない
	if (performance_->IsSkipFrame()) { return; }

	if (!IS_PRE_HIT_STOP)
	{
		// ゲーム実行時間
		totalTime_ += deltaTime_;

		if (CheckHitKey(KEY_INPUT_RETURN))
		{
			performance_->StartHitSlow(3.0f);
		}

		fader_->Update();

		if (isSceneChanging_)
		{
			FadeScreen();
		}
		else
		{
			scene_->Update();
		}

		// カメラ更新
		camera_->Update();
	}
}

void SceneManager::Draw(void)
{
	// 描画先グラフィック領域の指定
	// (３Ｄ描画で使用するカメラの設定などがリセットされる)
	SetDrawScreen(mainScreen_);

	// 画面を初期化
	ClearDrawScreen();

	// カメラ設定
	camera_->SetBeforeDraw();

	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();

	// 描画
	scene_->Draw();

	// 主にポストエフェクト用
	camera_->Draw();

	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();
	
	// 暗転・明転
	fader_->Draw();

	// 背面スクリーンにメインスクリーンを描画
	SetDrawScreen(DX_SCREEN_BACK);
	Vector2 offset = performance_->GetDrawOffset();
	DrawGraph(offset.x, offset.y, mainScreen_, true);
}

void SceneManager::DestroyInstance(void)
{
	DeleteGraph(mainScreen_);
	delete instance_;
}

void SceneManager::ChangeScene(SCENE_ID nextId)
{
	// フェード処理が終わってからシーンを変える場合もあるため、
	// 遷移先シーンをメンバ変数に保持
	waitSceneId_ = nextId;

	// フェードアウト(暗転)を開始する
	fader_->SetFade(Fader::STATE::FADE_OUT);
	isSceneChanging_ = true;
}


float SceneManager::GetDeltaTime(void) const
{
	//return 1.0f / 60.0f;
	return deltaTime_;
}

void SceneManager::ResetDeltaTime(void)
{
	deltaTime_ = 0.016f;
	preTime_ = std::chrono::system_clock::now();
}

void SceneManager::DoChangeScene(SCENE_ID sceneId)
{
	// シーンを変更する
	sceneId_ = sceneId;

	// 現在のシーンを解放
	if (scene_ != nullptr)
	{
		scene_.reset();
	}

	switch (sceneId_)
	{
		case SCENE_ID::TITLE:
		{
			scene_ = std::make_unique<TitleScene>();
		}
		break;

		case SCENE_ID::GAME:
		{
			scene_ = std::make_unique<GameScene>();
		}
		break;
	}

	scene_->Initialize();

	ResetDeltaTime();

	waitSceneId_ = SCENE_ID::NONE;
}

void SceneManager::FadeScreen(void)
{
	Fader::STATE fState = fader_->GetState();

	switch (fState)
	{
		case Fader::STATE::FADE_IN:
		{
			// 明転中
			if (fader_->IsEnd())
			{
				// 明転が終了したら、フェード処理終了
				fader_->SetFade(Fader::STATE::NONE);
				isSceneChanging_ = false;
			}
		}
		break;

		case Fader::STATE::FADE_OUT:
		{
			// 暗転中
			if (fader_->IsEnd())
			{
				// 完全に暗転してからシーン遷移
				DoChangeScene(waitSceneId_);
				// 暗転から明転へ
				fader_->SetFade(Fader::STATE::FADE_IN);
			}
		}
		break;
	}
}

void SceneManager::UpdateDeltaTime(void)
{
	auto nowTime = std::chrono::system_clock::now();
	deltaTime_ = static_cast<float>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - preTime_).count() / 1000000000.0);
	preTime_ = nowTime;
}


