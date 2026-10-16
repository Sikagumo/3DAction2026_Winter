#pragma once
#include <map>
#include <string>
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
		SE_SWORD_JUB, // 弱攻撃
		SE_SWORD_STRONG, // 強攻撃
		SE_SWORD_SPECIAL, // 必殺攻撃
		SE_KNOCK,  // 撃破
	};


	// マスターボリューム
	static constexpr float VOLUME_MASTER_MAX = 255;

	static constexpr float VOLUME_MASTER_NUM = (VOLUME_MASTER_MAX / 100);


	// タイトルBGM音量
	static constexpr float VOLUME_TITLE = 0.85f;

	// ゲームシーンBGM音量
	static constexpr float VOLUME_GAME = 0.9f;


	// クリックSEの音量
	static constexpr float VOLUME_CLICK = 0.9f;

	// 弱攻撃SEの音量
	static constexpr float VOLUME_JUB = 1.0f;

	// 強攻撃SEの音量
	static constexpr float VOLUME_STRONG = 0.95f;

	// 撃破SEの音量
	static constexpr float VOLUME_KNOCK = 1.5f;

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
	void Destroy(void);

	/// @brief 2Dサウンド再生処理
	/// @param _src 再生するサウンドの種類
	/// @param _isLoop ループするか否か
	/// @param _isPitch ピッチを変えるか否か
	/// @param _pitchRange ピッチ増減の範囲
	/// @param _isForce 
	bool Play(SRC _src, bool _isLoop, bool _isPitch = false, float _pitchRange = PITCH_RANGE_DEF, bool _isForce = false);
	/// @brief 2Dサウンド再生処理
	/// @param _src 再生するサウンドの種類
	/// @param _isLoop ループするか否か
	/// @param _isForce 
	bool Play(int _src, bool _isLoop, bool _isPitch = false, float _pitchRange = PITCH_RANGE_DEF, bool _isForce = false)
		{ Play(static_cast<SRC>(_src), _isLoop, _isPitch, _pitchRange, _isForce); };

	/// <summary>
	/// 3Dサウンド再生処理
	/// </summary>
	/// <param name="_sec">再生するサウンドの種類</param>
	/// <param name="_isLoop">ループするか否か</param>
	/// <param name="_pos">音量が出てくる座標</param>
	/// <param name="_radius">音量の聞こえる半径</param>
	/// <returns></returns>
	bool Play(SRC _sec, bool _isLoop, VECTOR _pos, float _radius);


	/// @brief サウンドが再生開始しているか判定
	/// @param _src 再生するサウンドの種類
	bool IsSoundStart(SRC _src);

	/// <summary>
	/// サウンドが再生しているか判定
	/// </summary>
	/// <param name="_src">再生するサウンドの種類</param>
	bool IsSoundPlay(SRC _src);

	/// <summary>
	/// サウンドが停止しているか判定
	/// </summary>
	/// <param name="_src">再生するサウンドの種類</param>
	bool IsSoundEnd(SRC _src);

	/// <summary>
	/// 指定の音声を停止
	/// </summary>
	void Stop(SRC _src);

	/// @brief 全音声を停止
	void StopAll(void);

	/// @brief 指定の種類の音声を停止
	/// @param _isBGM (true:BGMを停止, false:SEを停止)
	void StopAllChoice(bool _isBGM);

	/// <summary>
	/// 音量を割り当て
	/// </summary>
	/// <param name="_src">割り当てるサウンド</param>
	/// <param name="per">音量の倍率(0.0～1.0)</param>
	void SetVolume(SRC _src, float per = 1.0f);

	/// <summary>
	/// 主音量割り当て
	/// </summary>
	void SetVolumeMaster(float master = 255.0f);

	/// <summary>
	/// 主音量取得
	/// </summary>
	float GetVolumeMaster(void) { return volumeMaster_; };



private:

	// タイトルBGM
	static const std::string PATH_BGM_TITLE;

	// ゲームシーンBGM
	static const std::string PATH_BGM_GAME;


	// クリックSE
	static const std::string PATH_SE_CLICK;

	// 攻撃SE
	static const std::string PATH_SE_SWORD_JUB;

	// 撃破SE
	static const std::string PATH_SE_SWORD_STRONG;

	// 撃破SE
	static const std::string PATH_SE_KNOCK;


	// 静的インスタンス
	static SoundManager* instance_;

	std::map<SRC, Sound> sounds_;

	// 主音量
	float volumeMaster_;


	/// @brief デフォルトコントラクタ
	SoundManager(void) = default;

	/// @brief デフォルトデストラクタ
	~SoundManager(void) = default;

	/// @brief コピーコンストラクタ対策
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;
	SoundManager(SoundManager&&) = delete;
	SoundManager& operator=(SoundManager&&) = delete;

	/// <summary>
	/// 各音声割り当て処理
	/// </summary>
	void SetSounds(void);

	/// <summary>
	/// 音声割り当て処理
	/// </summary>
	/// <param name="_src">種類</param>
	/// <param name="path">パス</param>
	/// <param name="type">再生タイプ</param>
	/// <param name="maxVolume">最大音量</param>
	void SetSound(SRC _src, const std::string& path, Sound::TYPE type, float maxVolume, bool _isBGM = false);
};