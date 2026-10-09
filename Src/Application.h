#pragma once
#include <string>

class Application
{
public:

	// スクリーンサイズ
	static constexpr int SCREEN_ASPECT = (120 - 20);
	static constexpr int SCREEN_SIZE_X = (16 * SCREEN_ASPECT);
	static constexpr int SCREEN_SIZE_Y = (9 * SCREEN_ASPECT);
	static constexpr int SCREEN_HALF_X = (SCREEN_SIZE_X / 2);
	static constexpr int SCREEN_HALF_Y = (SCREEN_SIZE_Y / 2);

	// 重力最大値(default:9.8f)
	static constexpr float GRAVITY_MAX = 9.8f;

	// 重力増加値(default:0.25f)
	static constexpr float GRAVITY_ACC = 0.25f;


	/// @brief インスタンス処理
	static void CreateInstance(void);
	static Application& GetInstance(void) { return *instance_; };
	void DestroyInstance(void);

	void Initialize(void);

	void Run(void);

	/// @brief 初期化を失敗したか否か
	bool IsInitFail(void) const { return isInitializeFail_; }

	/// @brief メモリ解放を失敗したか否か
	bool IsReleaseFail(void) const { return isReleaseFail_; }

	/// @brief ゲーム終了処理
	void IsGameEnd(void) { isGameEnd_ = true; }


private:

	// 静的インスタンス
	static Application* instance_;

	// 初期化失敗
	bool isInitializeFail_ = false;

	// 解放失敗
	bool isReleaseFail_ = false;

	// ゲームを終了するか否か
	bool isGameEnd_ = false;


	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	Application(void);
	~Application(void) = default;
 
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator=(Application&&) = delete;

	/// @brief Effekseerの初期化
	void InitEffekseer(void);
};
