#pragma once
#include <memory>
#include <chrono>
#include "../Scene/SceneBase.h"
#include "../Common/Fader.h"
#include "./Camera.h"
#include "../Common/Performance.h"

class SceneBase;
class Fader;
class Camera;
class Performance;

class SceneManager
{
public:

	/// @brief シーン管理用
	enum class SCENE_ID
	{
		NONE = -1,
		TITLE,
		GAME
	};
	
	/// @brief インスタンス処理
	static void CreateInstance(void);
	static SceneManager& GetInstance(void);
	void DestroyInstance(void);

	void Initialize(void);
	void Init3D(void);
	void Update(void);
	void Draw(void);


	/// @brief 状態遷移
	/// @param nextId 遷移後のシーン
	void ChangeScene(SCENE_ID nextId);

	/// @brief 現在シーンID取得
	SCENE_ID GetSceneID(void) { return sceneId_; }

	/// @brief デルタタイムの取得
	float GetDeltaTime(void) const;

	/// @brief カメラの取得
	Camera& GetCamera(void) { return *camera_; }

	Performance& GetPerformance(void) { return *performance_; }

	int GetMainScreen(void) const { return mainScreen_; };

	float GetTotalTime(void) const { return totalTime_; };

	bool GetIsDebugMode(void)const { return isDebugMode_; };
	void ChangeIsDebugMode(void) { isDebugMode_ = !isDebugMode_; };


private:

	// 静的インスタンス
	static SceneManager* instance_;

	SCENE_ID sceneId_;
	SCENE_ID waitSceneId_;

	bool isDebugMode_ = false;

	// 各種シーン
	std::unique_ptr<SceneBase> scene_ = nullptr;

	// フェード
	std::unique_ptr<Fader> fader_ = nullptr;

	// 演出管理
	std::unique_ptr<Performance> performance_ = nullptr;

	// カメラ
	std::unique_ptr<Camera> camera_ = nullptr;

	// シーン遷移中判定
	bool isSceneChanging_ = false;

	// デルタタイム
	std::chrono::system_clock::time_point preTime_;
	float deltaTime_ = 0.0f;

	// ゲーム実行時間
	float totalTime_ = 0.0f;
	
	// メインスクリーン
	int mainScreen_ = -1;


	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SceneManager(void);

	// デストラクタも同様
	~SceneManager(void) = default;

	/// @brief コピーコンストラクタ対策
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

	/// @brief デルタタイムをリセットする
	void ResetDeltaTime(void);

	/// @brief シーン遷移
	void DoChangeScene(SCENE_ID sceneId);

	/// @brief フェード
	void FadeScreen(void);

	void UpdateDeltaTime(void);
};
