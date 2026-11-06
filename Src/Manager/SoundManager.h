#pragma once
#include <map>
#include <string>
#include <memory>
#include "Sound.h"
#include "ResourceManager.h"

class SoundManager
{
public:

	enum class SRC
	{
		NONE = -1,

		BGM_TITLE,
		BGM_GAME,
		BGM_CLEAR,
		BGM_GAMEOVER,

		SE_CLICK,  // クリック
		SE_SWORD_LIGHT, // 弱攻撃
		SE_SWORD_HEAVY, // 強攻撃
		SE_SWORD_SPECIAL, // 必殺攻撃
		SE_PARRY,  // パリィ成功
	};

	// マスターボリューム
	static constexpr float VOLUME_MASTER_MAX = 255;

	static constexpr float VOLUME_MASTER_NUM = (VOLUME_MASTER_MAX / 100);

	// BGM音量
	static constexpr float VOLUME_TITLE = 0.85f; // タイトル
	static constexpr float VOLUME_GAME  = 0.9f;  // ゲームシーン

	// 効果音音量
	static constexpr float VOLUME_CLICK = 0.9f;  // クリック
	static constexpr float VOLUME_LIGHT = 1.0f;  // 弱攻撃
	static constexpr float VOLUME_HEAVY = 0.95f; // 強攻撃
	static constexpr float VOLUME_PARRY = 1.0f;  // パリィ

	// ピッチの増減値
	static constexpr float PITCH_RANGE_DEF = 1.0f;


	/// @brief インスタンス生成
	static void CreateInstance(void);

	/// @brief インスタンス取得処理
	static SoundManager& GetInstance(void) { return *instance_; };

	/// @brief 初期化処理
	void Load(void);

	/// @brief メモリ解放処理
	void Release(void);

	/// @brief リソース完全破棄処理
	void DestroyInstance(void);

	/// @brief 2Dサウンド再生処理
	/// @param src 再生するサウンドの種類
	/// @param isLoop ループするか否か
	/// @param isPitch ピッチを変えるか否か
	/// @param pitchRange ピッチ増減の範囲
	/// @param isForce 
	bool Play(SRC src, bool isLoop, bool isPitch = false, float pitchRange = PITCH_RANGE_DEF, bool isForce = false);

	/// @brief 2Dサウンド再生処理
	/// @param src 再生するサウンドの種類
	/// @param isLoop ループするか否か
	/// @param isForce 
	bool Play(int src, bool isLoop, bool isPitch = false, float pitchRange = PITCH_RANGE_DEF, bool isForce = false)
		{ Play(static_cast<SRC>(src), isLoop, isPitch, pitchRange, isForce); };

	/// @brief 3Dサウンド再生処理
	/// @param src 再生するサウンドの種類
	/// @param isLoop ループするか否か
	/// @param _pos 音量が出てくる座標
	/// @param radius 音量の聞こえる半径
	bool Play(SRC src, bool isLoop, VECTOR _pos, float radius);

	/// @brief サウンドが再生開始しているか判定
	/// @param src 再生するサウンドの種類
	bool IsSoundStart(SRC src);

	/// @brief サウンドが再生しているか判定
	/// @param src 再生するサウンドの種類
	bool IsSoundPlay(SRC src);

	/// @brief サウンドが停止しているか判定
	/// @param src 再生するサウンドの種類
	bool IsSoundEnd(SRC src);

	/// @brief 指定の音声を停止
	void Stop(SRC src);

	/// @brief 全音声を停止
	void StopAll(void);

	/// @brief 指定の種類の音声を停止
	/// @param isBGM (true:BGMを停止, false:SEを停止)
	void StopAllChoice(bool isBGM);

	/// @brief 音量を割り当て
	/// @param src 割り当てるサウンド
	/// @param per 音量の倍率(0.0～1.0)
	void SetVolume(SRC src, float per = 1.0f);

	/// @brief 主音量割り当て
	void SetVolumeMaster(float master = 255.0f);

	/// @brief 主音量取得
	float GetVolumeMaster(void) { return volumeMaster_; };


private:

	// 静的インスタンス
	static SoundManager* instance_;

	std::map<SRC, std::unique_ptr<Sound>> sounds_;

	// 主音量
	float volumeMaster_ = 255.0f;

	/// @brief デフォルトコントラクタ
	SoundManager(void) = default;

	/// @brief デフォルトデストラクタ
	~SoundManager(void) = default;

	/// @brief コピーコンストラクタ対策
	SoundManager(const SoundManager&)            = delete;
	SoundManager& operator=(const SoundManager&) = delete;
	SoundManager(SoundManager&&)            = delete;
	SoundManager& operator=(SoundManager&&) = delete;

	/// @brief 各音声割り当て処理
	void SetSounds(void);

	/// @brief 音声割り当て処理
	/// @param src 種類
	/// @param resource リソース
	/// @param type 再生タイプ
	/// @param maxVolume 最大音量
	void SetSound(SRC src, int resource, Sound::TYPE type, float maxVolume, bool isBGM = false);
};