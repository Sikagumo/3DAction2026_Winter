#pragma once
#include "./CharaBase.h"

class Player : public CharaBase
{
public:

	enum class ANIMATION_TYPE
	{
		IDLE, // 待機
		WALK, // 歩行
		RUN,  // ダッシュ
		FALL, // 落下

		MAX,
	};

	Player(void);
	~Player(void)override = default;


protected:

	/// @brief 初期化処理
	void InitLoadPost(void)override;
	void InitTransform(void)override;
	void InitCollider(void)override;
	void InitAnimationPost(void)override;
	void InitPost(void)override;

	/// @brief 更新処理
	void UpdateProcess(void)override;
	void UpdateProcessPost(void)override;

	void CollisionReserve(void)override;


private:

	bool isDash_;


	// 操作
	void ProcessMove(void);

	// ジャンプ
	void ProcessJump(void);

	void PlayAnimation(ANIMATION_TYPE type, bool _isLoop = true);
};