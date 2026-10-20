#pragma once
#include <string>
#include <array>
#include <unordered_map>
#include "../../Utility/UtilityCommon.h"


class StatusEnemy
{
public:

	// 敵の種類
	enum class TYPE
	{
		NONE = 0,
		SKELETON,
		SKELETON_ELITE,
		SKELETON_WARRIOR,
		SKELETON_BOSS,
		MAX,
	};

	// 敵保存データ
	enum class PARAM
	{
		NAME,            // 敵名
		HANDLE_PATH,	 // ハンドルパス
		ENEMY_TYPE,		 // 敵の種類
		MODEL_SCALE,			 // モデルのスケール
		RADIUS,          // 半径
		HP_MAX,              // HP
		POWER,           // 攻撃力
		ANIMATION_SPEED,			 // 移動速度
		SPEED_WALK,       // 移動時の加速度
		ATTACK_INTERVAL, // 攻撃間隔
		ATTACK_RANEGE,   // 攻撃範囲
		SEARCH_RANGE,    // 索敵範囲

		ANIM_SPEED_SPAWN,  // 生成時のアニメーション速度
		ANIM_SPEED_IDLE,   // 待機アニメーション速度
		ANIM_SPEED_ATTACK, // 攻撃アニメーション速度
		ANIM_SPEED_WALK,   // 移動アニメーション速度
		ANIM_SPEED_HIT_1,  // 弱被ダメージアニメーション速度
		ANIM_SPEED_HIT_2,  // 強被ダメージアニメーション速度
		ANIM_SPEED_DEATH,

		MAX,
	};

	enum class ANIM_TYPE
	{
		IDLE,
		ATTACK,
		WALK,
		SPAWN,
		HIT_1,
		HIT_2,
		DEATH,

		MAX,
	};


	StatusEnemy(void);

	~StatusEnemy(void) = default;


	void LoadParam(const std::array<std::string, static_cast<int>(PARAM::MAX)>& _loadString)
	{
		name_ = _loadString[static_cast<int>(PARAM::NAME)];
		handlePath_ = _loadString[static_cast<int>(PARAM::HANDLE_PATH)];
		SetType(_loadString[static_cast<int>(PARAM::ENEMY_TYPE)]);

		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::MODEL_SCALE)], scale_, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::HP_MAX)], hp, 1);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::POWER)], power_, 0);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::RADIUS)], radius_, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIMATION_SPEED)], speedMax_, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::SPEED_WALK)], speedAcc_, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ATTACK_INTERVAL)], atkInterval_, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ATTACK_RANEGE)], atkRange_, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::SEARCH_RANGE)], searchRange_, 0.0f);

		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_IDLE)],
									animationSpeed_[static_cast<int>(ANIM_TYPE::IDLE)], 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_ATTACK)],
									animationSpeed_[static_cast<int>(ANIM_TYPE::ATTACK)], 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_WALK)],
									animationSpeed_[static_cast<int>(ANIM_TYPE::WALK)], 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_SPAWN)],
									animationSpeed_[static_cast<int>(ANIM_TYPE::SPAWN)], 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_HIT_1)],
									animationSpeed_[static_cast<int>(ANIM_TYPE::HIT_1)], 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_HIT_2)],
									animationSpeed_[static_cast<int>(ANIM_TYPE::HIT_2)], 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_DEATH)],
									animationSpeed_[static_cast<int>(ANIM_TYPE::DEATH)], 0.0f);
	}


	std::string& GetName(void) { return name_; }

	std::string& GetHandlePath(void) { return handlePath_; }

	TYPE& GetEnemyType(void) { return type_; };

	float GetScale(void)const { return scale_; };

	float GetRadius(void)const { return radius_; };

	int GetMaxHp(void)const { return hp; };

	int GetPower(void)const { return power_; };

	float GetSpeed(void)const { return speedMax_; };

	float GetSpeedAcc(void)const { return speedAcc_; }

	float GetAtkInterval(void)const { return atkInterval_; }

	float GetAtkRange(void)const { return atkRange_; }

	float GetSearchRange(void)const { return searchRange_; }


	// アニメーション速度取得
	float GetAnimationSpeed(ANIM_TYPE type) { return animationSpeed_[static_cast<int>(type)]; };
	float GetAnimationSpeed(int type) { return animationSpeed_[type]; };


private:

	// 敵の種類名
	const std::unordered_map<TYPE, std::string> TYPE_STRING =
	{
		{TYPE::SKELETON, "SKELETON"},
		{TYPE::SKELETON_ELITE, "SKELETON_ELITE"},
		{TYPE::SKELETON_WARRIOR, "SKELETON_WARRIOR"},
		{TYPE::SKELETON_BOSS, "SKELETON_BOSS"}
	};


	std::string name_ = "";

	std::string handlePath_ = "";

	TYPE type_;

	float scale_ = 0.0f;

	float radius_ = 0.0f;

	int hp = 0;

	int power_ = 0;

	// 移動速度
	float speedMax_ = 0.0f;

	// 移動上昇値
	float speedAcc_ = 0.0f;

	float atkInterval_ = 0.0f;

	float atkRange_ = 0.0f;

	float searchRange_ = 0.0f;

	float animationSpeed_[static_cast<int>(ANIM_TYPE::MAX)];

	void SetType(const std::string& text);
};