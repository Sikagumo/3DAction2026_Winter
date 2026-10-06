#pragma once
#include <map>
#include <functional>
#include "../Common/Transform.h"
#include "ActorBase.h"

class SkyDome : public ActorBase
{
public:

	// 状態
	enum class STATE
	{
		NONE,
		STAY,
		FOLLOW
	};

	
	SkyDome(const Transform& syncTransform);
	~SkyDome(void)override = default;

	void Update(void) override;
	void Draw(void) override;


private:

	// 自機の情報
	const Transform& syncTransform_;

	// 状態
	STATE state_;

	// 状態管理(状態遷移時初期処理)
	std::map<STATE, std::function<void(void)>> stateChanges_;

	// 状態管理(更新ステップ)
	std::function<void(void)> stateUpdate_;

	void InitLoad(void)override;
	void InitTransform(void)override;
	void InitCollider(void)override {};
	void InitAnimation(void)override {};
	void InitPost(void)override;

	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStateStay(void);
	void ChangeStateFollow(void);

	// 更新ステップ
	void UpdateNone(void);
	void UpdateStay(void);
	void UpdateFollow(void);
};
