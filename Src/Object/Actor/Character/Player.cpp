#include "Player.h"
#include <array>
#include <cassert>
#include <functional>
#include "../../../Manager/ResourceManager.h"
#include "../../../Utility/UtilityMath.h"
#include "../../../Utility/UtilityCommon.h"
#include "../../../Utility/MatrixUtility.h"
#include "../../Common/AnimationController.h"
#include "../../Common/ActionController.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/Camera.h"
#include "../../../Manager/StatusManager.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderCapsule.h"

namespace
{
	constexpr float LOCAL_ROT_Y = 180.0f;
	constexpr VECTOR START_POS = { 0.0f, 2.5f, 0.0f };

	// 衝突判定用線分開始
	constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };

	// 衝突判定用線分終了
	constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 衝突判定用カプセル上部球体
	constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 110.0f, 0.0f };

	// 衝突判定用カプセル下部球体
	constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 30.0f, 0.0f };


	// 衝突判定用カプセル球体半径
	constexpr float COL_CAPSULE_RADIUS = 20.0f;

	constexpr float PARRY_TIME = 0.5f;


	/// @brief CSV由来のパラメータを行動コントローラに変換
	ActionController::ActionParam ToControllerParam(
		const std::unordered_map<StatusPlayer::ACTION_TYPE, StatusPlayer::ActionParam>& map,
		StatusPlayer::ACTION_TYPE type)
	{
		const auto it = map.find(type);
		if (it == map.end())
		{
#ifdef _DEBUG
			assert((it == map.end()) + "\n行動が見つかりませんでした。；；\n");
#endif
			return ActionController::ActionParam{};	// 全て0のフォールバック
		}

		const StatusPlayer::ActionParam& status = it->second;
	
		return ActionController::ActionParam{
			.timeActive = status
			.timeActive,
			.timeActionTiming = status.timeTimingActive,
			.timeEnd = status.timeEnd,
			.timeStop = status.timeHitStop,
			.timeStopTiming = status.timeTimingHitStop,
			.timeInput = status.timeInput,
		};
	}
};

Player::Player(void)
	: CharaBase::CharaBase()
	, isDash_(false)
	, status_(StatusManager::GetInstance().GetPlayerStatus().GetPlayerParam())
	, actionStatus_(StatusManager::GetInstance().GetPlayerStatus().GetPlayerMotion())
{
}

void Player::InitLoadPost(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER));
}

void Player::InitTransform(void)
{
	transform_.InitTransform(status_.scale
							, Quaternion::Identity()
							, Quaternion::AngleAxis(UtilityMath::Deg2Rad(LOCAL_ROT_Y), UtilityMath::AXIS_Y)
							, START_POS);
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
	// 待機アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::IDLE)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_PLAYER_IDLE)
		, 30.0f);

	// 歩行アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::WALK)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_PLAYER_WALK)
		, 30.0f);

	// ダッシュアニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::RUN)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_PLAYER_RUN)
		, 30.0f);

	// 弱攻撃アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::ATTACK_LIGHT)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_PLAYER_SLASH_LIGHT)
		, 30.0f);

	// 強攻撃アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::ATTACK_HEAVY)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_PLAYER_SLASH_HEAVY)
		, 30.0f);

	PlayAnimation(ANIMATION_TYPE::IDLE);
}

void Player::InitActionPost(void)
{
	const auto& actionMap = StatusManager::GetInstance().GetPlayerStatus().GetPlayerMotion();

	// 防御
	actionController_->SetAction(static_cast<int>(ACTION_TYPE::DEFENSE)
		, ToControllerParam(actionMap, ACTION_TYPE::DEFENSE)
		, MakeActionCommand([this]() { Defense(); }));

	// 弱攻撃
	actionController_->SetAction(static_cast<int>(ACTION_TYPE::ATTACK_LIGHT)
		, ToControllerParam(actionMap, ACTION_TYPE::ATTACK_LIGHT)
		, MakeActionCommand([this]() { AttackLight(); })
		);

	// 強攻撃
	actionController_->SetAction(static_cast<int>(ACTION_TYPE::ATTACK_HEAVY)
		, ToControllerParam(actionMap, ACTION_TYPE::ATTACK_HEAVY)
		, MakeActionCommand([this]() { AttackHeavy(); })
		);
}

void Player::InitPost(void)
{
}

void Player::UpdateProcess(void)
{
	// 移動操作
	if (!actionController_->IsActiveAction())
	{
		ProcessMove();
	}

	ProcessDefense();

	ProcessDodge();

	ProcessAttackLight();
	ProcessAttackHeavy();

	// Y軸制限
	const float LIMIT_POS_Y = -1500.0f;
	if (transform_.pos.y < LIMIT_POS_Y)
	{
		transform_.pos.y = -(LIMIT_POS_Y);
	}
}

void Player::UpdateProcessPost(void)
{
	
}

void Player::DrawDebug(void)
{
	DrawFormatString(0, 0, 0xff0000, "Player:speed(time:%.2f, speed:%.2f)"
		, moveTime_, moveSpeed_);

	actionController_->DrawDebug();
}

void Player::ChangeAction(ACTION_TYPE action)
{

}

void Player::CollisionReserve(void)
{
	/* アニメーションごとの衝突位置調整 */

	// 通常時の線分に戻す
	if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::LINE)) != 0)
	{
		ColliderLine* colLine = dynamic_cast<ColliderLine*>(
			ownColliders_.at(static_cast<int>(COLLIDER_TYPE::LINE)));
		colLine->SetLocalPosStart(COL_LINE_START_LOCAL_POS);
		colLine->SetLocalPosEnd(COL_LINE_END_LOCAL_POS);

		jumpPow_.y = 0.0f;
	}

	// 通常時のカプセルに戻す
	if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::CAPSULE)) != 0)
	{
		ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
			ownColliders_.at(static_cast<int>(COLLIDER_TYPE::CAPSULE)));
		colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
		colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);

		jumpPow_.y = 0.0f;
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
		// 移動を正規化
		const float len = std::sqrtf((dir.x * dir.x) + (dir.z * dir.z));
		dir.x /= len;
		dir.z /= len;

		moveTime_ += SceneManager::GetInstance().GetDeltaTime();

		// 徐々に最高速度に到達させる
		const float SPEED_MAX = ((isDash_) ? status_.speedDash : status_.speedWalk);
		if (moveSpeed_ < SPEED_MAX)
		{
			const float rate = UtilityCommon::EaseInOutQuart(moveTime_ / status_.speedAccTime);
			moveSpeed_ = status_.speedInit + (SPEED_MAX - status_.speedInit) * rate;
		}

		if (isDash_)
		{
			PlayAnimation(ANIMATION_TYPE::RUN);
		}
		else
		{
			// 移動速度の上限制限
			moveSpeed_ = ((moveSpeed_ > status_.speedWalk) ? status_.speedWalk : moveSpeed_);

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
		ResetMove();

		PlayAnimation(ANIMATION_TYPE::IDLE);
	}
}

void Player::ResetMove(void)
{
	movePow_ = UtilityMath::VECTOR_ZERO;

	moveTime_ = 0.0f;
	moveSpeed_ = 0.0f;
}

void Player::ProcessDodge(void)
{
	
}
void Player::Dodge(void)
{
}

void Player::ProcessDefense(void)
{
	if (InputManager::GetInstance().IsTrgDown(InputManager::TYPE::PLAYER_DEFENSE))
	{
		actionController_->Active(static_cast<int>(ACTION_TYPE::DEFENSE));
		ResetMove();
	}

	if (actionController_->GetCurActionNum() == static_cast<int>(ACTION_TYPE::DEFENSE))
	{
		parryTime_ -= ((parryTime_ > 0.0f) ? SceneManager::GetInstance().GetDeltaTime() : 0.0f);
	}
}
void Player::Defense(void)
{
	parryTime_ = PARRY_TIME;

	SceneManager::GetInstance().GetPerformance().StartHitStrong();
}

void Player::ProcessAttackLight(void)
{
	if (InputManager::GetInstance().IsTrgDown(InputManager::TYPE::PLAYER_ATTACK_LIGHT))
	{
		actionController_->Active(static_cast<int>(ACTION_TYPE::ATTACK_LIGHT));
		ResetMove();
		PlayAnimation(ANIMATION_TYPE::ATTACK_LIGHT, false);
	}
}

void Player::AttackLight(void)
{
	SceneManager::GetInstance().GetPerformance().StartHitStrong();
}

void Player::ProcessAttackHeavy(void)
{
	if (InputManager::GetInstance().IsTrgDown(InputManager::TYPE::PLAYER_ATTACK_HEAVY))
	{
		actionController_->Active(static_cast<int>(ACTION_TYPE::ATTACK_HEAVY));
		ResetMove();
		PlayAnimation(ANIMATION_TYPE::ATTACK_HEAVY, false);
	}
}

void Player::AttackHeavy(void)
{
	SceneManager::GetInstance().GetPerformance().StartHitStrong();
}

void Player::PlayAnimation(Player::ANIMATION_TYPE _type, bool _isLoop)
{
	int type = static_cast<int>(_type);

	// 指定したアニメーションが割り当てられているとき、処理終了
	if (type == animation_->GetPlayType()) { return; }

	animation_->Play(type, _isLoop);
}