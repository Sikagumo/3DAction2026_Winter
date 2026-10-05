#pragma once
#include "Common/Transform.h"
#include <vector>
class ResourceManager;
class SceneManager;

class ActorBase
{

public:

	ActorBase(void);

	virtual ~ActorBase(void);

	virtual void Initialize(void)   = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void)   = 0;

	const Transform& GetTransform(void) const;

	void SetLightPos(const std::vector<VECTOR>& _lightsPos, COLOR_F _lightColor, float _lightRadius);


protected:

	// ÉÇÉfÉãêßå‰ÇÃäÓñ{èÓïÒ
	Transform transform_;

	COLOR_F lightColor_;
	float lightRadis_;
	std::vector<VECTOR> LIGHT_POS;

	virtual void AddShaderParamLight(void) {};
};
