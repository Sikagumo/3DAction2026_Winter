#include "Player.h"
#include <array>
#include <cassert>
#include <functional>
#include "../../../Application.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SoundManager.h"
#include "../../../Utility/UtilityMath.h"
#include "../../../Utility/UtilityCommon.h"
#include "../../../Utility/ModelFrameUtility.h"
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
	constexpr VECTOR LOCAL_ROT = { 0.0f, 180.0f, 0.0f };
	constexpr VECTOR START_POS = { 0.0f, 2.5f, 0.0f };
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

	constexpr float PARRY_TIME = 0.3f;

	constexpr float DRAWING_ANIMATION_POINT = 0.5f;


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
	, status_(StatusManager::GetInstance().GetPlayerStatus().GetPlayerParam())
	, actionStatus_(StatusManager::GetInstance().GetPlayerStatus().GetPlayerAction())
{
}

void Player::InitLoadPost(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER));
	sword_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER_SWORD));
}

void Player::InitTransform(void)
{
	transform_.InitTransform(status_.scale
		, Quaternion::Identity()
		, Quaternion::Euler(UtilityMath::Deg2Rad(LOCAL_ROT))
		, START_POS);

	VECTOR posSwordTop = MV1GetFramePosition(transform_.modelId, FRAME_NUM_SWORD_TOP);
	VECTOR posSwordBottom = MV1GetFramePosition(transform_.modelId, FRAME_NUM_SWORD_BOTTOM);

	sword_.InitTransform(status_.scaleSword
		, Quaternion::Identity()
		, Quaternion::Euler(UtilityMath::Deg2Rad(LOCAL_ROT_SWORD))
		, posSwordBottom, LOCAL_POS_SWORD);

	UpdateSwordParam();
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
	auto& animationSpeed = StatusManager::GetInstance().GetPlayerStatus().GetAnimationSpeed();

	constexpr VECTOR ANIMATION_LOCAL_POS = { 0.0f, 50.0f, 0.0f};
	// 待機アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::IDLE)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_PLAYER_IDLE)
		, animationSpeed.idle);

	// 歩行アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::WALK)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_PLAYER_WALK)
		, ANIMATION_LOCAL_POS, animationSpeed.walk);

	// ダッシュアニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::RUN)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_PLAYER_RUN)
		, ANIMATION_LOCAL_POS, animationSpeed.run);

	// 抜刀アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::DRAWING_SWORD)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_DRAWING_SWORD)
		, ANIMATION_LOCAL_POS, actionStatus_.at(ACTION_TYPE::DRAWING_SWORD).animationSpeed);

	// 納刀アニメーション(アニメーションは抜刀を流用)
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::SHEATHING_SWORD)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_DRAWING_SWORD)
		, ANIMATION_LOCAL_POS, actionStatus_.at(ACTION_TYPE::SHEATHING_SWORD).animationSpeed);

	// 弱攻撃アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::ATTACK_LIGHT)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_PLAYER_SLASH_LIGHT)
		, actionStatus_.at(ACTION_TYPE::ATTACK_LIGHT).animationSpeed);

	// 強攻撃アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::ATTACK_HEAVY)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_PLAYER_SLASH_HEAVY)
		, actionStatus_.at(ACTION_TYPE::ATTACK_HEAVY).animationSpeed);

	// 防御アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::DEFENSE)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_PLAYER_DEFENSE)
		, actionStatus_.at(ACTION_TYPE::DEFENSE).animationSpeed);

	// 被ダメージアニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::DAMAGE)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_DAMAGE)
		, actionStatus_.at(ACTION_TYPE::DAMAGE).animationSpeed);

	// 撃破アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::DEFEAT)
		, ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ANIMATION_DEFEAT)
		, actionStatus_.at(ACTION_TYPE::DEFEAT).animationSpeed);

	PlayAnimation(ANIMATION_TYPE::IDLE);
}

void Player::InitActionPost(void)
{
	const auto& actionMap = StatusManager::GetInstance().GetPlayerStatus().GetPlayerAction();

	// 抜刀
	actionController_->SetAction(static_cast<int>(ACTION_TYPE::DRAWING_SWORD)
		, ToControllerParam(actionMap, ACTION_TYPE::DRAWING_SWORD)
		, MakeActionCommand([this]() { ActionDefense(); }));

	// 納刀
	actionController_->SetAction(static_cast<int>(ACTION_TYPE::SHEATHING_SWORD)
		, ToControllerParam(actionMap, ACTION_TYPE::SHEATHING_SWORD)
		, MakeActionCommand([this]() { ActionSheathingSword(); }));

	// 防御
	actionController_->SetAction(static_cast<int>(ACTION_TYPE::DEFENSE)
		, ToControllerParam(actionMap, ACTION_TYPE::DEFENSE)
		, MakeActionCommand([this]() { ActionDefense(); }));

	// 弱攻撃
	actionController_->SetAction(static_cast<int>(ACTION_TYPE::ATTACK_LIGHT)
		, ToControllerParam(actionMap, ACTION_TYPE::ATTACK_LIGHT)
		, MakeActionCommand([this]() { ActionAttackLight(); })
		);

	// 強攻撃
	actionController_->SetAction(static_cast<int>(ACTION_TYPE::ATTACK_HEAVY)
		, ToControllerParam(actionMap, ACTION_TYPE::ATTACK_HEAVY)
		, MakeActionCommand([this]() { ActionAttackHeavy(); })
		);

	// 被ダメージ
	actionController_->SetAction(static_cast<int>(ACTION_TYPE::DAMAGE)
		, ToControllerParam(actionMap, ACTION_TYPE::DAMAGE)
		, MakeActionCommand([this]() { ActionNone(); })
		);
	// 強攻撃
	actionController_->SetAction(static_cast<int>(ACTION_TYPE::DEFEAT)
		, ToControllerParam(actionMap, ACTION_TYPE::DEFEAT)
		, MakeActionCommand([this]() { ActionNone(); })
		);
}

void Player::InitPost(void)
{
	hp_ = status_.hp;
	stamina_ = status_.stamina;
	attackType_ = ATTACK_TYPE::NONE;
	actionState_ = ACTION_STATE::IDLE;
}

void Player::UpdateProcess(void)
{
	if (hp_ <= 0)
	{
		if (GetAnimationType() != ANIMATION_TYPE::DEFEAT)
		{
			// 撃破処理
			ActiveAction(ACTION_TYPE::DEFEAT);
			PlayAnimation(ANIMATION_TYPE::DEFEAT, false);
		}
		return;
	}

	// 演出中は動作を無効化する
	if (SceneManager::GetInstance().IsActivePerformance()) { return; }

	// 移動操作
	if (!actionController_->IsActiveAction()
		&& actionState_ != ACTION_STATE::DEFENSE)
	{
		if (curAttackPower_ > 0
			&& actionController_->IsEndActionActive())
		{
			// 攻撃無効化
			ResetColliderAttackArea();
		}

		// 被ダメージ中は処理終了
		if (GetAnimationType() != ANIMATION_TYPE::DAMAGE)
		{
			// 移動処理
			ProcessMove();
		}
	}
	else
	{
		ResetMove();
	}

	//ProcessDodge();

	// 抜刀処理
	ProcessDrawingSword();

	// 納刀処理
	ProcessSheathingSword();

	if (timeDrawingSward_ > 0.0f)
	{
		// 抜刀・納刀中は処理終了
		if (GetActionType() == ACTION_TYPE::DRAWING_SWORD
			|| GetActionType() == ACTION_TYPE::SHEATHING_SWORD) { return; }

		// 防御処理
		ProcessDefense();

		if (InputManager::GetInstance().IsTrgDown(InputManager::TYPE::PLAYER_ATTACK_LIGHT))
		{
			ActiveAttackLight();
		}

		if (InputManager::GetInstance().IsTrgDown(InputManager::TYPE::PLAYER_ATTACK_HEAVY))
		{
			ActiveAttackHeavy();
		}
	}
}

void Player::UpdateSwordParam(void)
{
	ModelFrameUtility::SetFrameWorldMatrix(
		transform_, FRAME_NUM_SWORD_TOP, sword_, LOCAL_POS_SWORD, VGet(0.0f, 0.0f, 0.0f));

	// matRot(プレイヤーの向き+手の回転)を quaRot に変換して反映する
	sword_.quaRot = Quaternion::GetRotation(sword_.matRot);
	sword_.Update();
}

void Player::Draw(void)
{
	constexpr COLOR_F PARRY_COLOR = { 1.0f, 1.0f, 0.0f, 1.0f };
	constexpr COLOR_F DAMAGE_COLOR = { 1.0f, 0.25f, 0.25f, 1.0f };
	COLOR_F diffuseColor = COLOR_F(1.0f, 1.0f, 1.0f, 1.0f);

	// ダメージ有効時、モデル変更
	diffuseColor = ((timeInv_ > 0.0f) ? DAMAGE_COLOR : diffuseColor);

	// パリィ有効時、白く光らせる
	diffuseColor = ((parryTime_ > 0.0f && actionState_ == ACTION_STATE::DEFENSE) ? PARRY_COLOR : diffuseColor);

	MV1SetDifColorScale(transform_.modelId, diffuseColor);

	CharaBase::Draw();
}

void Player::DrawDebug(void)
{
#ifdef _DEBUG
	DrawFormatString(0, 0, 0xff0000, "Player:speed(time:%.2f, speed:%.2f)"
		, TimeMove_, moveSpeed_);

	actionController_->DrawDebug();
#endif

	DrawFormatString(0, Application::SCREEN_HALF_Y + (8 * 3), 0xffffff, "PlayerHp:%d", hp_, actionController_->GetCurActionNum());
}
void Player::DrawPre(void)
{
	if (isSwordActive_)
	{
		MV1DrawModel(sword_.modelId);
	}
}

void Player::ChangeAction(ACTION_TYPE action)
{
	ActiveAction(action);
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
	if (timeInv_ > 0.0f || hp_ <= 0) {
		return;
	}

	timeInv_ = DAMAGE_INV_TIME;

	float damagePowerHp = static_cast<float>(damageHp);
	float damagePowerStamina = static_cast<float>(damageStamina);

	if (actionState_ == ACTION_STATE::DEFENSE
		|| GetAnimationType() == ANIMATION_TYPE::DEFENSE)
	{
		if (parryTime_ > 0.0f)
		{
#ifdef _DEBUG
			printfDx("パリィ成功！");
#endif
			// パリィ成功時、ダメージを無効化
			SoundManager::GetInstance().Play(SoundManager::SRC::SE_PARRY, false);
			SceneManager::GetInstance().GetPerformance().StartHitSlow(0.5f, 2);
			return;
		}
		else
		{
			// 守備状態中はダメージを軽減し、受け流し成功時はダメージを無効化
			damagePowerHp = (damagePowerHp * DEFENSE_CUT_DAMAGE);
			damagePowerStamina = (damagePowerStamina * DEFENSE_CUT_DAMAGE);
		}
	}
	else if (actionController_->GetCurActionNum() == static_cast<int>(ACTION_TYPE::DODGE)
		&& !actionController_->IsEndActionActive())
	{
		// 回避成功時、処理終了
		return;
	}

	hp_ -= static_cast<int>(damagePowerHp);
	stamina_ -= static_cast<int>(damagePowerStamina);

	// 行動終了時、ダメージ演出
	PlayAnimation(ANIMATION_TYPE::DAMAGE, false, false, true);
	ActiveAction(ACTION_TYPE::DAMAGE);
	ResetMove();
	actionState_ = ACTION_STATE::IDLE;

#ifdef _DEBUG
	if (actionState_ == ACTION_STATE::DEFENSE
		&& GetAnimationType() == ANIMATION_TYPE::DEFENSE)
	{
		printfDx("パリィ失敗");
	}
	else
	{
		printfDx("ダメージ");
	}
#endif
}

void Player::ProcessMove(void)
{
	InputManager& input = InputManager::GetInstance();

	VECTOR dir = UtilityMath::VECTOR_ZERO;

	// 移動処理
	if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_BACK)) { dir.z += 1.0f; }
	if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_FRONT)) { dir.z += -1.0f; }
	if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_LEFT)) { dir.x += -1.0f; }
	if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_RIGHT)) { dir.x += 1.0f; }

	if (!UtilityMath::EqualsVZero(dir))
	{
		// 移動中にダッシュ入力時、状態切替
		const bool IS_DASH = input.IsNew(InputManager::TYPE::PLAYER_DASH);

		// 移動を正規化
		const float len = std::sqrtf((dir.x * dir.x) + (dir.z * dir.z));
		dir.x /= len;
		dir.z /= len;
		
		TimeMove_ += SceneManager::GetInstance().GetDeltaTime();

		// ダッシュ時と通常時で最高速度を切替
		const float SPEED_MAX = (IS_DASH ? status_.speedDash : status_.speedWalk);

		if (moveSpeed_ < SPEED_MAX)
		{
			const float rate = UtilityCommon::EaseInOutQuart(TimeMove_ / status_.speedAccTime);
			moveSpeed_ = status_.speedInit + (SPEED_MAX - status_.speedInit) * rate;
		}

		// アニメーション再生
		if (IS_DASH)
		{
			actionState_ = ACTION_STATE::DASH;
			PlayAnimation(ANIMATION_TYPE::RUN);
		}
		else
		{
			actionState_ = ACTION_STATE::WALK;
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
void Player::ActionDodge(void)
{
}

void Player::ProcessDefense(void)
{
	if (InputManager::GetInstance().IsTrgDown(InputManager::TYPE::PLAYER_DEFENSE)
		&& !actionController_->IsActiveAction())
	{
		ActiveAction(ACTION_TYPE::DEFENSE);
		ResetMove();
		PlayAnimation(ANIMATION_TYPE::DEFENSE, false);
		actionState_ = ACTION_STATE::DEFENSE;
	}
	if (actionState_ == ACTION_STATE::DEFENSE)
	{
		if (!InputManager::GetInstance().IsNew(InputManager::TYPE::PLAYER_DEFENSE)
			|| GetActionType() != ACTION_TYPE::DEFENSE && GetAnimationType() == ANIMATION_TYPE::DEFENSE)
		{
			actionState_ = ACTION_STATE::IDLE;
		}
	}

	if (GetActionType() == ACTION_TYPE::DEFENSE)
	{
		parryTime_ -= ((parryTime_ > 0.0f) ? SceneManager::GetInstance().GetDeltaTime() : 0.0f);
	}
}
void Player::ActionDefense(void)
{
	parryTime_ = PARRY_TIME;
	timeDrawingSward_ = TIME_DRAWING_SWORD;

	// 剣の表示
	isSwordActive_ = true;
}

void Player::ActiveAttackLight(void)
{
	timeDrawingSward_ = TIME_DRAWING_SWORD;
	ActiveAction(ACTION_TYPE::ATTACK_LIGHT);
	PlayAnimation(ANIMATION_TYPE::ATTACK_LIGHT, false, false, true);
}

void Player::ActionAttackLight(void)
{
	curAttackPower_ = status_.powerLight;
	SetColliderAttackArea(ATTACK_RADIUS);
}

void Player::ActiveAttackHeavy(void)
{
	timeDrawingSward_ = TIME_DRAWING_SWORD;

	ActiveAction(ACTION_TYPE::ATTACK_HEAVY);
	PlayAnimation(ANIMATION_TYPE::ATTACK_HEAVY, false, false, true);
}

void Player::ActionAttackHeavy(void)
{
	curAttackPower_ = status_.powerHeavy;
	SetColliderAttackArea(ATTACK_RADIUS);

	timeDrawingSward_ = TIME_DRAWING_SWORD;
}

void Player::ProcessDrawingSword(void)
{
	bool isInputHeavy = InputManager::GetInstance().IsTrgDown(InputManager::TYPE::PLAYER_ATTACK_HEAVY);
	bool isInputLight = InputManager::GetInstance().IsTrgDown(InputManager::TYPE::PLAYER_ATTACK_LIGHT);
	bool isInputDefense = InputManager::GetInstance().IsNew(InputManager::TYPE::PLAYER_DEFENSE);

	if (!isSwordActive_)
	{
		if ((isInputHeavy || isInputLight || isInputDefense)
			&& !isSwordActive_ && timeDrawingSward_ <= 0.0f)
		{
			// 抜刀
			timeDrawingSward_ = TIME_DRAWING_SWORD;
			PlayAnimation(ANIMATION_TYPE::DRAWING_SWORD, false);
			ActiveAction(ACTION_TYPE::DRAWING_SWORD);
			isSwordActive_ = true;

			isReservedDefense_ = isInputDefense;

			if (!isInputDefense)
			{
				// 攻撃種類を決定
				attackType_ = ((isInputHeavy) ? ATTACK_TYPE::HEAVY : ATTACK_TYPE::LIGHT1);
			}
		}
	}
	else if (attackType_ != ATTACK_TYPE::NONE && !isReservedDefense_)
	{
		if (isInputDefense)
		{
			isReservedDefense_ = isInputDefense;
		}
		else
		{
			if (isInputLight)
			{
				attackType_ = ATTACK_TYPE::LIGHT1;
			}
			else if (isInputHeavy)
			{
				attackType_ = ATTACK_TYPE::HEAVY;
			}
		}
	}

	if (timeDrawingSward_ > 0.0f)
	{
		timeDrawingSward_ -= SceneManager::GetInstance().GetDeltaTime();

		const bool isReserved = (attackType_ != ATTACK_TYPE::NONE || isReservedDefense_);
		const bool isDrawing = (animation_->GetPlayType() == static_cast<int>(ANIMATION_TYPE::DRAWING_SWORD));

		if (isReserved && isDrawing
			&& animation_->GetPlayPointRate() >= DRAWING_ANIMATION_POINT)
		{
			if (attackType_ != ATTACK_TYPE::NONE)
			{
				if (attackType_ == ATTACK_TYPE::HEAVY)
				{
					ActiveAttackHeavy();
				}
				else if (attackType_ == ATTACK_TYPE::LIGHT1)
				{
					ActiveAttackLight();
				}
			}
			else
			{
				ActiveAction(ACTION_TYPE::DEFENSE);
				PlayAnimation(ANIMATION_TYPE::DEFENSE, false, false, true);
				actionState_ = ACTION_STATE::DEFENSE;
			}

			isReservedDefense_ = false;
		}
	}
}

void Player::ActionDrawingSword(void)
{
	timeDrawingSward_ = TIME_DRAWING_SWORD;
	ResetMove();
}

void Player::ProcessSheathingSword(void)
{
	if (isSwordActive_
		&& timeDrawingSward_ <= 0.0f
		&& GetActionType() != ACTION_TYPE::SHEATHING_SWORD)
	{
		// 納刀処理
		ActiveAction(ACTION_TYPE::SHEATHING_SWORD);
		PlayAnimation(ANIMATION_TYPE::SHEATHING_SWORD, false, true);
		ResetMove();
		attackType_ = ATTACK_TYPE::NONE;
	}
}

void Player::ActionSheathingSword(void)
{
	isSwordActive_ = false;
	ResetMove();
}

void Player::PlayAnimation(Player::ANIMATION_TYPE _type, bool isLoop, bool isReversePlay, bool isReStart, bool isBlendAnimation)
{
	int type = static_cast<int>(_type);

	if (isReStart && GetAnimationType() == _type)
	{
		animation_->ReStart();
	}

	// 指定したアニメーションが割り当てられているとき、処理終了
	if (type == animation_->GetPlayType() && !animation_->IsEnd()) { return; }

	constexpr float BLEND_TIME_DEFENSE = 0.1f;
	float blendTime = (isBlendAnimation ? AnimationController::BLEND_TIME_DEFAULT : 0.0f);
	blendTime = ((_type == ANIMATION_TYPE::DEFENSE && isBlendAnimation) ? BLEND_TIME_DEFENSE : blendTime);

	animation_->Play(type, isLoop, isReversePlay, -1, blendTime);
}

Player::ANIMATION_TYPE Player::GetAnimationType(void)
{
	int actionNum = actionController_->GetCurActionNum();
	if (actionNum < 0 || actionNum >= static_cast<int>(ANIMATION_TYPE::MAX))
	{
#ifdef _DEBUG
		std::string text = "\nプレイヤーのアニメーションの種類が取得できませんでした。アニメーション番号：";
		std::to_string(actionNum);
		OutputDebugString(text.c_str());
#endif
		return ANIMATION_TYPE::NONE;
	}

	return static_cast<ANIMATION_TYPE>(actionNum);
}

void Player::ActiveAction(Player::ACTION_TYPE type)
{
	int actionNum = static_cast<int>(type);

	// 指定した行動が割り当てられているとき、処理終了
	if (actionNum == actionController_->GetCurActionNum()
		&& !actionController_->IsActionEnd()) {
		return;
	}

	actionController_->Active(actionNum);
}

Player::ACTION_TYPE Player::GetActionType(void)
{
	int actionNum = actionController_->GetCurActionNum();
	if (actionNum < 0 || actionNum >= static_cast<int>(ACTION_TYPE::MAX))
	{
#ifdef _DEBUG
		OutputDebugString("\nプレイヤーの行動の種類が取得できませんでした\n");
#endif
		return ACTION_TYPE::NONE;
	}

	return static_cast<ACTION_TYPE>(actionNum);
}