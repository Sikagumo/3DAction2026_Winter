#include "Sound.h"
#include "SoundManager.h"
#include <algorithm>

Sound::Sound(void)
	: sound_{}
{

}
Sound::Sound(TYPE type, int handle, bool isBGM)
	: Sound::Sound()
{
	SetSoundInfo(type, handle, isBGM);
}

void Sound::Update(VECTOR pos)
{
	if (sound_.type != TYPE::SOUND_3D ||
		CheckSoundMem(sound_.handle) == -1)
	{
		// 3Dではない、未割当時に処理終了
		return;
	}

	// 再生座標位置取得
	int soundPos = Set3DPositionSoundMem(sound_.pos, sound_.handle);
	if (soundPos == -1)
	{
		return;
	}

	// 座標割り当て
	sound_.pos = pos;
}

void Sound::Load(void)
{
	if (sound_.handle != -1) { return; }

	// 音声ハンドル割り当て
	ResourceManager::SRC soundSrc = static_cast<ResourceManager::SRC>(sound_.src);
	sound_.handle = ResourceManager::GetInstance().LoadHandleId(soundSrc);

	// ピッチ変更の基準
	sound_.baseFrequency = GetFrequencySoundMem(sound_.handle);
}

void Sound::Release(void)
{
	if (sound_.handle == -1) { return; }

	// 再生時、再生中の音声を停止
	StopSoundMem(sound_.handle);

	// ハンドルを未割当化
	sound_.handle = -1;
}


bool Sound::Play(TIMES times, bool isPitch, int pitchRange, bool isForce)
{
	sound_.isPlayOld = sound_.isPlay;

	if (sound_.type != TYPE::SOUND_2D ||
		sound_.handle == -1)
	{
		sound_.isPlay = false;

		// 2Dサウンドではない、未割当時にfalse
		return false;
	}

	if (!isForce && IsPlay())
	{
		sound_.isPlay = false;

		// 再生中は処理終了
		return false;
	}

	// 再生種類が1回再生時、バック再生状態
	int type = ((times == TIMES::ONCE) ?
		DX_PLAYTYPE_BACK :
		DX_PLAYTYPE_LOOP);

	if (isPitch)
	{
		// 半音の幅によって音声のピッチをランダムで変更
		float semitone = (GetRand(2000) / 1000.0f - 1.0f) * pitchRange;
		SetFrequencySoundMem(static_cast<int>(sound_.baseFrequency * std::exp2(semitone / 12.0f)),
			sound_.handle);
	}

	// サウンド割り当て判定
	int id = PlaySoundMem(sound_.handle, type, true);

	// 音量割り当て
	SetVolume();

	sound_.isPlay = true;

	// 再生指定したサウンドの未割り当て時 false
	return ((id == -1) ? false : true);
}

bool Sound::Play(TIMES times, VECTOR pos, float radius)
{
	// 再生しているか否か
	sound_.isPlayOld = sound_.isPlay;


	if (sound_.type != TYPE::SOUND_3D ||
		sound_.handle == -1)
	{
		sound_.isPlay = false;;

		// 3Dサウンドではない、未割当時にfalse
		return false;
	}

	if (IsPlay())
	{
		sound_.isPlay = false;
		// 再生中は処理終了
		return false;
	}

	// 音声がメモリに割り当てられているか判定
	int soundPos = Set3DPositionSoundMem(pos, sound_.handle);
	if (soundPos == -1)
	{
		sound_.isPlay = false;
		return false;
	}

	// 音量が有効な範囲を割り当てられているか判定
	int rad = Set3DRadiusSoundMem(radius, sound_.handle);
	if (rad == -1)
	{
		sound_.isPlay = false;
		return false;
	}


	// 再生種類が1回再生時、バック再生状態
	int type = ((times == TIMES::ONCE) ? DX_PLAYTYPE_BACK : DX_PLAYTYPE_LOOP);

	// サウンド割り当ているか判定
	int id = PlaySoundMem(sound_.handle, type, true);
	if (id == -1)
	{
		sound_.isPlay = false;
		return false;
	}


	// 再生する座標を割り当て
	sound_.pos = pos;

	sound_.radius = radius;

	// 再生中
	sound_.isPlay = true;

	return true;
}

void Sound::Stop(void)
{
	if (CheckSoundMem(sound_.handle) == -1)
	{
		// 音声ハンドル未割当時、処理終了
		return;
	}


	int mem = StopSoundMem(sound_.handle);
	if (mem == -1)
	{
		// メモリ未割当時、処理終了
		return;
	}

	// 座標初期化
	sound_.pos = {};

	// 半径初期化
	sound_.radius = 0.0f;

	sound_.isPlay = false;
}


bool Sound::CheckSoundHandle(void) const
{
	if (CheckSoundMem(sound_.handle) != -1)
	{
		// メモリに割り当てられていない時、false
		return false;
	}

	return true;
}

bool Sound::IsLoad(void)const
{
	// ハンドルが読み込まれている時、true
	return (sound_.handle != -1);
}

void Sound::SetVolume(float per)const
{
	// 音量の上限・下限制限(0.0～1.0)

	per = std::clamp(per, 0.0f, 1.0f);

	// 音量割り当て
	ChangeVolumeSoundMem(static_cast<int>(per * sound_.maxVolume), sound_.handle);

}

void Sound::SetMaxVolume(float per)
{
	// 負の値の時、正の値に変える
	float volume = ((per < 0.0f) ? -per : per);

	sound_.maxVolume = static_cast<int>(SoundManager::GetInstance().GetVolumeMaster() * per);
}


bool Sound::IsStart(void)const
{
	// サウンド未割当時、無効
	if (sound_.handle != -1) return false;

	// 前フレームがfalse,現在フレームがtrue時、true
	return (sound_.isPlay && !sound_.isPlayOld);
}

bool Sound::IsPlay(void)const
{
	// サウンド未割当時、無効
	if (sound_.handle != -1) return false;

	// 前フレームと現在フレームがtrue時、true
	return (sound_.isPlay && sound_.isPlayOld);
}

bool Sound::IsEnd(void)const
{
	// サウンド未割当時、無効
	if (sound_.handle != -1) return false;

	// 前フレームがtrue,現在フレームがfalse時、true
	return (!sound_.isPlay && sound_.isPlayOld);
}

bool Sound::IsBGM(void)const
{
	return (sound_.isBgm && sound_.handle != -1);
}

void Sound::SetSoundInfo(TYPE type, int src, bool isBgm)
{
	sound_.type = type;
	sound_.src = src;
	sound_.handle = -1;
	sound_.isBgm = isBgm;
}