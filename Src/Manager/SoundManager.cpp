#include "SoundManager.h"
#include <DxLib.h>
#include <string>
#include <cassert>
#include "./ResourceManager.h"
#include "./Resource.h"

SoundManager* SoundManager::instance_ = nullptr;

const std::string SoundManager::PATH_BGM_TITLE = ResourceManager::PATH_BGM + "TitleBGM.mp3";
const std::string SoundManager::PATH_BGM_GAME  = ResourceManager::PATH_BGM + "GameBGM.mp3";

const std::string SoundManager::PATH_SE_CLICK  = ResourceManager::PATH_SE + "Click.mp3";
const std::string SoundManager::PATH_SE_SWORD_JUB = ResourceManager::PATH_SE + "SwordJub.mp3";
const std::string SoundManager::PATH_SE_SWORD_STRONG = ResourceManager::PATH_SE + "SwordStrong.mp3";
const std::string SoundManager::PATH_SE_KNOCK  = ResourceManager::PATH_SE + "KnockDown.mp3";

void SoundManager::CreateInstance(void)
{
	/*　インスタンス初期化処理　*/
	if (instance_ == nullptr)
	{
		instance_ = new SoundManager();
	}

	// 初期化処理
	instance_->Load();
}


void SoundManager::Load(void)
{
	// 主音量割り当て
	volumeMaster_ = VOLUME_MASTER_MAX;

	// リストクリア
	sounds_.clear();

	// 各音声割り当て
	SetSounds();
}


void SoundManager::SetSounds(void)
{
	// SE

	SetSound(SRC::SE_CLICK, PATH_SE_CLICK, Sound::TYPE::SOUND_2D, VOLUME_CLICK);
	SetSound(SRC::SE_SWORD_JUB, PATH_SE_SWORD_JUB, Sound::TYPE::SOUND_2D, VOLUME_JUB);
	SetSound(SRC::SE_SWORD_STRONG, PATH_SE_SWORD_STRONG, Sound::TYPE::SOUND_2D, VOLUME_STRONG);

	//SetSound(SRC::SE_KNOCK, PATH_SE_KNOCK, Sound::TYPE::SOUND_2D, VOLUME_KNOCK);


	//　BGM

	SetSound(SRC::BGM_TITLE, PATH_BGM_TITLE, Sound::TYPE::SOUND_2D, VOLUME_TITLE, true);
	SetSound(SRC::BGM_GAME, PATH_BGM_GAME, Sound::TYPE::SOUND_2D, VOLUME_GAME, true);

}
void SoundManager::SetSound(SoundManager::SRC _src, const std::string& path,
							Sound::TYPE type, float maxVolume, bool _isBGM)
{
	Sound res;

	res = Sound(type, path, _isBGM);
	res.SetMaxVolume(maxVolume);
	sounds_.emplace(_src, res);
}


void SoundManager::Release(void)
{
	if (sounds_.empty()) return;

	for (auto& sound : sounds_)
	{
		// 各サウンドのメモリ解放
		sound.second.Release();
	}

	// リストをクリア
	sounds_.clear();
}

void SoundManager::Destroy(void)
{
	// メモリ解放
	Release();

	// リストクリア
	sounds_.clear();

	// インスタンス
	delete instance_;
}

bool SoundManager::Play(SRC _src, bool _isLoop, bool _isPitch, float _pitchRange, bool _isForce)
{
	/* 2D音声を再生 */
	const auto& findSound = sounds_.find(_src);

	if (findSound == sounds_.end())
	{
#ifdef _DEBUG
		assert(false && "\n2D再生する音声が割り当てられていません\n(；_；)\n"); // 例外スロー
#endif
		return false;
	}


	if (!findSound->second.IsLoad())
	{
		// 読み込み処理
		findSound->second.Load();
	}

	// 2Dサウンド再生処理
	Sound::TIMES times = ((_isLoop) ? Sound::TIMES::LOOP : Sound::TIMES::ONCE);
	return findSound->second.Play(times, _isPitch, _pitchRange,_isForce);
}
bool SoundManager::Play(SRC _src, bool _isLoop, VECTOR _pos, float _radius)
{
	/* 3D音声を再生 */
	const auto& lPair = sounds_.find(_src);

	if (lPair == sounds_.end())
	{
#ifdef _DEBUG
		assert(false && "\n3D再生する音声が割り当てられていません\n(；_；)\n"); // 例外スロー
#endif
		return false;
	}


	if (!lPair->second.IsLoad())
	{
		// 読み込み処理
		lPair->second.Load();
	}

	// 3Dサウンド再生処理
	Sound::TIMES times = ((_isLoop) ? Sound::TIMES::LOOP : Sound::TIMES::ONCE);
	return lPair->second.Play(times, _pos, _radius);
}

void SoundManager::Stop(SRC _src)
{
	const auto& listPair = sounds_.find(_src);

	// 判定対象
	if (listPair == sounds_.end())
	{
#ifdef _DEBUG
		OutputDebugString("\n停止する音声が割り当てられていません\n(；_；)\n");
		assert(false); // 例外スロー
#endif
		return;
	}
	
	// 指定の音声を停止
	return listPair->second.Stop();
}

void SoundManager::StopAll(void)
{
	if (sounds_.empty()) return;

	for (auto& sound : sounds_)
	{
		if (!sound.second.IsLoad())
		{
			// 未読み込み時、スキップ
			continue;
		}

		// リスト内のサウンドを全て停止
		sound.second.Stop();
	}
}

void SoundManager::StopAllChoice(bool _isBGM)
{
	/* 指定した種類の音声を停止 */

	if (sounds_.empty()) return;

	for (auto& sound : sounds_)
	{
		if (!sound.second.IsLoad() ||
			!sound.second.IsBGM() == _isBGM)
		{
			// 未読み込み時、指定の種類以外スキップ
			continue;
		}

		// リスト内のサウンドを全て停止
		sound.second.Stop();
	}
}

void SoundManager::SetVolume(SRC _src, float per)
{
	const auto& lPair = sounds_.find(_src);

	if (lPair != sounds_.end())
	{
		// 音声があるとき、音量を割り当て
		return lPair->second.SetVolume(per);
	}

#ifdef _DEBUG
	OutputDebugString("\nサウンドが割り当てられていません\n(；_；)\n");
	assert(false); // 例外スロー
#endif
}

bool SoundManager::IsSoundStart(SRC _src)
{
	const auto& soundlist = sounds_.find(_src);

	if (soundlist == sounds_.end())
	{
#ifdef _DEBUG
		assert(false &&"\n停止するサウンドが割り当てられていません\n(；_；)\n"); // 例外スロー
#endif
		return false;
	}

	// 音声があるとき、再生か判定
	return soundlist->second.IsStart();
}

bool SoundManager::IsSoundPlay(SRC _src)
{
	const auto& soundlist = sounds_.find(_src);

	if (soundlist == sounds_.end())
	{
#ifdef _DEBUG
		assert(false &&"\n停止するサウンドが割り当てられていません\n(；_；)\n"); // 例外スロー
#endif
		return false;
	}

	// 音声があるとき、再生か判定
	return soundlist->second.IsPlay();
}

bool SoundManager::IsSoundEnd(SRC _src)
{
	const auto& soundlist = sounds_.find(_src);

	if (soundlist == sounds_.end())
	{
#ifdef _DEBUG
		assert(false &&"\n停止するサウンドが割り当てられていません\n(；_；)\n"); // 例外スロー
#endif
		return false;
	}

	// 音声があるとき、再生か判定

	return soundlist->second.IsEnd();;
}

void SoundManager::SetVolumeMaster(float volume)
{
	float vol = volume;
	
	// 音量が０未満の時、音量を０にする
	vol = ((vol < 0.0f) ? 0.0f : vol);

	// 音量が最大値を超えた時、音量を最大値にする
	vol = ((volume > VOLUME_MASTER_MAX) ? VOLUME_MASTER_MAX : vol);

	vol = static_cast<int>(volume * VOLUME_MASTER_MAX);

	// 音量割り当て
	volumeMaster_ = vol;
}