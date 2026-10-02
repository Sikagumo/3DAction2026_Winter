#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "Common/Transform.h"
#include "WarpStar.h"
#include "Planet.h"
#include "../Manager/Camera.h"

Planet::Planet(const Stage::NAME& name, const TYPE& type, const Transform& transform)
{

	name_ = name;
	type_ = type;
	transform_ = transform;

	gravityPow_ = 0.0f;
	gravityRadius_ = 0.0f;
	deadLength_ = 0.0f;

}

Planet::~Planet(void)
{
}

void Planet::Init(void)
{
	gravityPow_ = DEFAULT_GRAVITY_POW;
	gravityRadius_ = DEFAULT_GRAVITY_RADIUS;
	deadLength_ = DEFAULT_DEAD_LENGTH;

	material_ = std::make_unique<ModelMaterial>("StdModelVS.cso", 33,
												"StdModelPS.cso", 2);

	const COLOR_F COLOR_AMBIENT = COLOR_F(0.2f, 0.2f, 0.2f, 1.0f);
	const VECTOR LIGHT_DIR = GetLightDirection();

	material_->AddConstBufPS({ LIGHT_DIR.x, LIGHT_DIR.y, LIGHT_DIR.z });
	material_->AddConstBufPS({ COLOR_AMBIENT.r, COLOR_AMBIENT.g, COLOR_AMBIENT.b, COLOR_AMBIENT.a });

	const VECTOR CAMERA_POS = SceneManager::GetInstance().GetCamera().GetPos();
	float fogStart = 0.0f, fogEnd = 0.0f;
	GetFogStartEnd(&fogStart, &fogEnd);

	material_->AddConstBufVS({ CAMERA_POS.x, CAMERA_POS.y, CAMERA_POS.z });
	material_->AddConstBufVS({ fogStart, fogEnd, 33 });
	material_->AddConstBufVS({ 0.0f, 100.0f, 1000.0f, 0.0f });


	renderer_ = std::make_unique<ModelRenderer>(transform_.modelId, *material_);
}

void Planet::Update(void)
{
	const VECTOR CAMERA_POS = SceneManager::GetInstance().GetCamera().GetPos();
	material_->SetConstBufVS(0, { CAMERA_POS.x, CAMERA_POS.y, CAMERA_POS.z});
}

void Planet::Draw(void)
{
	renderer_->Draw();
}

void Planet::SetPosition(const VECTOR& pos)
{
    transform_.pos = pos;
    transform_.Update();
}

void Planet::SetRotation(const Quaternion& rot)
{
	transform_.quaRot = rot;
	transform_.Update();
}

float Planet::GetGravityPow(void) const
{
	return gravityPow_;
}

void Planet::SetGravityPow(float pow)
{
	gravityPow_ = pow;
}

float Planet::GetGravityRadius(void) const
{
	return gravityRadius_;
}

void Planet::SetGravityRadius(float radius)
{
	gravityRadius_ = radius;
}

const Planet::TYPE& Planet::GetType(void) const
{
	return type_;
}

bool Planet::InRangeGravity(const VECTOR& pos) const
{
	return false;
}

bool Planet::InRangeDead(const VECTOR& pos) const
{
	return false;
}

void Planet::SetDeadLength(float len)
{
	deadLength_ = len;
}

void Planet::AddShaderParamLight(void)
{
	material_->AddConstBufVS({ lightColor_.r,lightColor_.g,lightColor_.b });
	for (auto& pos : LIGHT_POS)
	{
		material_->AddConstBufVS({ pos.x, pos.y, pos.z, lightRadis_ });
	}
}

const Stage::NAME& Planet::GetName(void) const
{
	return name_;
}
