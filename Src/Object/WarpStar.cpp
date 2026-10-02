#include <DxLib.h>
#include "../Manager/ResourceManager.h"
#include "Common/Transform.h"
#include "Player.h"
#include "WarpStar.h"

WarpStar::WarpStar(Player& player, const Transform& transform)
	: player_(player)
	, material_(nullptr), renderer_(nullptr)
{

	transform_ = transform;

	state_ = STATE::NONE;

	// 状態管理
	stateChanges_.emplace(STATE::IDLE, std::bind(&WarpStar::ChangeStateIdle, this));
	stateChanges_.emplace(STATE::RESERVE, std::bind(&WarpStar::ChangeStateReserve, this));
	stateChanges_.emplace(STATE::MOVE, std::bind(&WarpStar::ChangeStateMove, this));

}

WarpStar::~WarpStar(void)
{
}

void WarpStar::Init(void)
{

	// モデルの基本情報
	transform_.SetModel(
		resMng_.LoadModelDuplicate(
			ResourceManager::SRC::WARP_STAR)
	);
	transform_.Update();

	ChangeState(STATE::IDLE);

	material_ = std::make_unique<ModelMaterial>("StdModelVS.cso", 0
												, "StdModelPS.cso", 1);

	const COLOR_F COLOR_AMBIENT = COLOR_F(1.0f, 1.0f, 0.0f, 1.0f);
	const VECTOR LIGHT_DIR = GetLightDirection();

	material_->AddConstBufPS({ LIGHT_DIR.x, LIGHT_DIR.y, LIGHT_DIR.z });
	material_->AddConstBufPS({ COLOR_AMBIENT.r, COLOR_AMBIENT.g, COLOR_AMBIENT.b, COLOR_AMBIENT.a });

	renderer_ = std::make_unique<ModelRenderer>(transform_.modelId, *material_);
}

void WarpStar::Update(void)
{

	// 更新ステップ
	stateUpdate_();
}

void WarpStar::Draw(void)
{
	renderer_->Draw();
}

void WarpStar::ChangeState(STATE state)
{

	// 状態変更
	state_ = state;

	// 各状態遷移の初期処理
	stateChanges_[state_]();

}

void WarpStar::ChangeStateNone(void)
{
}

void WarpStar::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&WarpStar::UpdateIdle, this);
}

void WarpStar::ChangeStateReserve(void)
{
	stateUpdate_ = std::bind(&WarpStar::UpdateReserve, this);
}

void WarpStar::ChangeStateMove(void)
{
	stateUpdate_ = std::bind(&WarpStar::UpdateMove, this);
}

void WarpStar::UpdateNone(void)
{
}

void WarpStar::UpdateIdle(void)
{
}

void WarpStar::UpdateReserve(void)
{
}

void WarpStar::UpdateMove(void)
{
}
