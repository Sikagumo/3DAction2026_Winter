#include <DxLib.h>
#include "../Manager/ResourceManager.h"
#include "../Manager/Camera.h"
#include "../Manager/SceneManager.h"
#include "Common/Transform.h"
#include "../Utility/AsoUtility.h"
#include "RimPlanet.h"

RimPlanet::RimPlanet(void)
	: material_(nullptr), renderer_(nullptr)
{

}

RimPlanet::~RimPlanet(void)
{
}

void RimPlanet::Init(void)
{

	// ÉÇÉfÉãÇÃäÓñ{èÓïÒ
	transform_.SetModel(
		resMng_.LoadModelDuplicate(
			ResourceManager::SRC::MOON_PLANET)
	);



	transform_.InitTransform(1.0f
		, Quaternion::Identity()
		, Quaternion::Identity()
		, VGet(0.0f, 0.0f, 0.0f));

	material_ = std::make_unique<ModelMaterial>("StdModelVS.cso", 33
		, "StdModelPS_RimLight.cso", 5);

	const VECTOR& LIGHT_DIR = GetLightDirection();
	const VECTOR& CAMERA_POS = SceneManager::GetInstance().GetCamera().GetPos();

	
	float fogStart = 0.0f, fogEnd = 0.0f;
	GetFogStartEnd(&fogStart, &fogEnd);

	material_->AddConstBufVS({ CAMERA_POS.x, CAMERA_POS.y, CAMERA_POS.z });
	material_->AddConstBufVS({ fogStart, fogEnd, 30 });

	constexpr COLOR_F COLOR_DIFFUSE = { 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr COLOR_F COLOR_AMBIENT = { 0.1f, 0.1f, 0.1f, 0.5f };
	constexpr COLOR_F RIM_COLOR = { 1.0f, 1.0f, 0.0f, 0.85f };
	constexpr float RIM_POW = 2.0f;

	material_->AddConstBufPS({ LIGHT_DIR.x, LIGHT_DIR.y, LIGHT_DIR.z });
	material_->AddConstBufPS({ COLOR_AMBIENT.r, COLOR_AMBIENT.g, COLOR_AMBIENT.b, COLOR_AMBIENT.a });
	material_->AddConstBufPS({ COLOR_DIFFUSE.r, COLOR_DIFFUSE.g, COLOR_DIFFUSE.b, COLOR_DIFFUSE.a });
	material_->AddConstBufPS({ RIM_COLOR.r, RIM_COLOR.g, RIM_COLOR.b, RIM_POW});
	material_->AddConstBufPS({ CAMERA_POS.x, CAMERA_POS.y, CAMERA_POS.z });

	renderer_ = std::make_unique<ModelRenderer>(transform_.modelId, *material_);
}

void RimPlanet::Update(void)
{
	transform_.Update();

	const VECTOR& CAMERA_POS = SceneManager::GetInstance().GetCamera().GetPos();
	material_->SetConstBufPS(4, { CAMERA_POS.x, CAMERA_POS.y, CAMERA_POS.z });

	material_->SetConstBufVS(0, { CAMERA_POS.x, CAMERA_POS.y, CAMERA_POS.z });
}

void RimPlanet::Draw(void)
{
	renderer_->Draw();
}

void RimPlanet::AddShaderParamLight(void)
{
	material_->AddConstBufVS({ lightColor_.r,lightColor_.g,lightColor_.b });
	for (auto& pos : LIGHT_POS)
	{
		material_->AddConstBufVS({ pos.x, pos.y, pos.z, lightRadis_ });
	}
}