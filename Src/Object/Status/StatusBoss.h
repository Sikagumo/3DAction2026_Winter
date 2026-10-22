#pragma once
#include <string>
#include <array>
#include <unordered_map>
#include "../../Utility/UtilityCommon.h"
#include "./StatusEnemy.h"


class StatusBoss : public StatusEnemy
{
public:

	enum class ANIM_TYPE
	{
		IDLE,
		ATTACK,
		WALK,
		HIT_1,
		HIT_2,
		DEATH,

		MAX,
	};

	enum class ACTION_TYPE
	{
		NONE = -1,    // 未割当
		ATTACK_LIGHT, // 弱攻撃
		ATTACK_HEAVY, // 強攻撃
		DEFENSE,      // 防御

		MAX,
	};

	enum class ACTION_PARAM
	{
		NAME = 0,       // モーション名

		TIME_ACTIVE,    // 有効時間
		TIME_END,       // 後隙時間
		TIME_TIMING_ACTIVE,  // 有効のタイミング

		ANIMATION_SPEED, // モーションアニメーション速度
		ATTACK_RADIUS,    // 攻撃範囲

		MAX,
	};

	struct ActionParam
	{
		std::string actionName = "";

		float timeActive = 0.0f;

		float timeEnd = 0.0f;

		float timeTimingActive = 0.0f;

		float animationSpeed = 0.0f;

		float attackRadius = 0.0f;
	};


	StatusBoss(void);
	~StatusBoss(void)override = default;

	void LoadActionParam(ACTION_TYPE type, const std::array<std::string, static_cast<int>(ACTION_PARAM::MAX)>& _loadString)
	{
		ActionParam action = ActionParam();

		action.actionName = _loadString[static_cast<int>(ACTION_PARAM::NAME)];
		UtilityCommon::ChangeString(_loadString[static_cast<int>(ACTION_PARAM::TIME_ACTIVE)], action.timeActive, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(ACTION_PARAM::TIME_END)], action.timeEnd, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(ACTION_PARAM::TIME_TIMING_ACTIVE)], action.timeTimingActive, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(ACTION_PARAM::ANIMATION_SPEED)], action.animationSpeed, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(ACTION_PARAM::ATTACK_RADIUS)], action.attackRadius, 0.0f);
		action_.emplace(type, action);
	}
	
	const std::unordered_map<ACTION_TYPE, ActionParam>& GetBossAction(void) { return action_; }


private:

	// モーションパラメータ
	std::unordered_map<ACTION_TYPE, ActionParam> action_;
};