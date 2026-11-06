#pragma once
#include "../ActorBase.h"
#include "../../Common/ActionController.h"
#include <memory>
#include <functional>

class AnimationController;
class ActionController;
class ColliderSphere;

class CharaBase : public ActorBase
{
public:

	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		LINE,
		CAPSULE,
		ATTACK_AREA, // 攻撃範囲
		VIEW_RANGE,  // 索敵範囲
		MAX,
	};

	enum class ACTION_STATE
	{
		NONE = -1,

		IDLE,    // 待機
		WALK,    // 歩行
		DASH,    // ダッシュ
		ACTION,  // 行動
		DEFENSE, // 防御
		DEFEAT,  // 撃破

		MAX
	};

	CharaBase(void);
	virtual ~CharaBase(void) override;

	void Update(void)override final;

	void Release(void)override final;

	virtual void SetDamage(int damageHp, int damageStamina) = 0;

	int GetAttackPower(void)const { return curAttackPower_; };

	int GetHp(void)const { return hp_; };

	float GetTimeInv(void)const { return timeInv_; };

	const std::unique_ptr<AnimationController>& GetAnimation(void)const { return animation_; };

	// 攻撃判定無効化
	void ResetColliderAttackArea(void);

	void SetIsAction(bool isActive) { isAction_ = isActive; }

protected:

	// 防御時のダメージカット倍率
	static constexpr float DEFENSE_CUT_DAMAGE = 0.75f;

	// 無効時間
	static constexpr float DAMAGE_INV_TIME = 1.0f;

	std::unique_ptr<AnimationController> animation_ = nullptr;

	std::unique_ptr<ActionController> actionController_ = nullptr;

	Transform sword_;

	// 行動処理
	std::map<int, std::function<void(void)>> functionActionMap_;
	std::function<void(void)> functionAction_ = nullptr;

	int shadowHandle_ = -1;

	int hp_ = 0;
	int stamina_ = 0;

	float timeInv_ = 0.0f;

	float timeMove_ = 0.0f;

	float curSpeed_ = 0.0f;

	int curAttackPower_ = 0;

	float moveSpeed_ = 0.0f;

	VECTOR moveDir_;

	VECTOR movePow_;

	VECTOR jumpPow_;

	// 移動前の座標
	VECTOR prevPos_;

	// ジャンプ判定
	bool isFall_ = false;

	bool isSwordActive_ = false;

	bool isAction_ = true;
	
	virtual void InitLoad(void)override final;
	virtual void InitLoadPost(void) = 0;
	virtual void InitAnimation(void)override final;
	virtual void InitAnimationPost(void) = 0;
	virtual void InitActionPost(void) {};

	virtual void UpdateProcess(void) = 0;

	virtual void DrawPre(void)override;

	void ResetMove(void);

	// 移動方向に応じた遅延回転
	void DelayRotate(void);

	void CalcGravityPow(void);

	// 衝突判定
	void Collision(void);
	void CollisionGravity(void);
	void CollisionCapsule(void);
	virtual void CollisionReserve(void) {};

	
	void DrawLate(void)override;

	// 影描画
	void DrawShadowRound(void);

	virtual void UpdateSwordParam(void) {};

	/// @brief 攻撃範囲割り当て
	/// @param radius 範囲
	void SetColliderAttackArea(float radius);

	void ChangeUpdateAction(int actionType);

	/// @brief 行動の処理なし
	void ActionNone(void) {};
};