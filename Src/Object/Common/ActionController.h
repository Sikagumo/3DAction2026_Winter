#pragma once
#include <map>
#include <memory>
#include <functional>
#include "./ActionCommand.h"

class AnimationController;

/// @brief 行動コントローラ(Commandパターンの Invoker)
///        プレイヤー・敵・ボス共通で利用する
class ActionController
{
public:

	enum class STATE
	{
		NONE = -1,
		ACTION,
		END,
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

	STATE GetActionState(void) const { return actionState_; }

private:

	struct ActionEntry
	{
		ActionParam param;
		std::unique_ptr<IActionCommand> command;
	};

	std::map<int, ActionEntry> actions_;

	std::unique_ptr<AnimationController>& animation_;

	STATE actionState_;

	int curActionNum_ = -1;
	float curTimeAction_ = 0.0f;
	float curTimeActionActive_ = 0.0f;
	float curTimeInput_ = 0.0f;
	float curTimeStopActive_ = 0.0f;

	void Update_Action(void);
	void Update_End(void);
};
