#include <DxLib.h>
#include "../Manager/ResourceManager.h"
#include "Common/Transform.h"
//#include "../Renderer/ModelMaterial.h"
//#include "../Renderer/ModelRenderer.h"
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "Model.h"

namespace {
	const COLOR_F POW_SPECULAR = { 30.0f, 30.0f, 30.0f, 30.0f };
	const COLOR_F COLOR_DIFF = { 0.8f, 0.8f, 0.8f, 1.0f };
	const COLOR_F COLOR_SPECULAR = { 1.0f, 1.0f, 1.0f, 1.0f };
	const COLOR_F COLOR_AMBIENT = { 0.0f, 0.0f, 0.0f, 1.0f };

	const FLOAT2 VS_SCALE = { 4.0f, 4.0f };
	const float FREQUENCY = 5.0f;
	const float AMPLITUDE = 5.0f;
	const float WAVE_SPEED = 1.0f;
}

Model::Model(const Model::NAME& name, const Transform& transform)
{
	name_ = name;
	transform_ = transform;
	taim_ = 0.0f;
}

Model::~Model(void)
{
}

void Model::Init(void)
{
	/*
	if(name_ == NAME::SHIELD)
	{
		
		// ポストエフェクト用(ビネット)
		modelMaterial_ = std::make_unique<ModelMaterial>(
			"FohnReflexVS.cso", 0,
			"FohnReflexPS.cso", 6
		);

		//ディレクショナルライトの方向を取得
		auto dir = GetLightDirection();
		auto& camera = SceneManager::GetInstance().GetCamera();
		VECTOR cameraPos = camera.GetPos();
		modelMaterial_->AddConstBufPS({ dir.x,dir.y,dir.z, 0.0f });
		modelMaterial_->AddConstBufPS({ cameraPos.x,cameraPos.y,cameraPos.z, 0.0f });
		modelMaterial_->AddConstBufPS({ COLOR_DIFF.r, COLOR_DIFF.g, COLOR_DIFF.b, COLOR_DIFF.a });
		modelMaterial_->AddConstBufPS({ COLOR_AMBIENT.r, COLOR_AMBIENT.g, COLOR_AMBIENT.b, COLOR_AMBIENT.a });
		modelMaterial_->AddConstBufPS({ COLOR_SPECULAR.r, COLOR_SPECULAR.g, COLOR_SPECULAR.b, COLOR_SPECULAR.a });
		modelMaterial_->AddConstBufPS({ POW_SPECULAR.r, POW_SPECULAR.g, POW_SPECULAR.b, POW_SPECULAR.a });
	}

	if (name_ == NAME::WATER) 
	{
		// ポストエフェクト用(ビネット)
		modelMaterial_ = std::make_unique<ModelMaterial>(
			"WaterWaveVS.cso", 2,
			"WaterWavePS.cso", 1
		);

		modelMaterial_->AddConstBufVS({ VS_SCALE.u, VS_SCALE.v, FREQUENCY, AMPLITUDE });
		modelMaterial_->AddConstBufVS({ WAVE_SPEED, taim_ });
		modelMaterial_->AddConstBufPS({ taim_});
	}

	if (name_ == NAME::GATE) 
	{
		// 頂点シェーダー
		modelMaterial_ = std::make_unique<ModelMaterial>(
			"NonTextModelVS.cso", 0,
			"NonTextModelPS.cso", 1
		);
		modelMaterial_->AddConstBufPS({ 0.0f, 0.05f, 0.0f,1.0f });
	}

	if (name_ == NAME::GATE_MIST)
	{
		// ポストエフェクト用(ビネット)
		modelMaterial_ = std::make_unique<ModelMaterial>(
			"GateMiseVP.cso", 0,
			"GateMisePS.cso", 1
		);

		modelMaterial_->AddConstBufPS({ taim_ });
		text = ResourceManager::GetInstance().Load(ResourceManager::SRC::GATE_IMG).handleId_;
		modelMaterial_->SetTextureBuf(1, text);
	}

	modelRenderer_ = std::make_unique<ModelRenderer>(transform_.modelId, *modelMaterial_);
	*/
}

void Model::Update(void)
{
	taim_ += 1.0f * SceneManager::GetInstance().GetDeltaTime();
}

void Model::Draw(void)
{
	/*
	if (name_ == NAME::SHIELD)
	{
		//ディレクショナルライトの方向を取得
		auto dir = GetLightDirection();
		VECTOR pos = SceneManager::GetInstance().GetCamera().GetPos();
		modelMaterial_->SetConstBufPS(0, { dir.x,dir.y,dir.z, 0.0f });
		modelMaterial_->SetConstBufPS(1, { pos.x,pos.y,pos.z, 0.0f });
	}

	if (name_ == NAME::WATER)
	{
		modelMaterial_->SetConstBufVS(1, { WAVE_SPEED, taim_ });
		modelMaterial_->SetConstBufPS(0, { taim_, 0.0f, 0.0f, 0.0f });
	}

	if (name_ == NAME::GATE_MIST)
	{
		modelMaterial_->SetConstBufPS(0, { taim_, 0.0f, 0.0f, 0.0f });
	}
	
	modelRenderer_->Draw();
	*/
}
