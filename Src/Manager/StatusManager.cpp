#include "StatusManager.h"
#include <DxLib.h>
#include <fstream>
#include <sstream>
#include <array>
#include <string>
#include <memory>
#include <cassert>
#include <map>
#include "../Object/Status/StatusPlayer.h"
#include "../Object/Status/StatusEnemy.h"
#include "./ResourceManager.h"

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
	LoadPlayerStatus();

	LoadPlayerMotion();

	LoadEnemy();
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

void StatusManager::LoadPlayerMotion(void)
{
	constexpr size_t COLS_MAX = static_cast<size_t>(StatusPlayer::MOTION_PARAM::MAX);
	constexpr size_t TYPES = static_cast<size_t>(StatusPlayer::MOTION_TYPE::MAX);

	LoadCsv<COLS_MAX>(PATH_PLAYER_MOTION, [this](size_t i, const auto& row)
		{
			// 行が多すぎる場合の範囲外対策
			if (i >= TYPES) { return; }
			
			player_->LoadMotionParam(static_cast<StatusPlayer::MOTION_TYPE>(i), row);
		}
	);
}

void StatusManager::LoadEnemy(void)
{
	constexpr size_t COLS_MAX = static_cast<size_t>(StatusEnemy::PARAM::MAX);
	constexpr size_t TYPES = static_cast<size_t>(StatusEnemy::TYPE::MAX);

	LoadCsv<COLS_MAX>(PATH_ENEMY, [this](size_t i, const auto& row)
		{
			// 行が多すぎる場合の範囲外対策
			if (i >= TYPES) { return; }

			const int ENEMY_TYPE = static_cast<int>(i);
			enemy_[ENEMY_TYPE] = std::make_unique<StatusEnemy>();
			enemy_[ENEMY_TYPE]->LoadParam(row);
		}
	);
}

std::string& StatusManager::GetHandlePathPlayer(void)
{
	return player_->GetHandlePath();
}

std::string& StatusManager::GetHandlePathEnemy(int type)
{
	return enemy_[type]->GetHandlePath();
}