#pragma once
#include <string>
#include <DxLib.h>

class Sound
{
public:

	enum class TYPE
	{
		NONE = -1,
		SOUND_2D, // 2D音響
		SOUND_3D, // 3D音響
	};

	enum class TIMES
	{
		NONE = -1,
		ONCE, // １回
		LOOP, // 繰り返し
	};


	struct SoundInfo
	{
		// 再生する種類
		TYPE type;

		// リソース
		std::string path;

		// ハンドルID
		int handle;

		// 最大音声
		int maxVolume;

		// 再生位置(3D用)
		VECTOR pos;

		// 有効範囲(3D用)
		float radius;

		// 再生しているか否か
		bool isPlay;

		// 再生しているか否か
		bool isPlayOld;

		// 再生しているか否か
		bool isBgm;

		SoundInfo(void)
		{
			type = TYPE::NONE;
			handle = -1;
			path = "";
			maxVolume = 255;
			pos = VGet(0.0f, 0.0f, 0.0f);
			radius = 0.0f;
			isPlay = false;
			isPlayOld = false;
			isBgm = false;
		};

		SoundInfo(TYPE _type, const std::string& _path, bool _isBGM)
		{
			type = _type;
			handle = -1;
			path = _path;
			maxVolume = 255;
			pos = VGet(0.0f, 0.0f, 0.0f);
			radius = 0.0f;
			isPlay = false;
			isPlayOld = false;
			isBgm = _isBGM;
		};
	};


	Sound(void);

	Sound(TYPE type, const std::string& path, bool _isBGM);

	~Sound(void) = default;


	/// @brief 3D座標更新処理
	/// @param pos
	void Update(VECTOR pos);

	/// @brief 読み込み処理
	void Load(void);

	/// @brief 解放処理
	void Release(void);


	/// @brief 2Dサウンドの再生処
	/// @param _times 再生種類を取得
	/// @param _isPitch ピッチを上げるか否か
	/// @param _pitchRange ピッチの増加範囲
	/// @param isForce
	/// @returns 音声未割当時、false
	bool Play(TIMES times, bool _isPitch = false, int _pitchRange = 1, bool isForce = false);

	/// @brief 3Dサウンドの再生処
	/// @param times 再生種類を取得
	/// @param pos 音声が出てくる座標
	/// @param radius 音声が聞こえる範囲
	bool Play(TIMES times, VECTOR pos, float radius);

	/// @brief 停止処理
	void Stop(void);

	/// @brief サウンドがあるか判定
	bool CheckSoundHandle(void);

	/// @brief 音声が読み込まれているか否か
	bool IsLoad(void);


	/// @brief 現在の音量を割り当て
	/// @param per 音量
	void SetVolume(float _volume = 1.0f);

	/// @brief 最大音量を割り当て
	/// @param _volume 最大音量
	void SetMaxVolume(float _volume = 1.0f);


	/// @brief 再生を開始したか判定
	bool IsStart(void);

	/// @brief 再生を開始したか判定
	bool IsPlay(void);

	/// @brief 再生が終了したか判定
	bool IsEnd(void);

	/// @brief BGMか判定
	bool IsBGM(void);


private:

	// 半音階(1オクターブ=1200.0f)
	static constexpr float MUSICAL_SCALE_HALF = (100.0f / 2.0f);

	SoundInfo sound_;
};