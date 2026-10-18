#pragma once
#include <memory>
#include <map>
#include "./ActorBase.h"
class ResourceManager;
class Player;

class Stage : public ActorBase
{
public:

	// ステージの切り替え間隔
	static constexpr float TIME_STAGE_CHANGE = 1.0f;


	// ステージ名
	enum class NAME
	{
		MAIN_PLANET,
		FALL_PLANET,
		FLAT_PLANET_BASE,
		FLAT_PLANET_ROT01,
		FLAT_PLANET_ROT02,
		FLAT_PLANET_ROT03,
		FLAT_PLANET_ROT04,
		FLAT_PLANET_FIXED01,
		FLAT_PLANET_FIXED02,
		PLANET10,
		LAST_STAGE,
		SPECIAL_STAGE
	};

	Stage(std::unique_ptr<Player>& player);
	~Stage(void)override = default;

	void Update(void)override;


protected:

	void InitLoad(void)override;
	void InitTransform(void)override;
	void InitCollider(void)override;
	void InitAnimation(void)override;
	void InitPost(void)override;

private:

	std::unique_ptr<Player>& player_;

	Transform collisionTransform_;

	void DrawLate(void)override;
};
