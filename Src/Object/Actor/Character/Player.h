#pragma once
#include "./CharaBase.h"
#include <unordered_map>
#include "../../Status/StatusPlayer.h"

class Player : public CharaBase
{
public:

	enum class ANIMATION_TYPE
	{
		IDLE,          // 待機
		WALK,          // 歩行
		RUN,           // ダッシュ
		ATTACK_LIGHT,  // 弱攻撃
		ATTACK_HEAVY, // 強攻撃
		DEFENSE,       // 守備
		DODGE,         // 回避

		MAX,
	};

	Player(void);
	~Player(void)override = default;

	void DrawDebug(void)override;

	void ChangeAction(StatusPlayer::ACTION_TYPE action);


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

	// 行動の種類
	using ACTION_TYPE = StatusPlayer::ACTION_TYPE;

	const StatusPlayer::PlayerParam& status_;
	const std::unordered_map<StatusPlayer::ACTION_TYPE, StatusPlayer::ActionParam>& actionStatus_;

	int hp_ = 0;

	float moveTime_ = 0.0f;

	float curSpeed_ = 0.0f;

	bool isDash_ = false;

	float parryTime_ = 0.0f;


	// 操作
	void ProcessMove(void);
	void ResetMove(void);

	/// @brief 回避
	void ProcessDodge(void);
	void Dodge(void);

	/// @brief 守備
	void ProcessDefense(void);
	void Defense(void);

	/// @brief 弱攻撃
	void ProcessAttackLight(void);
	void AttackLight(void);

	/// @brief 強攻撃
	void ProcessAttackHeavy(void);
	void AttackHeavy(void);

	void PlayAnimation(ANIMATION_TYPE type, bool _isLoop = true);
};