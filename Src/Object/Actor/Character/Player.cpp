#include "Player.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Utility/UtilityMath.h"
#include "../../../Utility/MatrixUtility.h"
#include "../../Common/AnimationController.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/Camera.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderCapsule.h"

namespace
{
	// 衝突判定用線分開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };

	// 衝突判定用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 衝突判定用線分開始(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_START_LOCAL_POS = { 0.0f, 130.0f, 0.0f };

	// 衝突判定用線分終了(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_END_LOCAL_POS = { 0.0f, 50.0f, 0.0f };


	// 衝突判定用カプセル上部球体
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 110.0f, 0.0f };

	// 衝突判定用カプセル下部球体
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 30.0f, 0.0f };

	// 衝突判定用カプセル上部球体(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_TOP_JUMP_LOCAL_POS = { 0.0f, 160.0f, 0.0f };

	// 衝突判定用カプセル下部球体(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_DOWN_JUMP_LOCAL_POS = { 0.0f, 80.0f, 0.0f };

	// 衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 20.0f;

	// 移動速度(通常)
	static constexpr float SPEED_MOVE = 5.0f;

	// 移動速度(ダッシュ)
	static constexpr float SPEED_DASH = 10.0f;
};

Player::Player(void)
	: CharaBase::CharaBase(),
	isDash_(false)
{
}

void Player::InitLoadPost(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER));
}

void Player::InitTransform(void)
{
	transform_.InitTransform(1.0f,
							 Quaternion::Identity(), Quaternion::AngleAxis(180.0f, UtilityMath::AXIS_Y),
							 UtilityMath::VECTOR_ZERO);
}

void Player::InitCollider(void)
{
	// 主に地面との衝突で仕様する線分コライダ
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::PLAYER, &transform_,
											 COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(ColliderBase::TAG::PLAYER, &transform_,
													  COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
													  COL_CAPSULE_RADIUS);

	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);


	// 当たり判定リストに格納
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);
}

void Player::InitAnimationPost(void)
{
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::IDLE)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_PLAYER_IDLE), 30.0f);

	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::WALK)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_PLAYER_WALK), 30.0f);

	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::RUN)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_PLAYER_RUN), 30.0f);

	//animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::FALL)
//		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_PLAYER_FALL), 30.0f);

	PlayAnimation(ANIMATION_TYPE::IDLE);
}

void Player::InitPost(void)
{
}

void Player::UpdateProcess(void)
{
	ProcessJump();

	// 移動操作
	ProcessMove();
}

void Player::UpdateProcessPost(void)
{
	
}

void Player::CollisionReserve(void)
{
	/* アニメーションごとの衝突位置調整 */

	if (animation_->GetPlayType() == static_cast<int>(ANIMATION_TYPE::FALL))
	{
		// ジャンプ中は線分を伸ばす
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::LINE)) != 0)
		{
			ColliderLine* colLine = dynamic_cast<ColliderLine*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::LINE)));
			colLine->SetLocalPosStart(COL_LINE_JUMP_START_LOCAL_POS);
			colLine->SetLocalPosEnd(COL_LINE_JUMP_END_LOCAL_POS);
		}

		// ジャンプ中はカプセルを伸ばす
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::CAPSULE)) != 0)
		{
			ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::CAPSULE)));
			colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_JUMP_LOCAL_POS);
			colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_JUMP_LOCAL_POS);
		}
	}
	else
	{
		// 通常時の線分に戻す
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::LINE)) != 0)
		{
			ColliderLine* colLine = dynamic_cast<ColliderLine*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::LINE)));
			colLine->SetLocalPosStart(COL_LINE_START_LOCAL_POS);
			colLine->SetLocalPosEnd(COL_LINE_END_LOCAL_POS);
		}

		// 通常時のカプセルに戻す
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::CAPSULE)) != 0)
		{
			ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::CAPSULE)));
			colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
			colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);
		}
	}
}

void Player::ProcessMove(void)
{
	InputManager& input = InputManager::GetInstance();

	VECTOR dir = UtilityMath::VECTOR_ZERO;

	// ダッシュ処理
	isDash_ = (input.IsNew(InputManager::TYPE::PLAYER_DASH));


	if (GetJoypadNum() > 0)
	{
		dir = input.GetDirXZ_LStick();
	}
	else
	{
		if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_BACK)) { dir.z += 1.0f; }
		if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_FRONT)) { dir.z += -1.0f; }
		if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_LEFT)) { dir.x += -1.0f; }
		if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_RIGHT)) { dir.x += 1.0f; }
	}

	if (!UtilityMath::EqualsVZero(dir))
	{
		//movePow_ = UtilityMath::VECTOR_ZERO;

		// ダッシュ入力時にダッシュ加速度にする
		moveSpeed_ = ((isDash_) ? SPEED_DASH : SPEED_MOVE);

		if (isDash_)
		{
			PlayAnimation(ANIMATION_TYPE::RUN);
		}
		else
		{
			PlayAnimation(ANIMATION_TYPE::WALK);
		}

		// カメラの方向で進行
		Quaternion cameraRot = SceneManager::GetInstance().GetCamera().GetQuaRotY();

		// 移動方向を取得
		moveDir_ = Quaternion::PosAxis(cameraRot, dir);

		// 加速度に割り当て
		movePow_ = VScale(moveDir_, moveSpeed_);
	}
	else
	{
		movePow_ = UtilityMath::VECTOR_ZERO;
	}
}

void Player::ProcessJump(void)
{
	/*
	if (isHitKeyNew)
	{
		if (stepJump_ <= TIME_JUMP_INPUT)
		{
			// ジャンプ量の計算
			float jumpSpeed = POW_JUMP_KEEP * SceneManager::GetInstance().GetDeltaTime();
			jumpPow_ = VAdd(jumpPow_, VScale(UtilityMath::DIR_UP, jumpSpeed));
		}
	}*/

	// ジャンプ
	if (!isFall_)
	{
		// ジャンプ量の計算
		constexpr float POW_JUMP_INIT = 10.0f;
		float jumpSpeed = (POW_JUMP_INIT * SceneManager::GetInstance().GetDeltaTime());
		jumpPow_ = VScale(UtilityMath::DIR_UP, jumpSpeed);

		isFall_ = true;

		// アニメーション再生
		PlayAnimation(ANIMATION_TYPE::FALL, false);
	}

	// Y軸制限
	const float LIMIT_POS_Y = -1500.0f;
	if (transform_.pos.y < LIMIT_POS_Y)
	{
		transform_.pos.y = -(LIMIT_POS_Y);
	}
}

void Player::PlayAnimation(Player::ANIMATION_TYPE _type, bool _isLoop)
{
	int type = static_cast<int>(_type);

	// 指定したアニメーションが割り当てられているとき、処理終了
	if (type == animation_->GetPlayType()) { return; }

	animation_->Play(type, _isLoop);
}