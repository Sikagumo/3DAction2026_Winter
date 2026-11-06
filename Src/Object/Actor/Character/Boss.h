#pragma once
#include "EnemyBase.h"
#include "../../Status/StatusBoss.h"
#include <unordered_map>
#include <memory>
class EnemyBase;
class StatusBoss;
class Player;

class Boss : public EnemyBase
{
public:

	enum class ANIMATION_TYPE
	{
		NONE = -1, // 無効状態

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

	Boss(const std::unique_ptr<Player>& player);
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
	void UpdateProcessPost(void)override;

	void CollisionReserve(void)override;

	void DrawPre(void)override;


private:

	// 行動の種類
	using ACTION_TYPE = StatusBoss::ACTION_TYPE;

	static constexpr float TIME_ACTION_INTERVAL = 5.0f;

	const std::unordered_map<StatusBoss::ACTION_TYPE, StatusBoss::ActionParam>& actionStatus_;

	ACTION_TYPE nextActionType_;

	// 操作
	void ProcessMove(void);

	void AttackLight(void);
	void AttackHeavy(void);

	/// @brief アニメーション再生
	/// @param type アニメーションの種類
	/// @param isLoop ループ再生するか否か
	/// @param isReStart 再スタートするか否か
	/// @param isReversePlay 逆再生するか否か
	void PlayAnimation(ANIMATION_TYPE type, bool isLoop = true, bool isReStart = false, bool isReversePlay = false);

	ANIMATION_TYPE GetAnimationType(void)const;

	void ActiveAction(ACTION_TYPE type);

	ACTION_TYPE GetActionType(void)const;
};