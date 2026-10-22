#pragma once
#include <DxLib.h>
#include <string>
#include "../Utility/UtilityCommon.h"
#include "../Common/Vector2.h"

class SceneManager;
class InputManager;

class GameExit
{
public:

	
	/// @brief 終了確認状態
	enum class SELECT
	{
		NONE = -1,
		YES, // ゲーム終了
		NO,  // 終了無効

		MAX,
	};
	

	// メニューのテキスト
	const char* TEXT_LABEL_END = "ゲームを終了する？";

	const char* TEXT_LABEL_TITLE = "タイトルに戻る？";


	// 決定のテキスト
	const char* TEXT_YES = "はい";

	// キャンセルのテキスト
	const char* TEXT_NO = "いいえ";


	// テキスト位置
	static constexpr Vector2 MENU_TEXT_OFFSET = { -75, -100 };


	// メニューの横サイズ
	static constexpr int MENU_WIDTH  = 500;

	// メニューの縦サイズ
	static constexpr int MENU_HEIGHT = 250;

	// メニューの色
	static constexpr UtilityCommon::Color MENU_COLOR = { 125, 125, 125, (255 - 50) };

	// 背景の透明度
	static constexpr int BACK_ALPHA = (255 - 50);


	// メニューのテキストの非選択時の透明度
	static constexpr UtilityCommon::Color MENU_TEXT_COLOR = { 255, 0, 0, (255 - 200) };

	// テキストの間隔
	static constexpr Vector2 TEXT_OFFSET = {200, 75};



	/// @brief デフォルトコンストラクタ
	GameExit(void);

	/// @brief デストラクタ
	~GameExit(void) = default;

	
	/// @brief 初期化処理
	void Init(void);

	/// @brief 更新処理
	void Update(void);

	/// @brief 描画処理
	void Draw(void);

	/// @brief 解放処理
	void Release(void);

	
	/// @brief 終了メニューが有効か判定
	bool GetIsActiveMenu(void)const { return isActiveMenu_; };


private:

	SceneManager& sceneMng_;
	InputManager& input_;

	// 終了確認状態
	SELECT select_;


	// 終了確認画面有効化フラグ
	bool isActiveMenu_;

	// ゲーム終了フラグ
	bool isEnd_;


	/// @brief 選択判定
	bool IsSelect(void);
};
