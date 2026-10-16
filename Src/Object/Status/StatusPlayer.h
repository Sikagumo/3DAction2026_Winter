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
		MODEL_SCALE,  // モデルのスケール
		RADIUS,       // 半径
		HP_MAX,       // 最大HP
		STAMINA_MAX,  // 最大スタミナ
		POWER_WEAK,   // 弱攻撃力
		POWER_STRONG, // 強攻撃力
		MOVE_SPEED,   // 移動速度
		SPEED_ACC,    // 移動時の加速度
		DASH_SPEED_MULT, // ダッシュ時の増加倍率
		TIME_INVINCIBLE, // 無敵時間
		TIME_EVASION,    // 回避時間

		ANIM_SPEED_IDLE, // 待機アニメーション速度
		ANIM_SPEED_WALK, // 移動アニメーション速度
		ANIM_SPEED_DASH, // ダッシュアニメーション速度

		MAX,
	};

	enum class MOTION_TYPE
	{
		NONE = -1,
		JUB_1,
		JUB_2,
		JUB_END,

		SPECIAL,
		STRONG_1,
		STRONG_2,
		STRONG_3,

		MAX,
	};
	enum class MOTION_PARAM
	{
		NAME = 0,       // モーション名
		TIME_ACTIVE,    // 有効時間
		TIME_INPUT,     // 入力時間
		TIME_HIT_STOP,  // ヒットストップ時間
		TIME_END,       // 後隙時間
		TIME_TIMING_ACTIVE,// 有効のタイミング
		TIME_TIMING_HIT_STOP,// ヒットストップのタイミング

		MOVE_SPEED, // モーションアニメーション速度

		MAX,
	};

	struct PlayerParam
	{
		std::string handlePath = "";

		float scale = 0.0f;

		float radius = 0.0f;

		int hp = 0;

		int stamina = 0;

		// 攻撃力
		int powerWeak = 0;
		int powerStrong = 0;

		float speed = 0.0f;

		float speedAcc = 0.0f;

		float dashSpeedMult = 0.0f;

		float timeEvasion = 0.0f;

		// 回避時間
		float timeInv = 0.0f;
	};

	struct Motion
	{
		std::string motionName = "";

		float timeActive = 0.0f;

		float timeInput = 0.0f;

		float timeHitStop = 0.0f;

		float timeEnd = 0.0f;

		float timeTimingActive = 0.0f;

		float timeTimingHitStop = 0.0f;

		float animationSpeed = 0.0f;
	};


	StatusPlayer(void);

	~StatusPlayer(void) = default;

	void LoadStatusParam(const std::array<std::string, static_cast<int>(PARAM::MAX)>& _loadString)
	{
		playerParam_.handlePath = _loadString[static_cast<int>(PARAM::HANDLE_PATH)];

		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::MODEL_SCALE)], playerParam_.scale, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::HP_MAX)], playerParam_.hp, 0);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::RADIUS)], playerParam_.radius, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::POWER_WEAK)], playerParam_.powerWeak, 0);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::POWER_STRONG)], playerParam_.powerStrong, 0);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::MOVE_SPEED)], playerParam_.speed, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::SPEED_ACC)], playerParam_.speedAcc, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::DASH_SPEED_MULT)], playerParam_.dashSpeedMult, 1.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::TIME_INVINCIBLE)], playerParam_.timeInv, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::TIME_EVASION)], playerParam_.timeEvasion, 0.0f);

		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_IDLE)], animationSpeedIdle_, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_WALK)], animationSpeedWalk_, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_DASH)], animationSpeedRun_, 0.0f);
	}

	void LoadMotionParam(MOTION_TYPE type, const std::array<std::string, static_cast<int>(MOTION_PARAM::MAX)>& _loadString)
	{
		Motion motion = Motion();

		motion.motionName = _loadString[static_cast<int>(MOTION_PARAM::NAME)];
		UtilityCommon::ChangeString(_loadString[static_cast<int>(MOTION_PARAM::TIME_ACTIVE)], motion.timeActive, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(MOTION_PARAM::TIME_INPUT)], motion.timeInput, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(MOTION_PARAM::TIME_HIT_STOP)], motion.timeInput, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(MOTION_PARAM::TIME_END)], motion.timeEnd, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(MOTION_PARAM::TIME_TIMING_ACTIVE)], motion.timeTimingActive, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(MOTION_PARAM::MOVE_SPEED)], motion.animationSpeed, 0.0f);
		motion_.emplace(type, motion);
	}

	/// @brief 各パラメータ読み込み
	std::string& GetHandlePath(void) { return playerParam_.handlePath; }
	float GetScale(void)const { return playerParam_.scale; }
	float GetRadius(void)const { return playerParam_.radius; }
	int GetHp(void)const { return playerParam_.hp; }
	int GetPowerWeak(void)const { return playerParam_.powerWeak; }
	int GetPowerStrong(void)const { return playerParam_.powerStrong; }
	float GetSpeed(void)const { return playerParam_.speed; }
	float GetSpeedAcc(void)const { return playerParam_.speedAcc; }

	/// @brief ダッシュ時の加速倍率取得
	float GetDashSpeedMult(void)const { return playerParam_.dashSpeedMult; }

	/// @brief 無敵時間取得
	float GetTimeInvincible(void)const { return playerParam_.timeInv; }

	/// @brief 回避時間
	float GetTimeEvasion(void)const { return playerParam_.timeEvasion; }


	/// @brief アニメーション速度
	float GetAnimationSpeedIdle(void)const { return animationSpeedIdle_; }
	float GetAnimationSpeedWalk(void)const { return animationSpeedWalk_; }
	float GetAnimationSpeedRun(void)const { return animationSpeedRun_; }


	// モーション開始時間
	float GetMotionStart(MOTION_TYPE type) { return motion_[type].timeActive; }
	float GetMotionStart(int type) { return motion_[static_cast<MOTION_TYPE>(type)].timeActive; }

	// 入力猶予時間
	float GetMotionInput(MOTION_TYPE type) { return motion_[type].timeInput; }
	float GetMotionInput(int type) { return motion_[static_cast<MOTION_TYPE>(type)].timeInput; }

	// モーション終了時間
	float GetMotionEnd(MOTION_TYPE type) { return motion_[type].timeEnd; }
	float GetMotionEnd(int type) { return motion_[static_cast<MOTION_TYPE>(type)].timeEnd; }

	// 攻撃時間
	float GetMotionAtk(MOTION_TYPE type) { return motion_[type].timeTimingActive; }
	float GetMotionAtk(int type) { return motion_[static_cast<MOTION_TYPE>(type)].timeTimingActive; }

	// アニメーション速度
	float GetMotionSpeed(MOTION_TYPE type) { return motion_[type].animationSpeed; }
	float GetMotionSpeed(int type) { return motion_[static_cast<MOTION_TYPE>(type)].animationSpeed; }


	const PlayerParam& GetPlayerParam(void) { return playerParam_; }


private:
	
	PlayerParam playerParam_;

	// アニメーション速度
	float animationSpeedIdle_ = 0.0f;
	float animationSpeedWalk_ = 0.0f;
	float animationSpeedRun_ = 0.0f;


	std::unordered_map<MOTION_TYPE, Motion> motion_;
};