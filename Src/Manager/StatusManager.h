#pragma once
#include <string>
#include <memory>
#include <array>
#include <vector>
#include <map>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <type_traits>

class BinaryReader;
class StatusPlayer;
class StatusEnemy;
class StatusBoss;

class StatusManager
{
public:

	// 敵パラメータマップ
	using EnemyMap = std::map<int, std::unique_ptr<StatusEnemy>>;


	/// @brief インスタンス処理
	static void CreateInstance(void);
	static StatusManager& GetInstance(void);
	static void DestroyInstance(void);

	/// @brief 初回読み込み処理
	void Load(void);


	/// @brief プレイヤーステータス参照
	StatusPlayer& GetPlayerStatus(void)const { return *player_; };

	/// @brief 敵ステータス参照
	const StatusEnemy& GetEnemyStatus(int target);

	/// @brief 敵ステータスマップ参照
	const EnemyMap& GetEnemyStatusMap(void)const { return enemy_; };

	StatusBoss& GetBossStatus(void)const { return *boss_; };

	/// @brief モデルのハンドルパス取得処理
	const std::string& GetHandlePathPlayer(void)const;
	const std::string& GetHandlePathEnemy(int type)const;

	/// @brief 現在のステータスを.datファイルへ書き出す
	/// @return 成功時、true
	bool SaveStatusDat(void) const;


private:

	using CsvRow = std::vector<std::string>;
	using CsvTable = std::vector<CsvRow>;

	static constexpr const char* FILE_EXTENSION_CSV = ".csv";
	static constexpr const char* FILE_EXTENSION_DAT = ".dat";

	// セーブデータのハンドル
	static constexpr const char* PATH_PLAYER = "PlayerData";
	static constexpr const char* PATH_PLAYER_ACTION = "PlayerActionData";
	static constexpr const char* PATH_ENEMY = "EnemyData";
	static constexpr const char* PATH_BOSS_ACTION = "BossActionData";


	// インスタンス
	static StatusManager* instance_;

	std::unique_ptr<BinaryReader> reader_ = nullptr;

	// プレイヤーパラメータ
	std::unique_ptr<StatusPlayer> player_ = nullptr;

	// 敵パラメータ	
	EnemyMap enemy_;

	// ボスパラメータ
	std::unique_ptr<StatusBoss> boss_ = nullptr;


	/// @brief デフォルトコンストラクタ
	StatusManager(void);

	/// @brief デフォルトデストラクタ
	~StatusManager(void) = default;

	/// @brief コピーコンストラクタ対策
	StatusManager(const StatusManager&) = delete;
	StatusManager& operator=(const StatusManager&) = delete;
	StatusManager(StatusManager&&) = delete;
	StatusManager& operator=(StatusManager&&) = delete;

	/// @brief 各外部ファイル読み込み
	void LoadPlayerStatus(void);
	void LoadPlayerAction(void);
	void LoadEnemy(void);
	void LoadBossAction(void);

	/// @brief バイナリファイル読み込み処理
	bool ReadBinaryPlayerStatus(BinaryReader& reader, StatusPlayer& player);
	bool ReadBinaryPlayerAction(BinaryReader& reader, StatusPlayer& player);
	bool ReadBinaryEnemyData(BinaryReader& reader, StatusManager::EnemyMap& out);
	bool ReadBinaryBossAction(BinaryReader& reader, StatusBoss& boss);

	/// @brief 現在のステータスを.datファイルへ書き出す
	/// @return 成功時、true
	bool SavePlayer(void) const;
	bool SaveEnemy(void) const;

	/// @brief バイナリファイルに書き出し
	/// @param ofs 出力ストリーム
	void WritePlayerStatus(std::ofstream& ofs) const;
	void WritePlayerAction(std::ofstream& ofs) const;
	void WriteEnemyData(std::ofstream& ofs) const;
	void WriteBossAction(std::ofstream& ofs) const;

	/// @brief CSVファイル読み込み処理
	/// @param path ファイルパス
	std::string ReadCsvFile(const std::string& path);

	/// @brief CSVを読み込み行列に変換
	/// @param fileName 読み込むCSVファイル名
	/// @param columnCount 1行あたりに読む最大列数
	CsvTable ReadCsvTable(const std::string& fileName, size_t columnCount);

	/// @brief ファイルを読み込み、ヘッダを検証する
	bool OpenDat(const std::string& path, uint32_t expectedSections, BinaryReader& reader);
	

	/// @brief 行を配列へ変換(不足分は空文字)
	/// @tparam t_Size 要素数
	/// @param row 各列の文字列
	template<size_t t_Size>
	static std::array<std::string, t_Size> ToArray(const CsvRow& row)
	{
		std::array<std::string, t_Size> result{};
		for (size_t i = 0; (i < t_Size && i < row.size()); i++)
		{
			result[i] = row[i];
		}
		return result;
	}

	/// @brief CSVを読み、1行ごとにonRow(行番号, 列配列)を呼ぶ
	/// @tparam t_Func 任意の関数
	/// @tparam t_SizeRow 列数
	/// @param fileName 読み込むファイル
	/// @param getStatusFunc ステータス取得関数
	template<size_t t_SizeRow, typename t_Func>
	void LoadCsv(const std::string& fileName, t_Func&& getStatusFunc)
	{
		const CsvTable TABLE = ReadCsvTable(fileName + FILE_EXTENSION_CSV, t_SizeRow);
		for (size_t i = 0; i < TABLE.size(); i++)
		{
			getStatusFunc(i, ToArray<t_SizeRow>(TABLE[i]));
		}
	}

	/// @brief 値をそのままバイナリで書き込む
	/// @tparam t_Param 値の型
	/// @param ofs 出力ストリーム
	/// @param value 書き込む値
	template<typename t_Param>
	static void WriteValue(std::ofstream& ofs, const t_Param& value)
	{
		static_assert(std::is_trivially_copyable_v<t_Param>);
		ofs.write(reinterpret_cast<const char*>(&value), sizeof(t_Param));
	}

	/// @brief 文字列を「長さ(uint16_t) + 本体」で書き込む
	/// @param ofs 出力ストリーム
	/// @param str 書き込む文字列
	static void WriteString(std::ofstream& ofs, const std::string& str);
};