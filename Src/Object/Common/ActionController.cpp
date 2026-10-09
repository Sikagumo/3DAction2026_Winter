#include "ActionController.h"
#include <DxLib.h>
#include "../../Manager/SceneManager.h"
#include "./AnimationController.h"

ActionController::ActionController(std::unique_ptr<AnimationController>& animation)
	: animation_(animation)
	, actionState_(STATE::NONE)
{
}

void ActionController::SetAction(int actionNum, const ActionParam& param
								 , std::unique_ptr<IActionCommand> command)
{
	if (actionNum < 0)
	{
#ifdef _DEBUG
		OutputDebugString("\n行動番号は０以上の値で登録してください！\n");
#endif
		return;
	}

	if (actions_.contains(actionNum))
	{
#ifdef _DEBUG
		OutputDebugString("\n同一番号の行動が既に登録されています！\n");
#endif
		return;
	}

	actions_.emplace(actionNum, ActionEntry{ param, std::move(command) });
}

void ActionController::SetAction(int _actionNum, const ActionParam& _param
								 , std::function<void(void)> _func)
{
	std::unique_ptr<IActionCommand> command;
	if (_func)
	{
		command = std::make_unique<FunctionActionCommand>(std::move(_func));
	}
	SetAction(_actionNum, _param, std::move(command));
}

void ActionController::Active(int actionNum)
{
	auto it = actions_.find(actionNum);
	if (it == actions_.end())
	{
		OutputDebugString("\n有効にしようとした行動が見つかりませんでした。\n");
		return;
	}

#ifdef _DEBUG
	if (actionState_ != STATE::NONE)
	{
		OutputDebugString("\n行動中でしたが、行動を上書きしました。\n");
	}
#endif

	const ActionParam& param = it->second.param;

	actionState_ = STATE::ACTION;
	curActionNum_ = actionNum;
	curTimeAction_ = param.timeActive;
	curTimeStopActive_ = param.timeStopTiming;
	curTimeActionActive_ = param.timeActionTiming;
	curTimeInput_ = param.timeInput;
}

bool ActionController::IsActiveInput(void) const
{
	// 行動していない、または入力時間が未割当の場合は false
	if (curActionNum_ == -1) { return false; }
	if (actions_.at(curActionNum_).param.timeInput <= 0.0f) { return false; }

	return (curTimeInput_ <= 0.0f && actionState_ == STATE::ACTION);
}

void ActionController::Update(void)
{
	if (curActionNum_ == -1 || actionState_ == STATE::NONE) { return; }

	const bool isStop = (animation_ != nullptr && animation_->IsStop());
	if (!isStop)
	{
		curTimeAction_ -= SceneManager::GetInstance().GetDeltaTime();
	}

	if (actionState_ == STATE::ACTION)
	{
		Update_Action();
	}
	else if (actionState_ == STATE::END)
	{
		Update_End();
	}
}

void ActionController::Update_Action(void)
{
	const float delta = SceneManager::GetInstance().GetDeltaTime();
	const ActionEntry& entry = actions_.at(curActionNum_);

	if (curTimeStopActive_ > 0.0f)
	{
		curTimeStopActive_ -= delta;

		// 一度だけ停止を実行
		if (curTimeStopActive_ <= 0.0f && animation_ != nullptr)
		{
			animation_->Stop(entry.param.timeStop);
		}
	}

	if (animation_ != nullptr && animation_->IsStop()) { return; }

	curTimeInput_ = ((curTimeInput_ > 0.0f) ? (curTimeInput_ - delta) : 0.0f);

	if (curTimeActionActive_ > 0.0f)
	{
		curTimeActionActive_ -= delta;

		// 一度だけコマンドを実行
		if (curTimeActionActive_ <= 0.0f && entry.command != nullptr)
		{
			entry.command->Execute();
		}
	}

	if (curTimeAction_ < 0.0f)
	{
		actionState_ = STATE::END;
		curTimeAction_ = entry.param.timeEnd;
	}
}

void ActionController::Update_End(void)
{
	if (curTimeAction_ < 0.0f)
	{
		actionState_ = STATE::NONE;
		curActionNum_ = -1;
	}
}

void ActionController::DrawDebug(void)
{
#ifdef _DEBUG
	if (actionState_ == STATE::NONE) { return; }

	DrawFormatString(0, 16, 0xffff00, "Action(type:%s, active: %.1f, input:%.1f, num:%d)"
		, (actionState_ == STATE::ACTION) ? "ACTION" : "END"
		, curTimeAction_, curTimeInput_, curActionNum_);
#endif
}
