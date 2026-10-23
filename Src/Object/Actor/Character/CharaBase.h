#pragma once
#include "../ActorBase.h"
#include <memory>
#include <functional>

class AnimationController;
class ActionController;
class ColliderSphere;

class CharaBase : public ActorBase
{
public:

	// Õ“Ë”»’èí•Ê
	enum class COLLIDER_TYPE
	{
		LINE,
		CAPSULE,
		ATTACK_AREA, // UŒ‚”ÍˆÍ
		VIEW_RANGE,  // õ“G”ÍˆÍ
		MAX,
	};


	CharaBase(void);
	virtual ~CharaBase(void) override = default;

	void Update(void)override final;

	void Release(void)override;

	virtual void SetDamage(int damageHp, int damageStamina) = 0;

	int GetAttackPower(void) { return curAttackPower_; };

	int GetHp(void)const { return hp_; };

	float GetTimeInv(void)const { return timeInv_; };

	const std::unique_ptr<AnimationController>& GetAnimation(void)const { return animation_; };


protected:

	// –hŒä‚Ìƒ_ƒ[ƒWƒJƒbƒg”{—¦
	static constexpr float DEFENSE_CUT_DAMAGE = 0.75f;

	// –³ŒøŠÔ
	static constexpr float DAMAGE_INV_TIME = 1.0f;


	std::unique_ptr<AnimationController> animation_ = nullptr;

	std::unique_ptr<ActionController> actionController_ = nullptr;

	Transform sward_;

	// s“®ˆ—
	std::map<int, std::function<void(void)>> functionActionMap_;
	std::function<void(void)> functionAction_ = nullptr;

	int shadowHandle_ = -1;

	int hp_ = 0;
	int stamina_ = 0;

	float timeInv_ = 0.0f;

	float TimeMove_ = 0.0f;

	float curSpeed_ = 0.0f;

	int curAttackPower_ = 0;

	float moveSpeed_ = 0.0f;

	VECTOR moveDir_;

	VECTOR movePow_;

	VECTOR jumpPow_;

	// ˆÚ“®‘O‚ÌÀ•W
	VECTOR prevPos_;

	// ƒWƒƒƒ“ƒv”»’è
	bool isFall_ = false;

	
	virtual void InitLoad(void)override final;
	virtual void InitLoadPost(void) = 0;
	virtual void InitAnimation(void)override final;
	virtual void InitAnimationPost(void) = 0;
	virtual void InitActionPost(void) {};

	virtual void UpdateProcess(void) = 0;

	virtual void DrawPre(void)override;

	void ResetMove(void);

	// ˆÚ“®•ûŒü‚É‰‚¶‚½’x‰„‰ñ“]
	void DelayRotate(void);

	void CalcGravityPow(void);

	// Õ“Ë”»’è
	void Collision(void);
	void CollisionGravity(void);
	void CollisionCapsule(void);
	virtual void CollisionReserve(void) {};

	
	void DrawLate(void)override;

	// ‰e•`‰æ
	void DrawShadowRound(void);

	virtual void UpdateSwardParam(void) {};

	/// @brief UŒ‚”ÍˆÍŠ„‚è“–‚Ä
	/// @param _radius ”ÍˆÍ
	void SetColliderAttackArea(float _radius);
	void ResetColliderAttackArea(void);

	void ChangeUpdateAction(int actionType);
};