#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "Manager/InputManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/SceneManager.h"
#include "Application.h"

namespace
{

};
Application* Application::instance_ = nullptr;

const std::string Application::PATH_IMAGE = "Data/Image/";
const std::string Application::PATH_MODEL = "Data/Model/";
const std::string Application::PATH_EFFECT = "Data/Effect/";
const std::string Application::PATH_SHADER = "Data/Shader/";

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

Application& Application::GetInstance(void)
{
	return *instance_;
}

void Application::Initialize(void)
{
	// アプリケーションの初期設定
	SetWindowText("");

	// ウィンドウサイズ
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);
	
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
	InputManager::CreateInstance();

	// 管理マネージャ初期化
	ResourceManager::CreateInstance();
	SceneManager::CreateInstance();
}

void Application::Run(void)
{
	// ゲームループ
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		InputManager::GetInstance().Update();
		SceneManager::GetInstance().Update();

		SceneManager::GetInstance().Draw();

		ScreenFlip();
	}
}

void Application::DestroyInstance(void)
{
	// 管理マネージャのメモリ解放
	InputManager::GetInstance().DestroyInstance();
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
