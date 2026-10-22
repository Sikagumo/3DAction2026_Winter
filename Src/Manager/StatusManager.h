#include <string>
#include <memory>
#include <array>
#include <vector>
#include <map>

class StatusPlayer;
class StatusEnemy;
class StatusBoss;

class StatusManager
{
public:

	// 敵パラメータマップ
	using EnemyMap = std::map<int, std::unique_ptr<StatusEnemy>>;

	// セーブデータのハンドル
	const char* PATH_PLAYER = "PlayerData.csv";
	const char* PATH_PLAYER_ACTION = "PlayerActionData.csv";
	const char* PATH_ENEMY = "EnemyData.csv";
	const char* PATH_BOSS_ACTION = "BossActionData.csv";


	/// @brief インスタンス処理
	static void CreateInstance(void);
	static StatusManager& GetInstance(void);
	static void DestroyInstance(void);

	/// @brief 初回読み込み処理
	void Load(void);


	/// @brief プレイヤーステータス参照
	StatusPlayer& GetPlayerStatus(void)const { return *player_; };

	/// @brief 敵ステータス参照
	StatusEnemy& GetEnemyStatus(int target)const { return *enemy_.at(target - 1).get(); };

	/// @brief 敵ステータスマップ参照
	const EnemyMap& GetEnemyStatusMap(void)const { return enemy_; };

	StatusBoss& GetBossStatus(void)const { return *boss_; };

	/// @brief モデルのハンドルパス取得処理
	const std::string& GetHandlePathPlayer(void)const;
	const std::string& GetHandlePathEnemy(int type)const;


private:

	using CsvRow = std::vector<std::string>;
	using CsvTable = std::vector<CsvRow>;

	// インスタンス
	static StatusManager* instance_;

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

	/// @brief CSVファイル読み込み処理
	/// @param path ファイルパス
	std::string ReadCsvFile(const std::string& path);

	/// @brief CSVを読み込み行列に変換
	/// @param fileName 読み込むCSVファイル名
	/// @param columnCount 1行あたりに読む最大列数
	CsvTable ReadCsvTable(const std::string& fileName, size_t columnCount);


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
		const CsvTable TABLE = ReadCsvTable(fileName, t_SizeRow);
		for (size_t i = 0; i < TABLE.size(); i++)
		{
			getStatusFunc(i, ToArray<t_SizeRow>(TABLE[i]));
		}
	}
};