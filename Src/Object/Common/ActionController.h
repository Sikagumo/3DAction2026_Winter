#pragma once
#include <map>
#include <memory>
#include <functional>
#include "ActionCommand.h"

class AnimationController;

/// @brief 行動コントローラ
class ActionController
{
public:

	/// @brief 行動状態
	enum class ACTION_STATE
	{
		NONE = -1, // 無効状態
		ACTION,    // 行動中
		END,       // 終了状態
	};

	/// @brief 行動パラメータ(秒)
	struct ActionParam
	{
		// 有効時間
		float timeActive = 0.0f;

		// 行動(コマンド実行)までの時間
		float timeActionTiming = 0.0f;

		// 行動終了後の時間
		float timeEnd = 0.0f;

		// 停止する時間
		float timeStop = 0.0f;

		// 停止までの時間
		float timeStopTiming = 0.0f;

		// 次の入力までの時間(コンボ用)
		float timeInput = 0.0f;
	};

	ActionController(std::unique_ptr<AnimationController>& animation);
	~ActionController(void) = default;

	void Update(void);
	void DrawDebug(void);

	/// @brief 行動の登録(コマンド所有版)
	void SetAction(int _actionNum, const ActionParam& param
		, std::unique_ptr<IActionCommand> command);

	/// @brief 行動の登録(std::function版。std::bindからの移行用)
	void SetAction(int _actionNum, const ActionParam& param
		, std::function<void(void)> function);

	/// @brief 登録した行動を開始する
	void Active(int actionNum);

	/// @brief 入力可能か否か
	bool IsActiveInput(void) const;

	/// @brief 行動中か否か
	bool IsActiveAction(void) const { return (curActionNum_ != -1); }

	/// @brief 現在の行動番号を取得
	int GetCurActionNum(void) const { return curActionNum_; }

	/// @brief コマンド実行が済んでいるか否か
	bool IsEndActionActive(void) const { return (curTimeActionActive_ <= 0.0f); }

	/// @brief 行動が終了しているか否か
	bool IsActionEnd(void)const { return (actionState_ != ACTION_STATE::ACTION && curTimeAction_ <= 0.0f); }

	ACTION_STATE GetActionState(void) const { return actionState_; }

	void SetIsSlow(bool isSlow) { isSlow_ = isSlow; }


private:

	struct ActionEntry
	{
		ActionParam param;
		std::unique_ptr<IActionCommand> command;
	};

	std::map<int, ActionEntry> actions_;

	std::unique_ptr<AnimationController>& animation_;

	bool isSlow_ = false;

	// 現在の行動状態
	ACTION_STATE actionState_;

	int curActionNum_ = -1;

	// 有効時間
	float curTimeAction_ = 0.0f;

	// 行動までの時間
	float curTimeActionActive_ = 0.0f;

	// 次の入力までの有効時間
	float curTimeInput_ = 0.0f;

	// 停止までの時間
	float curTimeStopActive_ = 0.0f;

	/// @brief 行動中状態の更新
	void Update_Action(void);

	/// @brief 終了中状態の更新
	void Update_End(void);
};
