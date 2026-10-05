#pragma once
#include <string>

class Application
{
public:

	// スクリーンサイズ
	static constexpr int SCREEN_ASPECT = 100;/*120*/
	static constexpr int SCREEN_SIZE_X = (16 * SCREEN_ASPECT);
	static constexpr int SCREEN_SIZE_Y = (9 * SCREEN_ASPECT);

	// データパス関連
	//-------------------------------------------
	static const std::string PATH_IMAGE;
	static const std::string PATH_MODEL;
	static const std::string PATH_EFFECT;
	static const std::string PATH_SHADER;
	//-------------------------------------------

	/// @brief 明示的にインステンスを生成する
	static void CreateInstance(void);

	/// @brief 静的インスタンスの取得
	static Application& GetInstance(void);

	/// @brief リソースの破棄
	void DestroyInstance(void);

	void Initialize(void);

	void Run(void);

	/// @brief 初期化を失敗したか否か
	bool IsInitFail(void) const { return isInitializeFail_; };

	/// @brief メモリ解放を失敗したか否か
	bool IsReleaseFail(void) const { return isReleaseFail_; };


private:

	// 静的インスタンス
	static Application* instance_;

	// 初期化失敗
	bool isInitializeFail_ = false;

	// 解放失敗
	bool isReleaseFail_ = false;


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
