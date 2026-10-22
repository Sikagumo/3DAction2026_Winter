#pragma once
#include <string>
#include <array>
#include <unordered_map>
#include "../../Utility/UtilityCommon.h"


class StatusEnemy
{
public:

	// 敵の種類
	enum class ENEMY_TYPE
	{
		NONE = 0,
		BOSS,
		MAX,
	};

	// 敵保存データ
	enum class PARAM
	{
		NAME,            // 敵名
		HANDLE_PATH,	 // ハンドルパス
		ENEMY_TYPE,		 // 敵の種類
		MODEL_SCALE,     // モデルのスケール
		RADIUS,          // 半径
		HP_MAX,          // HP
		STAMINA,         // 体幹
		POWER,           // 攻撃力
		SPEED_INIT,      // 初期移動速度
		SPEED_MOVE,      // 移動速度
		SPEED_ACC,       // 移動時の加速度
		ATTACK_INTERVAL, // 攻撃間隔
		ATTACK_RANEGE,   // 攻撃範囲
		SEARCH_RANGE,    // 索敵範囲

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

	struct ActionParam
	{
		std::string actionName = "";

		float timeActive = 0.0f;

		float timeEnd = 0.0f;

		float timeTimingActive = 0.0f;

		float animationSpeed = 0.0f;

		float attackRadius = 0.0f;
	};

	struct EnemyParam
	{
		std::string name = "";

		std::string handlePath = "";

		ENEMY_TYPE type = ENEMY_TYPE::NONE;

		// モデルのスケール
		float scale = 0.0f;

		// 当たり判定の半径
		float radius = 0.0f;

		int hp = 0;

		int stamina = 0;

		int power = 0;

		// 移動速度の初速
		float speedInit = 0.0f;

		// 移動速度
		float speedMax = 0.0f;

		// 移動上昇値
		float speedAcc = 0.0f;

		float atkInterval = 0.0f;

		float atkRange = 0.0f;

		float searchRange = 0.0f;

		std::array<float, static_cast<size_t>(ANIM_TYPE::MAX)> animationSpeed;
	};


	StatusEnemy(void);

	virtual ~StatusEnemy(void) = default;


	void LoadStatusParam(const std::array<std::string, static_cast<int>(PARAM::MAX)>& _loadString)
	{
		enemyParam_.name = _loadString.at(static_cast<int>(PARAM::NAME));
		enemyParam_.handlePath = _loadString[static_cast<int>(PARAM::HANDLE_PATH)];
		SetType(_loadString[static_cast<int>(PARAM::ENEMY_TYPE)]);

		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::MODEL_SCALE)], enemyParam_.scale, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::RADIUS)], enemyParam_.radius, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::HP_MAX)], enemyParam_.hp, 1);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::STAMINA)], enemyParam_.stamina, 0);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::POWER)], enemyParam_.power, 0);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::SPEED_INIT)], enemyParam_.speedInit, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::SPEED_MOVE)], enemyParam_.speedMax, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::SPEED_ACC)], enemyParam_.speedAcc, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ATTACK_INTERVAL)], enemyParam_.atkInterval, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ATTACK_RANEGE)], enemyParam_.atkRange, 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::SEARCH_RANGE)], enemyParam_.searchRange, 0.0f);

		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_IDLE)],
			enemyParam_.animationSpeed[static_cast<int>(ANIM_TYPE::IDLE)], 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_ATTACK)],
			enemyParam_.animationSpeed[static_cast<int>(ANIM_TYPE::ATTACK)], 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_WALK)],
			enemyParam_.animationSpeed[static_cast<int>(ANIM_TYPE::WALK)], 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_HIT_1)],
			enemyParam_.animationSpeed[static_cast<int>(ANIM_TYPE::HIT_1)], 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_HIT_2)],
			enemyParam_.animationSpeed[static_cast<int>(ANIM_TYPE::HIT_2)], 0.0f);
		UtilityCommon::ChangeString(_loadString[static_cast<int>(PARAM::ANIM_SPEED_DEATH)],
			enemyParam_.animationSpeed[static_cast<int>(ANIM_TYPE::DEATH)], 0.0f);
	}

	const EnemyParam& GetEnemyParam(void)const { return enemyParam_; }


	// アニメーション速度取得
	float GetAnimationSpeed(ANIM_TYPE type) { return enemyParam_.animationSpeed[static_cast<int>(type)]; };
	float GetAnimationSpeed(int type) { return enemyParam_.animationSpeed[type]; };


private:

	// 敵の種類名
	const std::unordered_map<ENEMY_TYPE, std::string> TYPE_STRING =
	{
		{ ENEMY_TYPE::BOSS, "BOSS"}
	};

	EnemyParam enemyParam_;



	void SetType(const std::string& text);
};