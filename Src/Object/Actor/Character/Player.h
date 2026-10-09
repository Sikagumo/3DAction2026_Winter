#pragma once
#include "./CharaBase.h"

class Player : public CharaBase
{
public:

	enum class ANIMATION_TYPE
	{
		IDLE,          // 待機
		WALK,          // 歩行
		RUN,           // ダッシュ
		FALL,          // 落下
		ATTACK_LIGHT,  // 弱攻撃
		ATTACK_STRONG, // 強攻撃
		DEFENSE,       // 守備
		DODGE,         // 回避

		MAX,
	};

	enum class ACTION_TYPE
	{
		NONE = -1,
		ATTACK_LIGHT,  // 弱攻撃
		ATTACK_STRONG, // 強攻撃
		DEFENSE,       // 守備
		DODGE,         // 回避

		MAX
	};

	Player(void);
	~Player(void)override = default;


	void ChangeAction(ACTION_TYPE action);

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
	void UpdateProcessPost(void)override;

	void CollisionReserve(void)override;

private:

	bool isDash_ = false;

	float parryTime_ = 0.0f;

	// 操作
	void ProcessMove(void);

	// ジャンプ
	void ProcessJump(void);

	/// @brief 回避
	void ProcessDodge(void);

	/// @brief 守備
	void ProcessDefense(void);
	void Defense(void);

	void AttackLight(void);
	void AttackStrong(void);

	void PlayAnimation(ANIMATION_TYPE type, bool _isLoop = true);
};