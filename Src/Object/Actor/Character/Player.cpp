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
#include "../../../Common/Performance.h"
#include "../../../Camera/Camera.h"
#include "../../../Manager/StatusManager.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Collider/ColliderCapsule.h"

namespace
{
	constexpr int FRAME_NUM_SWARD_TOP = 38;
	constexpr int FRAME_NUM_SWARD_BOTTOM = 52;
	constexpr float LOCAL_ROT_Y = 180.0f;
	constexpr VECTOR START_POS = { 0.0f, 2.5f, 0.0f };
	//constexpr VECTOR LOCAL_POS_SWARD = { -8.15f, -5.0f, -5.0f };
	constexpr VECTOR LOCAL_POS_SWARD = { 0.0f, 0.0f, 0.0f };
	constexpr VECTOR LOCAL_ROT_SWARD = { 10.0f, 0.0f, 0.0f };
	const float ATTACK_RADIUS = 85.0f;

	// 衝突判定用線分開始
	constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };

	// 衝突判定用線分終了
	constexpr VECTOR COL_LINE_END_LOCAL_POS = { -10.0f, -10.0f, 0.0f };

	// 衝突判定用カプセル上部球体
	constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 75.0f, 0.0f };

	// 衝突判定用カプセル下部球体
	constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 50.0f, 0.0f };

	// 攻撃位置ローカル座標
	constexpr VECTOR COL_ATTACK_AREA_LOCAL_POS = { 0.0f, 50.0f, 0.0f };


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
	, actionStatus_(StatusManager::GetInstance().GetPlayerStatus().GetBossAction())
{
}

void Player::InitLoadPost(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER));
	sward_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER_SWARD));
}

void Player::InitTransform(void)
{
	transform_.InitTransform(status_.scale
		, Quaternion::Identity()
		, Quaternion::AngleAxis(UtilityMath::Deg2Rad(LOCAL_ROT_Y), UtilityMath::AXIS_Y)
		, START_POS);

	VECTOR posSwardTop = MV1GetFramePosition(transform_.modelId, FRAME_NUM_SWARD_TOP);
	VECTOR posSwardBottom = MV1GetFramePosition(transform_.modelId, FRAME_NUM_SWARD_BOTTOM);

	sward_.InitTransform(status_.scale
		, Quaternion::Identity()
		, Quaternion::Euler(UtilityMath::Deg2Rad(LOCAL_ROT_SWARD))
		, posSwardBottom, LOCAL_POS_SWARD);

	UpdateSwardParam();
}

void Player::InitCollider(void)
{
	// 主に地面との衝突で仕様する線分コライダ
	ColliderLine* colLine
		= new ColliderLine(ColliderBase::TAG::PLAYER, &transform_
			, COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule
		= new ColliderCapsule(ColliderBase::TAG::PLAYER, &transform_
			, COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS
			, status_.radius);

	// 攻撃範囲コライダ
	ColliderSphere* colArea
		= new ColliderSphere(ColliderBase::TAG::PLAYER, &transform_
			, COL_ATTACK_AREA_LOCAL_POS, 0.0f);

	// 当たり判定リストに格納
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::ATTACK_AREA), colArea);
}

void Player::InitAnimationPost(void)
{
	// 待機アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::IDLE)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_PLAYER_IDLE)
		, 30.0f);

	// 歩行アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::WALK)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_PLAYER_WALK)
		, VGet(0.0f, 50.0f, 0.0f), 30.0f);

	// ダッシュアニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::RUN)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_PLAYER_RUN)
		, VGet(0.0f, 50.0f, 0.0f), 30.0f);

	// 弱攻撃アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::ATTACK_LIGHT)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_PLAYER_SLASH_LIGHT)
		, 30.0f);

	// 強攻撃アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::ATTACK_HEAVY)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_PLAYER_SLASH_HEAVY)
		, 30.0f);

	// 防御アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::DEFENSE)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_PLAYER_DEFENSE)
		, 30.0f);

	// 撃破アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::DEFEAT)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_DEFEAT)
		, 30.0f);

	PlayAnimation(ANIMATION_TYPE::IDLE);
}

void Player::InitActionPost(void)
{
	const auto& actionMap = StatusManager::GetInstance().GetPlayerStatus().GetBossAction();

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
	hp_ = status_.hp;
	stamina_ = status_.stamina;
}

void Player::UpdateProcess(void)
{
	// 演出中は動作を無効化する
	if (SceneManager::GetInstance().IsActivePerformance()) { return; }

	if (hp_ <= 0)
	{
		PlayAnimation(ANIMATION_TYPE::DEFEAT, false);
	}

	// 移動操作
	if (!actionController_->IsActiveAction())
	{
		if (actionController_->IsEndActionActive())
		{
			// 攻撃無効化
			ResetColliderAttackArea();
		}

		// 被ダメージ中は処理終了
		if (animation_->GetPlayType() == static_cast<int>(ANIMATION_TYPE::DAMAGE)) { return; }

		// 移動処理
		ProcessMove();
	}

	// 防御処理
	ProcessDefense();

	ProcessDodge();

	ProcessAttackLight();
	ProcessAttackHeavy();

	UpdateSwardParam();
}

void Player::UpdateSwardParam(void)
{
	VECTOR posSwardTop = MV1GetFramePosition(transform_.modelId, FRAME_NUM_SWARD_TOP);
	VECTOR posSwardBottom = MV1GetFramePosition(transform_.modelId, FRAME_NUM_SWARD_BOTTOM);

	sward_.pos = posSwardBottom;
	sward_.quaRot = Quaternion::Mult(transform_.quaRot, Quaternion::Euler(UtilityMath::Deg2Rad(LOCAL_ROT_SWARD)));
	sward_.Update();
}

void Player::DrawDebug(void)
{
	DrawFormatString(0, 0, 0xff0000, "Player:speed(time:%.2f, speed:%.2f)"
		, TimeMove_, moveSpeed_);

	actionController_->DrawDebug();
}
void Player::DrawPre(void)
{
	MV1DrawModel(sward_.modelId);
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

void Player::SetDamage(int damageHp, int damageStamina)
{
	if (timeInv_ > 0.0f) { return; }

	timeInv_ = DAMAGE_INV_TIME;

	float damagePowerHp = static_cast<float>(damageHp);
	float damagePowerStamina = static_cast<float>(damageStamina);

	if (actionController_->GetCurActionNum() == static_cast<int>(ACTION_TYPE::DEFENSE))
	{
		// 守備状態中はダメージを軽減し、受け流し成功時はダメージを無効化
		damagePowerHp = ((parryTime_ > 0.0f) ? 0.0f : (damagePowerHp * DEFENSE_CUT_DAMAGE));
		damagePowerStamina = ((parryTime_ > 0.0f) ? 0.0f : (damagePowerStamina * DEFENSE_CUT_DAMAGE));
	}
	else if (actionController_->GetCurActionNum() == static_cast<int>(ACTION_TYPE::DODGE)
			  && !actionController_->IsEndActionActive())
	{
		// 回避成功時、ダメージを無効化j
		damagePowerHp = 0.0f;
		damagePowerStamina = 0.0f;
	}

	hp_ -= static_cast<int>(damagePowerHp);
	stamina_ -= static_cast<int>(damagePowerStamina);

	if (actionController_->IsEndActionActive())
	{
		// 行動終了時、ダメージアニメーション
		PlayAnimation(ANIMATION_TYPE::DAMAGE, false);
	}
}

void Player::ProcessMove(void)
{
	InputManager& input = InputManager::GetInstance();

	VECTOR dir = UtilityMath::VECTOR_ZERO;

	// ダッシュ処理
	isDash_ = (input.IsNew(InputManager::TYPE::PLAYER_DASH));

	// 移動処理
	if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_BACK)) { dir.z += 1.0f; }
	if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_FRONT)) { dir.z += -1.0f; }
	if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_LEFT)) { dir.x += -1.0f; }
	if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_RIGHT)) { dir.x += 1.0f; }

	if (!UtilityMath::EqualsVZero(dir))
	{
		// 移動を正規化
		const float len = std::sqrtf((dir.x * dir.x) + (dir.z * dir.z));
		dir.x /= len;
		dir.z /= len;

		TimeMove_ += SceneManager::GetInstance().GetDeltaTime();

		// 徐々に最高速度に到達させる
		const float SPEED_MAX = ((isDash_) ? status_.speedDash : status_.speedWalk);
		if (moveSpeed_ < SPEED_MAX)
		{
			const float rate = UtilityCommon::EaseInOutQuart(TimeMove_ / status_.speedAccTime);
			moveSpeed_ = status_.speedInit + (SPEED_MAX - status_.speedInit) * rate;
		}

		// アニメーション再生
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
		ResetMove();

		PlayAnimation(ANIMATION_TYPE::IDLE);
	}
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
		PlayAnimation(ANIMATION_TYPE::DEFENSE, false);
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
	curAttackPower_ = status_.powerLight;
	SetColliderAttackArea(ATTACK_RADIUS);
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
	curAttackPower_ = status_.powerHeavy;
	SetColliderAttackArea(ATTACK_RADIUS);
}

void Player::PlayAnimation(Player::ANIMATION_TYPE _type, bool isLoop)
{
	int type = static_cast<int>(_type);

	// 指定したアニメーションが割り当てられているとき、処理終了
	if (type == animation_->GetPlayType() && !animation_->IsEnd()) { return; }

	animation_->Play(type, isLoop);
}