#pragma once
#include <string>
#include <array>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../../Utility/UtilityCommon.h"

class StatusPlayer
{
public:

	// プレイヤー保存データ
	enum class PARAM
	{
		HANDLE_PATH,  // モデルのハンドルパス
		SCALE_MODEL,  // モデルのスケール
		SCALE_SWARD,  // 剣モデルのスケール
		RADIUS,          // 半径
		HP_MAX,          // 最大HP
		STAMINA_MAX,     // 最大スタミナ
		POWER_LIGHT,     // 弱攻撃力
		POWER_HEAVY,     // 強攻撃力
		ANIMATION_SPEED, // 初期移動速度
		SPEED_ACC_TIME,  // 最高速度到達度
		SPEED_WALK,      // 歩行速度
		SPEED_DASH,      // ダッシュ速度
		DASH_SPEED_MULT, // ダッシュ時の増加倍率
		TIME_INVINCIBLE, // 無敵時間
		TIME_EVASION,    // 回避時間

		ANIM_SPEED_IDLE, // 待機アニメーション速度
		ANIM_SPEED_WALK, // 移動アニメーション速度
		ANIM_SPEED_DASH, // ダッシュアニメーション速度

		MAX,
	};

	enum class ACTION_TYPE
	{
		NONE = -1,    // 未割当
		ATTACK_LIGHT, // 弱攻撃
		ATTACK_HEAVY, // 強攻撃
		DEFENSE,      // 防御
		DODGE,        // 回避

		MAX,
	};
	enum class ACTION_PARAM
	{
		NAME = 0,       // 行動名

		TIME_ACTIVE,    // 有効時間
		TIME_INPUT,     // 入力時間
		TIME_END,       // 後隙時間
		TIME_HIT_STOP,  // ヒットストップ時間
		TIME_TIMING_ACTIVE,  // 有効のタイミング
		TIME_TIMING_HIT_STOP,// ヒットストップのタイミング

		ANIMATION_SPEED, // モーションアニメーション速度

		MAX,
	};

	struct PlayerParam
	{
		std::string handlePath = "";

		float scale = 0.0f;
		float scaleSward = 0.0f;

		float radius = 0.0f;

		int hp = 0;

		int stamina = 0;

		// 攻撃力
		int powerLight = 0;
		int powerHeavy = 0;

		// 初速
		float speedInit = 0.0f;

		// 最高速度到達度
		float speedAccTime = 0.0f;

		// 歩行速度
		float speedWalk = 0.0f;

		// ダッシュ速度
		float speedDash = 0.0f;

		float dashSpeedMult = 0.0f;

		float timeEvasion = 0.0f;

		// 回避時間
		float timeInv = 0.0f;
	};

	struct ActionParam
	{
		std::string actionName = "";

		float timeActive = 0.0f;

		float timeInput = 0.0f;

		float timeEnd = 0.0f;

		float timeHitStop = 0.0f;

		float timeTimingActive = 0.0f;

		float timeTimingHitStop = 0.0f;

		float animationSpeed = 0.0f;
	};


	StatusPlayer(void);

	~StatusPlayer(void) = default;

	void LoadStatusParam(const std::array<std::string, static_cast<int>(PARAM::MAX)>& _loadString)
	{
		playerParam_.handlePath = _loadString[static_cast<int>(PARAM::HANDLE_PATH)];

		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::SCALE_MODEL)], playerParam_.scale, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::SCALE_SWARD)], playerParam_.scaleSward, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::RADIUS)], playerParam_.radius, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::HP_MAX)], playerParam_.hp, 0);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::STAMINA_MAX)], playerParam_.stamina, 0);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::POWER_LIGHT)], playerParam_.powerLight, 0);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::POWER_HEAVY)], playerParam_.powerHeavy, 0);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIMATION_SPEED)], playerParam_.speedInit, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::SPEED_ACC_TIME)], playerParam_.speedAccTime, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::SPEED_WALK)], playerParam_.speedWalk, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::SPEED_DASH)], playerParam_.speedDash, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::DASH_SPEED_MULT)], playerParam_.dashSpeedMult, 1.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::TIME_INVINCIBLE)], playerParam_.timeInv, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::TIME_EVASION)], playerParam_.timeEvasion, 0.0f);

		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_IDLE)], animationSpeedIdle_, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_WALK)], animationSpeedWalk_, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_DASH)], animationSpeedRun_, 0.0f);
	}

	void LoadActionParam(ACTION_TYPE type, const std::array<std::string, static_cast<int>(ACTION_PARAM::MAX)>& _loadString)
	{
		ActionParam action = ActionParam();

		action.actionName = _loadString[static_cast<int>(ACTION_PARAM::NAME)];
		UtilityCommon::ChangeString(_loadString[static_cast<int>(ACTION_PARAM::TIME_ACTIVE)], action.timeActive, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(ACTION_PARAM::TIME_INPUT)], action.timeInput, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(ACTION_PARAM::TIME_END)], action.timeEnd, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(ACTION_PARAM::TIME_HIT_STOP)], action.timeHitStop, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(ACTION_PARAM::TIME_TIMING_ACTIVE)], action.timeTimingActive, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(ACTION_PARAM::TIME_TIMING_HIT_STOP)], action.timeTimingHitStop, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(ACTION_PARAM::ANIMATION_SPEED)], action.animationSpeed, 0.0f);
		action_.emplace(type, action);
	}

	/// @brief プレイヤーパラメータ取得
	const PlayerParam& GetPlayerParam(void) { return playerParam_; }

	const std::unordered_map<ACTION_TYPE, ActionParam>& GetBossAction(void) { return action_; }

	/// @brief アニメーション速度
	float GetAnimationSpeedIdle(void)const { return animationSpeedIdle_; }
	float GetAnimationSpeedWalk(void)const { return animationSpeedWalk_; }
	float GetAnimationSpeedRun(void)const { return animationSpeedRun_; }


private:
	
	PlayerParam playerParam_;

	// アニメーション速度
	float animationSpeedIdle_ = 0.0f;
	float animationSpeedWalk_ = 0.0f;
	float animationSpeedRun_ = 0.0f;

	// モーションパラメータ
	std::unordered_map<ACTION_TYPE, ActionParam> action_;
};