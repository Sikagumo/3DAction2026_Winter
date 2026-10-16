#include "Sound.h"
#include "./SoundManager.h"
#include <algorithm>

Sound::Sound(void):
	sound_(SoundInfo())
{

}
Sound::Sound(TYPE type, const std::string& path, bool _isBGM):
	sound_(SoundInfo(type, path, _isBGM))

{

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
	if (sound_.type == TYPE::NONE) return;


	// 3Dサウンド時に、3Dサウンドのフラグ割り当て
	SetCreate3DSoundFlag((sound_.type == TYPE::SOUND_3D));

	// 音声ハンドル割り当て
	sound_.handle = LoadSoundMem(sound_.path.c_str());
	
}

void Sound::Release(void)
{
	if (sound_.handle == -1) return;

	if (CheckSoundMem(sound_.handle) == 1)
	{
		// 再生時、再生中の音声を停止
		StopSoundMem(sound_.handle);
	}

	// 音声ファイルを解放
	DeleteSoundMem(sound_.handle);
}


bool Sound::Play(TIMES times, bool _isPitch, int _pitchRange, bool isForce)
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


	// 音声のピッチをランダムで変更しソースを再読み込み
	if (_isPitch)
	{
		int range = _pitchRange;
		int rand = GetRand(range + range) - (range / 2);
		SetCreateSoundPitchRate(static_cast<float>(rand) * MUSICAL_SCALE_HALF);
		sound_.handle = LoadSoundMem(sound_.path.c_str());
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


bool Sound::CheckSoundHandle(void)
{
	if (CheckSoundMem(sound_.handle) != -1)
	{
		// メモリに割り当てられていない時、false
		return false;
	}

	return true;
}

bool Sound::IsLoad(void)
{
	// ハンドルが読み込まれている時、true
	return (sound_.handle != -1);
}

void Sound::SetVolume(float per)
{
	// 音量の上限・下限制限(0.0～1.0)

	per = std::clamp(per, 0.0f, 1.0f);

	// 音量割り当て
	ChangeVolumeSoundMem((per * sound_.maxVolume), sound_.handle);

}

void Sound::SetMaxVolume(float per)
{
	// 負の値の時、正の値に変える
	float volume = ((per < 0.0f) ? -per : per);

	sound_.maxVolume = (SoundManager::GetInstance().GetVolumeMaster() * per);
}


bool Sound::IsStart(void)
{
	// サウンド未割当時、無効
	if (sound_.handle != -1) return false;

	// 前フレームがfalse,現在フレームがtrue時、true
	return (sound_.isPlay && !sound_.isPlayOld);
}

bool Sound::IsPlay(void)
{
	// サウンド未割当時、無効
	if (sound_.handle != -1) return false;

	// 前フレームと現在フレームがtrue時、true
	return (sound_.isPlay && sound_.isPlayOld);
}

bool Sound::IsEnd(void)
{
	// サウンド未割当時、無効
	if (sound_.handle != -1) return false;

	// 前フレームがtrue,現在フレームがfalse時、true
	return (!sound_.isPlay && sound_.isPlayOld);
}

bool Sound::IsBGM(void)
{
	return (sound_.isBgm && sound_.handle != -1);
}
