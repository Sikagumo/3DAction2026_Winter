#pragma once
#include <map>
#include <functional>
#include <vector>
#include <memory>
#include "../Common/Vector2.h"
#include "Input.h"

class InputManager
{
public:

	/// <summary>
	/// 入力状況の種類
	/// </summary>
	enum class TYPE
	{
		NONE,

		// プレイヤー移動
		PLAYER_MOVE_RIGHT,
		PLAYER_MOVE_LEFT,
		PLAYER_MOVE_BACK,
		PLAYER_MOVE_FRONT,

		// プレイヤー回転
		PLAYER_ROTATION_UP,
		PLAYER_ROTATION_DOWN,
		PLAYER_ROTATION_LEFT,
		PLAYER_ROTATION_RIGHT,

		// プレイヤーアクション
		PLAYER_ATTACK_JUB,	  // 弱攻撃
		PLAYER_ATTACK_STRONG, // 強攻撃
		PLAYER_DASH,      // ダッシュ
		PLAYER_DEFENSE,   // 守備

		GAME_STATE_CHANGE, // ゲーム状態遷移

		// 選択
		SELECT_RIGHT,
		SELECT_LEFT,
		SELECT_DOWN,
		SELECT_UP,
		SELECT_DECISION, // 決定
		SELECT_CANCEL,   // キャンセル

		DEBUG_SCENE_CHANGE, // デバッグシーン遷移
		DEBUG_MODE,

		PAUSE, // ポーズ(開閉)

		MAX,
	};


	/// @brief インスタンス処理 
	static void CreateInstance(void);
	static InputManager& GetInstance(void) { return *instance_; };
	static void DestroyInstance(void);

	void Init(void);
	void Update(void);
	void Release(void);


	/// <summary>
	/// 指定した状況での各入力機器の押下判定
	/// </summary>
	/// <param name="type">状況の種類</param>
	/// <param name="padNo">パッド番号(デフォルトはPAD1)</param>
	/// <returns>trueの場合入力中,falseの場合未入力</returns>
	bool IsNew(const TYPE type, const Input::JOYPAD_NO padNo = Input::JOYPAD_NO::PAD1);

	/// <summary>
	/// 指定した状況での各入力機器の押下判定(最初のみ)
	/// </summary>
	/// <param name="type">状況の種類</param>
	/// <param name="padNo">パッド番号(デフォルトはPAD1)</param>
	/// <returns>trueの場合入力中,falseの場合未入力</returns>
	bool IsTrgDown(const TYPE type, const Input::JOYPAD_NO padNo = Input::JOYPAD_NO::PAD1);

	/// <summary>
	/// 指定した状況での各入力機器の離した判定
	/// </summary>
	/// <param name="type">状況の種類</param>
	/// <param name="padNo">パッド番号(デフォルトはPAD1)</param>
	/// <returns>trueの場合離したタイミング,falseの場合まだ押下中</returns>
	bool IsTrgUp(const TYPE type, const Input::JOYPAD_NO padNo = Input::JOYPAD_NO::PAD1);

	/// <summary>
	/// マウス位置を設定
	/// </summary>
	/// <param name="pos"></param>
	void SetMousePos(const Vector2& pos);

	/// @brief カーソル状態遷移
	void SetCursorMode(Input::CURSOR_MODE _mode);

	/// <summary>
	/// マウス座標の取得
	/// </summary>
	/// <returns>マウス座標の取得</returns>
	Vector2 GetMousePos(void) const;

	/// @brief マウスの移動量 取得
	Vector2 GetMouseMove(void) const;

	/// @brief 左スティックの倒れた度合いを取得
	/// @param _num パッド番号
	Vector2 GetKnockLStickSize(Input::JOYPAD_NO num = Input::JOYPAD_NO::PAD1) const;
	
	/// @brief 右スティックの倒れた度合いを取得
	/// @param _num パッド番号
	Vector2 GetKnockRStickSize(Input::JOYPAD_NO num = Input::JOYPAD_NO::PAD1) const;

	/// @brief 左スティックの倒れた方向を取得
	/// @param _num パッド番号
	VECTOR GetDirXZ_LStick(Input::JOYPAD_NO num = Input::JOYPAD_NO::PAD1, float _threshold = STICK_THRESHOLD) const;
	
	/// @brief 右スティックの倒れた方向を取得
	/// @param _num パッド番号
	VECTOR GetDirXZ_RStick(Input::JOYPAD_NO num = Input::JOYPAD_NO::PAD1, float _threshold = STICK_THRESHOLD) const;


private:


	static constexpr float STICK_THRESHOLD = 1000.0f;
	static constexpr float MOUSE_THRESHOLD = 100.0f;

	static InputManager* instance_;


	// 入力トリガーの情報
	struct TriggerInfo
	{
		std::vector<int> keys;
		std::vector<Input::JOYPAD_BTN> padButtons;
		std::vector<Input::JOYPAD_STICK> padSticks;
		Input::MOUSE mouse = Input::MOUSE::MAX;
	};

	// 入力判定クラス
	std::unique_ptr<Input> input_;

	// 入力状況別のトリガーを管理するマップ
	std::map<TYPE, TriggerInfo> triggerMap_;

	// 入力状況に応じた押下処理を管理するマップ
	std::map<TYPE, std::vector<std::function<bool(TYPE, Input::JOYPAD_NO)>>> funcNewMap_;
	std::map<TYPE, std::vector<std::function<bool(TYPE, Input::JOYPAD_NO)>>> funcTrgDownMap_;
	std::map<TYPE, std::vector<std::function<bool(TYPE, Input::JOYPAD_NO)>>> funcTrgUpMap_;


	/// @brief 入力状況に応じたトリガーを登録
	void RegisterTrigger(const TYPE type, 
						 const std::vector<int> keys,
						 const std::vector<Input::JOYPAD_BTN> padButtons,
						 const std::vector<Input::JOYPAD_STICK> padSticks,
						 const Input::MOUSE mouse = Input::MOUSE::MAX);

	/// @brief 処理の登録
	void RegisterTriggerFunction(const InputManager::TYPE type, 
								 std::vector<std::function<bool(TYPE, Input::JOYPAD_NO)>> newFuncs,
								 std::vector<std::function<bool(TYPE, Input::JOYPAD_NO)>> trgDownFuncs,
								 std::vector<std::function<bool(TYPE, Input::JOYPAD_NO)>> trgUpFuncs);

	/// @brief キー関係の入力判定
	bool IsNewKey(const TYPE type);
	bool IsTrgDownKey(const TYPE type);
	bool IsTrgUpKey(const TYPE type);

	/// @brief パッドボタンの入力判定
	bool IsNewPadButton(const TYPE type, const Input::JOYPAD_NO padNo);
	bool IsTrgDownPadButton(const TYPE type, const Input::JOYPAD_NO padNo);
	bool IsTrgUpPadButton(const TYPE type, const Input::JOYPAD_NO padNo);

	/// @brief パッドスティックの入力判定
	bool IsNewPadStick(const TYPE type, const Input::JOYPAD_NO padNo);
	bool IsTrgDownPadStick(const TYPE type, const Input::JOYPAD_NO padNo);
	bool IsTrgUpPadStick(const TYPE type, const Input::JOYPAD_NO padNo);

	/// @brief マウスの入力判定
	bool IsNewMouse(const TYPE type);
	bool IsTrgDownMouse(const TYPE type);
	bool IsTrgUpMouse(const TYPE type);


	/// @brief コンストラクタ
	InputManager(void);

	/// @brief デストラクタ
	~InputManager(void) = default;

	/// @brief コピーコンストラクタ対策
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;
};