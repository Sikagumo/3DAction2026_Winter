#pragma once
#include "CharaBase.h"
#include "../../Status/StatusEnemy.h"
class CharaBase;
class StatsEnemy;
class Player;

class EnemyBase : public CharaBase
{
public:

	EnemyBase(const std::unique_ptr<Player>& player, StatusEnemy::ENEMY_TYPE enemyType);

	virtual ~EnemyBase(void) = default;

	void SetDamage(int damageHp, int damageStamina)override = 0;


protected:

	// 移動する範囲
	static constexpr float ACTIVE_MOVE_RANGE = 100.0f;

	const std::unique_ptr<Player>& player_;
	const StatusEnemy::EnemyParam& status_;

	int curActionNum_ = -1;

	// 行動時間
	float actionTime_ = 0.0f;

	virtual void InitLoadPost(void)override = 0;
	virtual void InitAnimationPost(void)override = 0;
	virtual void InitActionPost(void)override = 0;

	virtual void UpdateProcess(void)override final;
	virtual void UpdateProcessPost(void) = 0;

	virtual void DrawLate(void)override;

	void SearchField(void);

	/// @brief プレイヤーの方向を追尾
	void LookPlayerPos(void);

	void UpdateSwordParam(void)override;


private:

	bool isMove_ = false;
};