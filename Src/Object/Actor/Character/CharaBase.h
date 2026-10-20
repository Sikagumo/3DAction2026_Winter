#pragma once
#include "../ActorBase.h"
#include <memory>

class AnimationController;
class ActionController;

class CharaBase : public ActorBase
{
public:

	// Õ“Ë”»’èí•Ê
	enum class COLLIDER_TYPE
	{
		LINE,
		CAPSULE,
		VIEW_RANGE,
		MAX,
	};


	CharaBase(void);
	virtual ~CharaBase(void) override = default;

	void Update(void)override final;

	void Release(void)override;


protected:

	std::unique_ptr<AnimationController> animation_ = nullptr;

	std::unique_ptr<ActionController> actionController_ = nullptr;

	int shadowHandle_ = -1;

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
	virtual void UpdateProcessPost(void) = 0;

	virtual void DrawPre(void)override;

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
};