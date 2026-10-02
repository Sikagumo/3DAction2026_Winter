#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "ActorBase.h"

ActorBase::ActorBase(void)
{
}

ActorBase::~ActorBase(void)
{
}

const Transform& ActorBase::GetTransform(void) const
{
	return transform_;
}

void ActorBase::SetLightPos(const std::vector<VECTOR>& _lightsPos, COLOR_F _lightColor, float _lightRadius)
{
	LIGHT_POS = _lightsPos;
	lightColor_ = _lightColor;
	lightRadis_ = _lightRadius;
	AddShaderParamLight();
}
