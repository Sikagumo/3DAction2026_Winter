#include "StatusManager.h"
#include <DxLib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <array>
#include <string>
#include <memory>
#include <cassert>
#include <map>
#include "../Object/Status/StatusPlayer.h"
#include "../Object/Status/StatusEnemy.h"
#include "../Object/Status/StatusBoss.h"
#include "../Object/Status/BinaryRender.h"
#include "ResourceManager.h"

namespace
{
	constexpr size_t BINARY_FILE_SIZE = 4;
	struct StatusDatHeader
	{
		char signature[4]; // "stdt"
		const float version = 1.0f; // ファイルのバージョン
		uint32_t dataNum;  // セクション数
	};
	static_assert(sizeof(StatusDatHeader) == 12);
}


StatusManager* StatusManager::instance_ = nullptr;

void StatusManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new StatusManager();
	}
	// マネージャ読み込み処理
	instance_->Load();
}

StatusManager& StatusManager::GetInstance(void)
{
	if (instance_ == nullptr)
	{
		CreateInstance();
	}
	return *instance_;
}

void StatusManager::DestroyInstance(void)
{
	if (instance_ != nullptr)
	{
		delete instance_;
	}
}

StatusManager::StatusManager(void)
{
}

void StatusManager::Load(void)
{
	/* 初回読み込み処理 */
	reader_ = std::make_unique<BinaryReader>();

	if (!ReadBinaryPlayerStatus(*reader_, *player_))
	{
		LoadPlayerStatus();
	}

	if (!ReadBinaryPlayerAction(*reader_, *player_))
	{
		LoadPlayerAction();
	}

	if (!ReadBinaryEnemyData(*reader_, enemy_))
	{
		LoadEnemy();
	}

	if (!ReadBinaryBossAction(*reader_, *boss_))
	{
		LoadBossAction();
	}
}

std::string StatusManager::ReadCsvFile(const std::string& path)
{
	std::string content;

#ifdef _DEBUG
	// デバッグビルド：通常のファイルシステムから読み込み
	std::ifstream file(path);

	if (!file.is_open())
	{
		std::string error = "\ncsvファイルが読み込まれませんでした。\nファイルパス:";
		error += path;
		OutputDebugString(error.c_str());
		assert(false);
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	content = buffer.str();
	file.close();

#else
	// リリースビルド：DXアーカイブから読み込み
	int fileHandle = FileRead_open(path.c_str());

	if (fileHandle == 0)
	{
		// 読み込み失敗
		std::string error = "\ncsvファイルが読み込まれませんでした。\nファイルパス:";
		error += path;
		error += " ";
		OutputDebugString(error.c_str());
		assert(false);
		return "";
	}

	// バッファ確保
	int fileSize = FileRead_size(path.c_str());
	char* buffer = new char[fileSize + 1];

	FileRead_read(buffer, fileSize, fileHandle);
	buffer[fileSize] = '\0';
	FileRead_close(fileHandle);

	content = std::string(buffer);
	delete[] buffer;
#endif
	return content;
}

StatusManager::CsvTable StatusManager::ReadCsvTable(const std::string& fileName, size_t columnCount)
{
	std::istringstream fileStream(ReadCsvFile(ResourceManager::PATH_CSV + fileName));

	CsvTable table;
	std::string line;
	bool isHeader = true;

	while (std::getline(fileStream, line))
	{
		if (isHeader)
		{
			// ラベル行はスキップ
			isHeader = false;
			continue;
		}

		// 改行コードがCRLFの場合の対策、空行は無視
		if (!line.empty() && line.back() == '\r') { line.pop_back(); }
		if (line.empty()) { continue; }

		std::istringstream ss(line);
		std::string cell;
		CsvRow row;

		// カンマごとにデータを格納
		while (row.size() < columnCount && std::getline(ss, cell, ','))
		{
			row.push_back(cell);
		}
		table.push_back(std::move(row));
	}
	return table;
}

bool StatusManager::OpenDat(const std::string& path, uint32_t expectedSections, BinaryReader& reader)
{
	const int HANDLE = FileRead_open(path.c_str());
	if (HANDLE <= 0) { return false; }

	auto SIZE = FileRead_size(path.c_str());
	if (SIZE <= 0)
	{
		FileRead_close(HANDLE);
		return false;
	}

	std::vector<char> buffer(SIZE);
	FileRead_read(buffer.data(), static_cast<int>(SIZE), HANDLE);
	FileRead_close(HANDLE);

	reader = BinaryReader(std::move(buffer));

	// ヘッダ(書き出し側はstruct丸ごとだが、パディングなしの12byteなので個別に読めば一致する)
	char signature[4]{};
	float version = 0.0f;
	uint32_t dataNum = 0;
	if (!reader.Read(signature)
		|| !reader.Read(version)
		|| !reader.Read(dataNum)) { return false; }

	return std::memcmp(signature, "stdt", 4) == 0
		&& version == 1.0f
		&& dataNum == expectedSections;
}

void StatusManager::LoadPlayerStatus(void)
{
	constexpr size_t COLS_MAX = static_cast<size_t>(StatusPlayer::PARAM::MAX);

	player_ = std::make_unique<StatusPlayer>();
	LoadCsv<COLS_MAX>(PATH_PLAYER, [this](size_t, const auto& row)
		{
			player_->LoadStatusParam(row);
		}
	);
}

void StatusManager::LoadPlayerAction(void)
{
	constexpr size_t COLS_MAX = static_cast<size_t>(StatusPlayer::ACTION_PARAM::MAX);
	constexpr size_t TYPES = static_cast<size_t>(StatusPlayer::ACTION_TYPE::MAX);

	LoadCsv<COLS_MAX>(PATH_PLAYER_ACTION, [this](size_t i, const auto& row)
		{
			// 行が多すぎる場合の範囲外対策
			if (i >= TYPES) { return; }
			
			player_->LoadActionParam(static_cast<StatusPlayer::ACTION_TYPE>(i), row);
		}
	);
}

void StatusManager::LoadEnemy(void)
{
	constexpr size_t COLS_MAX = static_cast<size_t>(StatusBoss::PARAM::MAX);
	constexpr size_t TYPES = static_cast<size_t>(StatusBoss::ENEMY_TYPE::MAX);

	LoadCsv<COLS_MAX>(PATH_ENEMY, [this](size_t i, const auto& row)
		{
			// 行が多すぎる場合の範囲外対策
			if (i >= TYPES) { return; }

			const int ENEMY_TYPE = static_cast<int>(i);
			enemy_[ENEMY_TYPE] = std::make_unique<StatusBoss>();
			enemy_[ENEMY_TYPE]->LoadStatusParam(row);
		}
	);
}

void StatusManager::LoadBossAction(void)
{
	constexpr size_t COLS_MAX = static_cast<size_t>(StatusBoss::ACTION_PARAM::MAX);
	constexpr size_t TYPES = static_cast<size_t>(StatusBoss::ACTION_TYPE::MAX);

	boss_ = std::make_unique<StatusBoss>();
	LoadCsv<COLS_MAX>(PATH_BOSS_ACTION, [this](size_t i, const auto& row)
		{
			// 行が多すぎる場合の範囲外対策
			if (i >= TYPES) { return; }

			boss_->LoadActionParam(static_cast<StatusBoss::ACTION_TYPE>(i), row);
		}
	);
}

bool StatusManager::ReadBinaryPlayerStatus(BinaryReader& reader, StatusPlayer& player)
{
	StatusPlayer::PlayerParam param{};
	StatusPlayer::AnimationSpeed animationSpeed{};

	// 書き出し(WritePlayerStatus)と同じ順番
	const bool IS_OK = (
		reader.ReadString(param.handlePath) &&
		reader.Read(param.scale) &&
		reader.Read(param.scaleSword) &&
		reader.Read(param.radius) &&
		reader.Read(param.hp) &&
		reader.Read(param.stamina) &&
		reader.Read(param.powerLight) &&
		reader.Read(param.powerHeavy) &&
		reader.Read(param.speedInit) &&
		reader.Read(param.speedAccTime) &&
		reader.Read(param.speedWalk) &&
		reader.Read(param.speedDash) &&
		reader.Read(param.dashSpeedMultiple) &&
		reader.Read(param.timeEvasion) &&
		reader.Read(param.timeInv) &&
		reader.Read(animationSpeed.idle) &&
		reader.Read(animationSpeed.walk) &&
		reader.Read(animationSpeed.run)
		);
	if (!IS_OK) { return false; }

	
	player.SetPlayerParam(param);
	player.SetAnimationSpeed(animationSpeed);
	return true;
}

bool StatusManager::ReadBinaryPlayerAction(BinaryReader& reader, StatusPlayer& player)
{
	uint32_t count = 0;
	if (!reader.Read(count)) { return false; }

	for (uint32_t i = 0; i < count; i++)
	{
		int32_t type = 0;
		StatusPlayer::ActionParam a{};
		const bool IS_OK =
			reader.Read(type) &&
			reader.ReadString(a.actionName) &&
			reader.Read(a.timeActive) &&
			reader.Read(a.timeInput) &&
			reader.Read(a.timeEnd) &&
			reader.Read(a.timeHitStop) &&
			reader.Read(a.timeTimingActive) &&
			reader.Read(a.timeTimingHitStop) &&
			reader.Read(a.animationSpeed);
		if (!IS_OK) { return false; }

		player_->SetActionParam(static_cast<StatusPlayer::ACTION_TYPE>(type), a);
	}
	return true;
}

bool StatusManager::ReadBinaryEnemyData(BinaryReader& reader, StatusManager::EnemyMap& out)
{
	uint32_t count = 0;
	if (!reader.Read(count)) { return false; }

	for (uint32_t i = 0; i < count; i++)
	{
		int32_t id = 0;
		int32_t type = 0;
		uint8_t animCount = 0;
		StatusEnemy::EnemyParam param{};
		param.animationSpeed.fill(-1.0f);   // コンストラクタと同じ初期値(SPAWNはCSVに無いので-1のまま)

		const bool IS_OK =
			reader.Read(id) &&
			reader.ReadString(param.name) &&
			reader.ReadString(param.handlePath) &&
			reader.Read(type) &&
			reader.Read(param.scale) &&
			reader.Read(param.radius) &&
			reader.Read(param.hp) &&
			reader.Read(param.stamina) &&
			reader.Read(param.power) &&
			reader.Read(param.speedInit) &&
			reader.Read(param.speedMax) &&
			reader.Read(param.speedAcc) &&
			reader.Read(param.atkInterval) &&
			reader.Read(param.atkRange) &&
			reader.Read(param.searchRange) &&
			reader.Read(animCount);
		if (!IS_OK) { return false; }
		param.type = static_cast<StatusEnemy::ENEMY_TYPE>(type);

		for (uint8_t a = 0; a < animCount; a++)
		{
			// 個数が今のenumと違っても安全に読む(はみ出た分は捨てる)
			float speed = 0.0f;
			if (!reader.Read(speed)) { return false; }
			if (a < param.animationSpeed.size()) { param.animationSpeed[a] = speed; }
		}

		auto enemy = std::make_unique<StatusBoss>();   // LoadEnemyと同じ生成方法
		enemy->SetEnemyParam(param);
		out[id] = std::move(enemy);
	}
	return true;
}

bool StatusManager::ReadBinaryBossAction(BinaryReader& reader, StatusBoss& boss)
{
	uint32_t count = 0;
	if (!reader.Read(count)) { return false; }

	for (uint32_t i = 0; i < count; i++)
	{
		int32_t type = 0;
		StatusBoss::ActionParam a{};
		const bool IS_OK =
			reader.Read(type) && reader.ReadString(a.actionName) &&
			reader.Read(a.timeActive) && reader.Read(a.timeEnd) && reader.Read(a.timeTimingActive) &&
			reader.Read(a.animationSpeed) && reader.Read(a.attackRadius);
		if (!IS_OK) { return false; }

		boss.SetActionParam(static_cast<StatusBoss::ACTION_TYPE>(type), a);
	}
	return true;
}

bool StatusManager::SaveStatusDat(void) const
{
	bool ret = true;

	// プレイヤーパラメータの保存
	ret = SavePlayer();

	ret = SaveEnemy();

	return ret;
}

bool StatusManager::SavePlayer(void) const
{
	std::string filePath = PATH_PLAYER;
	filePath += FILE_EXTENSION_DAT;
	std::ofstream ofs(filePath.c_str(), std::ios::binary | std::ios::trunc);

	if (!ofs.is_open())
	{
		// ファイルオープン失敗
		std::string error = "\nバイナリファイルが読み込まれませんでした。\nファイルパス:";
		error += filePath;
		OutputDebugString(error.c_str());
		assert(false);
		return false;
	}

	// ヘッダ(未初期化メンバーが出ないよう全て設定する)
	StatusDatHeader header{};
	std::memcpy(header.signature, "stdt", BINARY_FILE_SIZE);
	header.dataNum = 2; // プレイヤーパラメータ + プレイヤーアクション
	WriteValue(ofs, header);

	WritePlayerStatus(ofs);
	WritePlayerAction(ofs);

	return ofs.good();
}

bool StatusManager::SaveEnemy(void) const
{
	std::string filePath = PATH_ENEMY;
	filePath += FILE_EXTENSION_DAT;
	std::ofstream ofs(filePath.c_str(), std::ios::binary | std::ios::trunc);

	if (!ofs.is_open())
	{
		// ファイルオープン失敗
		std::string error = "\nバイナリファイルが読み込まれませんでした。\nファイルパス:";
		error += filePath;
		OutputDebugString(error.c_str());
		assert(false);
		return false;
	}

	StatusDatHeader header{};
	std::memcpy(header.signature, "stdt", BINARY_FILE_SIZE);
	header.dataNum = 2; // 敵パラメータ + ボスアクション
	WriteValue(ofs, header);

	WriteEnemyData(ofs);
	WriteBossAction(ofs);   // WriteEnemyAction から変更

	return ofs.good();
}

void StatusManager::WritePlayerStatus(std::ofstream& ofs) const
{
	const auto& playerParam = player_->GetPlayerParam();

	WriteString(ofs, playerParam.handlePath);
	WriteValue(ofs, playerParam.scale);
	WriteValue(ofs, playerParam.scaleSword);
	WriteValue(ofs, playerParam.radius);
	WriteValue(ofs, playerParam.hp);
	WriteValue(ofs, playerParam.stamina);
	WriteValue(ofs, playerParam.powerLight);
	WriteValue(ofs, playerParam.powerHeavy);
	WriteValue(ofs, playerParam.speedInit);
	WriteValue(ofs, playerParam.speedAccTime);
	WriteValue(ofs, playerParam.speedWalk);
	WriteValue(ofs, playerParam.speedDash);
	WriteValue(ofs, playerParam.dashSpeedMultiple);
	WriteValue(ofs, playerParam.timeEvasion);
	WriteValue(ofs, playerParam.timeInv);

	// PlayerParam外のアニメーション速度
	const auto& animationSpeed = player_->GetAnimationSpeed();
	WriteValue(ofs, animationSpeed.idle);
	WriteValue(ofs, animationSpeed.walk);
	WriteValue(ofs, animationSpeed.run);
}

void StatusManager::WritePlayerAction(std::ofstream& ofs) const
{
	// ACTION_TYPE順に並べ替えて出力を安定させる
	const auto& SRC = player_->GetPlayerAction();
	const std::map<StatusPlayer::ACTION_TYPE, StatusPlayer::ActionParam> SORTED(SRC.begin(), SRC.end());

	WriteValue(ofs, static_cast<uint32_t>(SORTED.size()));

	for (const auto& [type, param] : SORTED)
	{
		WriteValue(ofs, static_cast<int32_t>(type));
		WriteString(ofs, param.actionName);
		WriteValue(ofs, param.timeActive);
		WriteValue(ofs, param.timeInput);
		WriteValue(ofs, param.timeEnd);
		WriteValue(ofs, param.timeHitStop);
		WriteValue(ofs, param.timeTimingActive);
		WriteValue(ofs, param.timeTimingHitStop);
		WriteValue(ofs, param.animationSpeed);
	}
}

void StatusManager::WriteEnemyData(std::ofstream& ofs) const
{
	// std::mapはキー(敵ID)の昇順なので、そのまま出力順が安定する
	WriteValue(ofs, static_cast<uint32_t>(enemy_.size()));

	for (const auto& [id, enemy] : enemy_)
	{
		const auto& param = enemy->GetEnemyParam();

		WriteValue(ofs, static_cast<int32_t>(id));
		WriteString(ofs, param.name);
		WriteString(ofs, param.handlePath);
		WriteValue(ofs, static_cast<int32_t>(param.type));
		WriteValue(ofs, param.scale);
		WriteValue(ofs, param.radius);
		WriteValue(ofs, param.hp);
		WriteValue(ofs, param.stamina);
		WriteValue(ofs, param.power);
		WriteValue(ofs, param.speedInit);
		WriteValue(ofs, param.speedMax);
		WriteValue(ofs, param.speedAcc);
		WriteValue(ofs, param.atkInterval);
		WriteValue(ofs, param.atkRange);
		WriteValue(ofs, param.searchRange);

		// アニメーション速度(将来ANIM_TYPEが増えても読めるよう、個数を先に書く)
		WriteValue(ofs, static_cast<uint8_t>(param.animationSpeed.size()));
		for (const auto& animationSpeed : param.animationSpeed)
		{
			WriteValue(ofs, animationSpeed);
		}
	}
}

void StatusManager::WriteBossAction(std::ofstream& ofs) const
{
	// unordered_mapは順序が不定なので、ACTION_TYPE順に並べ替えて出力を安定させる
	const auto& SRC = boss_->GetBossAction();
	const std::map<StatusBoss::ACTION_TYPE, StatusBoss::ActionParam> SORTED(SRC.begin(), SRC.end());

	WriteValue(ofs, static_cast<uint32_t>(SORTED.size()));

	for (const auto& [type, param] : SORTED)
	{
		WriteValue(ofs, static_cast<int32_t>(type));
		WriteString(ofs, param.actionName);
		WriteValue(ofs, param.timeActive);
		WriteValue(ofs, param.timeEnd);
		WriteValue(ofs, param.timeTimingActive);
		WriteValue(ofs, param.animationSpeed);
		WriteValue(ofs, param.attackRadius);
	}
}

const std::string& StatusManager::GetHandlePathPlayer(void)const
{
	return player_->GetPlayerParam().handlePath;
}

const std::string& StatusManager::GetHandlePathEnemy(int type)const
{
	return enemy_.at(type)->GetEnemyParam().handlePath;
}

void StatusManager::WriteString(std::ofstream& ofs, const std::string& str)
{
	// バイナリファイルに書き込み
	const auto SIZE = static_cast<uint16_t>(str.size());
	WriteValue(ofs, SIZE);
	ofs.write(str.data(), SIZE);
}