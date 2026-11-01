#pragma once
#include "./CharaBase.h"
#include <unordered_map>
#include "../../Status/StatusPlayer.h"

class Player : public CharaBase
{
public:

	enum class ANIMATION_TYPE
	{
		NONE = -1, // 無効状態

		IDLE,          // 待機
		WALK,          // 歩行
		RUN,           // ダッシュ
		DRAWING_SWORD,  // 弱攻撃
		ATTACK_LIGHT,  // 弱攻撃
		ATTACK_HEAVY, // 強攻撃
		DEFENSE,       // 守備
		DODGE,         // 回避
		DAMAGE,        // 被ダメージ
		DEFEAT,        // 撃破

		MAX,
	};

	enum class ATTACK_TYPE
	{
		NONE = -1, // 無効状態
		LIGHT1, // 弱攻撃
		HEAVY, // 強攻撃
	};

	Player(void);
	~Player(void)override = default;

	void DrawDebug(void)override;

	void ChangeAction(StatusPlayer::ACTION_TYPE action);

	/// @brief 被ダメージ処理
	void SetDamage(int damageHp, int damageStamina)override;

	void SetIsControl(bool isControl) { isControl_ = isControl; };
	bool GetIsControl(void)const { return isControl_; }


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

	void DrawPre(void)override;

	void CollisionReserve(void)override;

	void UpdateSwordParam(void)override;


private:

	// 行動の種類
	using ACTION_TYPE = StatusPlayer::ACTION_TYPE;

	const StatusPlayer::PlayerParam& status_;
	const std::unordered_map<StatusPlayer::ACTION_TYPE, StatusPlayer::ActionParam>& actionStatus_;
	static constexpr float TIME_DRAWING_SWORD = 15.0f;

	// 攻撃の種類
	ATTACK_TYPE attackType_ = ATTACK_TYPE::NONE;

	bool isDash_ = false;

	// 抜刀中か否か
	bool isDrawingSward_ = false;

	// 抜刀中の現在時間
	float timeDrawingSward_ = 0.0f;

	bool isControl_ = false;

	float parryTime_ = 0.0f;


	// 操作
	void ProcessMove(void);

	/// @brief 回避
	void ProcessDodge(void);
	void ActionDodge(void);

	/// @brief 守備
	void ProcessDefense(void);
	void ActionDefense(void);

	/// @brief 弱攻撃
	void ProcessAttackLight(void);
	void ActionAttackLight(void);

	/// @brief 強攻撃
	void ProcessAttackHeavy(void);
	void ActionAttackHeavy(void);

	/// @brief 抜刀
	void ActionDrawingSword(void);

	/// @brief アニメーション再生
	/// @param type アニメーションの種類
	/// @param isLoop ループ再生するか否か
	/// @param isBlendAnimation ブレンドアニメーションをするか否か
	void PlayAnimation(ANIMATION_TYPE type, bool isLoop = true, bool isBlendAnimation = true);

	ACTION_TYPE GetActionType(void);
};