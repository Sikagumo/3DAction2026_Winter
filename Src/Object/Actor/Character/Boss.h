#pragma once
#include "./CharaBase.h"
#include <unordered_map>
#include "../../Status/StatusEnemy.h"
#include "../../Status/StatusBoss.h"
class CharaBase;
class StatusEnemy;

class Boss : public CharaBase
{
public:

	enum class ANIMATION_TYPE
	{
		STAND_UP,     // 立ち上がり
		IDLE,         // 待機
		WALK,         // 歩行
		ATTACK_LIGHT, // 弱攻撃
		ATTACK_HEAVY, // 強攻撃
		DEFENSE,      // 守備
		DAMAGE,       // 被ダメージ
		DEFEAT,       // 撃破

		MAX,
	};

	Boss(void);
	~Boss(void)override = default;

	void DrawDebug(void)override;

	/// @brief 被ダメージ処理
	void SetDamage(int damageHp, int damageStamina)override;


protected:

	/// @brief 初期化処理
	void InitLoadPost(void)override;
	void InitTransform(void)override;
	void InitCollider(void)override;
	void InitAnimationPost(void)override;
	void InitActionPost(void)override;
	void InitPost(void)override;

	/// @brief 更新処理
	void UpdateProcess(void)override;

	void CollisionReserve(void)override;


private:

	// 行動の種類
	using ACTION_TYPE = StatusBoss::ACTION_TYPE;

	const StatusEnemy::EnemyParam& status_;
	const std::unordered_map<StatusBoss::ACTION_TYPE, StatusBoss::ActionParam>& actionStatus_;

	int curActionNum_ = -1;


	// 操作
	void ProcessMove(void);

	void AttackLight(void);
	void AttackHeavy(void);

	void PlayAnimation(ANIMATION_TYPE type, bool isLoop = true);
};