#include "Application.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include <string>
#include "Manager/StatusManager.h"
#include "Manager/InputManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/SceneManager.h"
#include "Common/FrameRate.h"
#include "./resource.h"

namespace
{
	const std::string GAME_TITLE = "悪逆ノ流儀";
};
Application* Application::instance_ = nullptr;

Application::Application(void)
{

}

void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
	}
	instance_->Initialize();
}

void Application::Initialize(void)
{
	// アプリケーションの初期設定
	SetWindowText(GAME_TITLE.c_str());

	// ウィンドウサイズ
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);
	
	// ウィンドウアイコン
	SetWindowIconID(IDI_ICON1);

#ifdef _DEBUG
	ChangeWindowMode(TRUE);
#else
	ChangeWindowMode(FALSE);
#endif

	// DxLibの初期化
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	isInitializeFail_ = false;
	if (DxLib_Init() == -1)
	{
		isInitializeFail_ = true;
		return;
	}

	// Effekseerの初期化
	InitEffekseer();

	// キー制御初期化
	SetUseDirectInputFlag(true);

	frameRate_ = std::make_unique<FrameRate>();

	InputManager::CreateInstance();

	// 管理マネージャ初期化
	StatusManager::CreateInstance();
	ResourceManager::CreateInstance();
	SceneManager::CreateInstance();
}

void Application::Run(void)
{
	// ゲームループ
	while (ProcessMessage() == 0 && !isGameEnd_)
	{
		if (SceneManager::GetInstance().GetSceneID() == SceneManager::SCENE_ID::TITLE
			&& CheckHitKey(KEY_INPUT_ESCAPE) != 0)
		{
			IsGameEnd();
		}

		frameRate_->Update();

		// フレームレート制限
		if (frameRate_->GetLimitFrameRate() == false)
		{
			frameRate_->SetFrameRate();

			InputManager::GetInstance().Update();
			SceneManager::GetInstance().Update();

			SceneManager::GetInstance().Draw();

#ifdef _DEBUG
			if (SceneManager::GetInstance().GetIsDebugMode())
			{
				frameRate_->Draw();

				DrawString(SCREEN_HALF_X, 0, "デバッグモード有効化中", 0xff0000);

				// データ再ロード
				if (CheckHitKey(KEY_INPUT_DELETE) &&
					SceneManager::GetInstance().GetIsDebugMode() &&
					SceneManager::GetInstance().GetSceneID() == SceneManager::SCENE_ID::TITLE)
				{
					StatusManager::GetInstance().Load();
					OutputDebugString("\nリソースデータを再ロードしました。(・ω・)b ｸﾞｯ!\n");
				}
			}
#endif
		}

		ScreenFlip();
	}
}

void Application::DestroyInstance(void)
{
	// 管理マネージャのメモリ解放
	InputManager::GetInstance().DestroyInstance();
	StatusManager::GetInstance().DestroyInstance();
	ResourceManager::GetInstance().DestroyInstance();
	SceneManager::GetInstance().DestroyInstance();

	// Effekseerを終了
	Effkseer_End();

	// DxLib終了
	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}

	delete instance_;
}

void Application::InitEffekseer(void)
{
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
	}

	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
}
