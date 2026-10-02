#include "PointLightObj.h"
#include <array>
#include "../Manager/ResourceManager.h"
#include "../Manager/Camera.h"
#include "../Manager/SceneManager.h"
#include "Common/Transform.h"
#include "../Utility/AsoUtility.h"
#include "../Common/Vector2.h"
#include "Player.h"
#include "ActorBase.h"

PointLightObj::PointLightObj(const VECTOR& _pos, float _scale)
	: ActorBase::ActorBase()
	, scale_(_scale)
{
	transform_.pos = _pos;
}

PointLightObj::~PointLightObj(void)
{
}

void PointLightObj::Init(void)
{
	// モデルの基本情報
	transform_.SetModel(
		resMng_.LoadModelDuplicate(ResourceManager::SRC::MOON_PLANET)
	);



	transform_.InitTransform(scale_
		, Quaternion::Identity()
		, Quaternion::Identity());

	material_ = std::make_unique<ModelMaterial>("StdModelVS.cso", 1
		, "PointLightPS.cso", 1);

	// 頂点シェーダ登録
	const VECTOR CAMERA_POS = SceneManager::GetInstance().GetCamera().GetPos();
	material_->AddConstBufVS({ CAMERA_POS.x, CAMERA_POS.y, CAMERA_POS.z });

	//float fogStart = 0.0f, fogEnd = 0.0f;
	//GetFogStartEnd(&fogStart, &fogEnd);
	//material_->AddConstBufVS({ fogStart, fogEnd,  });

	// ピクセルシェーダ登録
	const VECTOR LIGHT_DIR = GetLightDirection();
	material_->SetTextureAddress(ModelMaterial::TEXADDRESS::WRAP);
	material_->AddConstBufPS({ LIGHT_DIR.x, LIGHT_DIR.y, LIGHT_DIR.z  });


	renderer_ = std::make_unique<ModelRenderer>(transform_.modelId, *material_);
}
void PointLightObj::AddShaderParamLight(void)
{
	/*
	material_->AddConstBufVS({ lightColor_.r,lightColor_.g,lightColor_.b });
	for (auto& pos : LIGHT_POS)
	{
		material_->AddConstBufVS({ pos.x, pos.y, pos.z});
	}*/
}

void PointLightObj::Update(void)
{
	transform_.Update();

	const VECTOR CAMERA_POS = SceneManager::GetInstance().GetCamera().GetPos();
	material_->SetConstBufVS(0, { CAMERA_POS.x, CAMERA_POS.y, CAMERA_POS.z, lightRadis_ });
}

void PointLightObj::Draw(void)
{
	renderer_->Draw();
}
