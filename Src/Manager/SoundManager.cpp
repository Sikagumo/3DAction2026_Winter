#include "ResourceManager.h"
#include "Resource.h"
#include <DxLib.h>
#include <string>
#include <cassert>
#include <memory>
#include "SoundManager.h"

SoundManager* SoundManager::instance_ = nullptr;

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
	using RESOURCE = ResourceManager::SRC;

	// SE
	SetSound(SRC::SE_CLICK, static_cast<int>(RESOURCE::SE_SELECT)
		, Sound::TYPE::SOUND_2D, VOLUME_CLICK);

	SetSound(SRC::SE_SWORD_LIGHT, static_cast<int>(RESOURCE::SE_SLASH_LIGHT)
		, Sound::TYPE::SOUND_2D, VOLUME_LIGHT);

	SetSound(SRC::SE_PARRY, static_cast<int>(RESOURCE::SE_PARRY)
		, Sound::TYPE::SOUND_2D, VOLUME_PARRY);

	//　BGM
	SetSound(SRC::BGM_TITLE, static_cast<int>(RESOURCE::BGM_TITLE)
		, Sound::TYPE::SOUND_2D, VOLUME_TITLE, true);

	SetSound(SRC::BGM_GAME, static_cast<int>(RESOURCE::BGM_GAME)
		, Sound::TYPE::SOUND_2D, VOLUME_GAME, true);

}
void SoundManager::SetSound(SoundManager::SRC src, int resource,
							Sound::TYPE type, float maxVolume, bool isBGM)
{
	std::unique_ptr<Sound> res = std::make_unique<Sound>(type, resource, isBGM);

	res->SetMaxVolume(maxVolume);
	sounds_.emplace(src, std::move(res));
}


void SoundManager::Release(void)
{
	if (sounds_.empty()) { return; }

	for (auto& sound : sounds_)
	{
		// 各サウンドのメモリ解放
		sound.second->Release();
	}

	// リストをクリア
	sounds_.clear();
}

void SoundManager::DestroyInstance(void)
{
	// メモリ解放
	Release();

	// リストクリア
	sounds_.clear();

	// インスタンス
	delete instance_;
}

bool SoundManager::Play(SRC src, bool isLoop, bool isPitch, float pitchRange, bool isForce)
{
	/* 2D音声を再生 */
	const auto& findSound = sounds_.find(src);

	if (findSound == sounds_.end())
	{
#ifdef _DEBUG
		assert(false && "\n2D再生する音声が割り当てられていません\n(；_；)\n"); // 例外スロー
#endif
		return false;
	}


	if (!findSound->second->IsLoad())
	{
		// 読み込み処理
		findSound->second->Load();
	}

	// 2Dサウンド再生処理
	Sound::TIMES times = ((isLoop) ? Sound::TIMES::LOOP : Sound::TIMES::ONCE);
	return findSound->second->Play(times, isPitch, static_cast<int>(pitchRange * 255.0f),isForce);
}
bool SoundManager::Play(SRC src, bool isLoop, VECTOR _pos, float radius)
{
	/* 3D音声を再生 */
	const auto& lPair = sounds_.find(src);

	if (lPair == sounds_.end())
	{
#ifdef _DEBUG
		assert(false && "\n3D再生する音声が割り当てられていません\n(；_；)\n"); // 例外スロー
#endif
		return false;
	}


	if (!lPair->second->IsLoad())
	{
		// 読み込み処理
		lPair->second->Load();
	}

	// 3Dサウンド再生処理
	Sound::TIMES times = ((isLoop) ? Sound::TIMES::LOOP : Sound::TIMES::ONCE);
	return lPair->second->Play(times, _pos, radius);
}

void SoundManager::Stop(SRC src)
{
	const auto& listPair = sounds_.find(src);

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
	return listPair->second->Stop();
}

void SoundManager::StopAll(void)
{
	if (sounds_.empty()) return;

	for (auto& sound : sounds_)
	{
		if (!sound.second->IsLoad())
		{
			// 未読み込み時、スキップ
			continue;
		}

		// リスト内のサウンドを全て停止
		sound.second->Stop();
	}
}

void SoundManager::StopAllChoice(bool isBGM)
{
	/* 指定した種類の音声を停止 */

	if (sounds_.empty()) return;

	for (auto& sound : sounds_)
	{
		if (!sound.second->IsLoad() ||
			!sound.second->IsBGM() == isBGM)
		{
			// 未読み込み時、指定の種類以外スキップ
			continue;
		}

		// リスト内のサウンドを全て停止
		sound.second->Stop();
	}
}

void SoundManager::SetVolume(SRC src, float per)
{
	const auto& lPair = sounds_.find(src);

	if (lPair != sounds_.end())
	{
		// 音声があるとき、音量を割り当て
		return lPair->second->SetVolume(per);
	}

#ifdef _DEBUG
	OutputDebugString("\nサウンドが割り当てられていません\n(；_；)\n");
	assert(false); // 例外スロー
#endif
}

bool SoundManager::IsSoundStart(SRC src)
{
	const auto& soundList = sounds_.find(src);

	if (soundList == sounds_.end())
	{
#ifdef _DEBUG
		assert(false &&"\n停止するサウンドが割り当てられていません\n(；_；)\n"); // 例外スロー
#endif
		return false;
	}

	// 音声があるとき、再生か判定
	return soundList->second->IsStart();
}

bool SoundManager::IsSoundPlay(SRC src)
{
	const auto& soundList = sounds_.find(src);

	if (soundList == sounds_.end())
	{
#ifdef _DEBUG
		assert(false &&"\n停止するサウンドが割り当てられていません\n(；_；)\n"); // 例外スロー
#endif
		return false;
	}

	// 音声があるとき、再生か判定
	return soundList->second->IsPlay();
}

bool SoundManager::IsSoundEnd(SRC src)
{
	const auto& soundList = sounds_.find(src);

	if (soundList == sounds_.end())
	{
#ifdef _DEBUG
		assert(false &&"\n停止するサウンドが割り当てられていません\n(；_；)\n"); // 例外スロー
#endif
		return false;
	}

	// 音声があるとき、再生か判定

	return soundList->second->IsEnd();;
}

void SoundManager::SetVolumeMaster(float volume)
{
	float vol = volume;
	
	// 音量が０未満の時、音量を０にする
	vol = ((vol < 0.0f) ? 0.0f : vol);

	// 音量が最大値を超えた時、音量を最大値にする
	vol = ((volume > VOLUME_MASTER_MAX) ? VOLUME_MASTER_MAX : vol);

	vol = (volume * VOLUME_MASTER_MAX);

	// 音量割り当て
	volumeMaster_ = vol;
}