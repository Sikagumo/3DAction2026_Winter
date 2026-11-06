#include "../Object/Actor/ActorBase.h"
#include "../Object/Common/Transform.h"
#include "../Utility/UtilityMath.h"
#include "../Manager/InputManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Object/Collider/ColliderBase.h"
#include "../Object/Collider/ColliderModel.h"
#include "../Object/Collider/ColliderSphere.h"
#include "../Application.h"
#include <DxLib.h>
#include <algorithm>
#include <EffekseerForDXLib.h>
#include <map>
#include "Camera.h"


namespace
{
	// カメラの初期座標
	constexpr VECTOR DERFAULT_POS = { 0.0f, 200.0f, -500.0f };

	// カメラの移動スピード
	constexpr float CAMERA_MOVE_SPEED = 50.0f;

	// カメラの初期角度
	constexpr VECTOR DERFAULT_ANGLES = { 0.0f, 0.0f, 0.0f };

	// 追従対象の最大Y軸座標
	constexpr float TARGET_POS_MAX_Y = 275.0f;

	// マウス移動量が反映される最小値
	constexpr float MOUSE_MOVE_THRESHOLD = 0.0f;

	// 追従有効範囲
	constexpr float POS_SPACE_MIN = 300.0f;
	constexpr float POS_SPACE_MAX = 1750.0f;

	// Y軸の押し出し最小地点
	constexpr float RESOLVE_FOLLOW_POS_MIN_Y = -5.0f;

	// カメラの回転量
	const float ROT_POW_KEY = UtilityMath::Deg2Rad(5.0f);
	const float ROT_POW_RAD = UtilityMath::Deg2Rad(2.5f);
	const float ROT_POW_MOUSE = UtilityMath::Deg2Rad(0.05f);
};

Camera::Camera(void)
	: ActorBase::ActorBase()
	, followTransform_(nullptr)
	, mode_(MODE::NONE)
	, angles_(UtilityMath::VECTOR_ZERO)
	, rotY_(Quaternion::Identity())
	, targetPos_(UtilityMath::VECTOR_ZERO)
	, prePos_(UtilityMath::VECTOR_ZERO)
	, lockOnParam_{}
	, followDistScale_(1.0f)
	, easing_(EasingParam())
{
}

void Camera::InitCollider(void)
{
	// 主に地面との衝突で使用する球体コライダ
	ColliderSphere* colliderSphere
		= new ColliderSphere(ColliderBase::TAG::CAMERA, &transform_
			, UtilityMath::VECTOR_ZERO, COL_CAPSULE_SPHERE);

	ownColliders_.emplace(static_cast<int>(ColliderBase::TAG::CAMERA), colliderSphere);
}

void Camera::InitPost(void)
{
	// 初期化処理
	SetCameraMode(MODE::NONE);

	// ロックオン対象なし
	hasTarget_ = false;
	isLockOn_ = false;
	lockOnParam_ = TargetParam();
}

void Camera::Update(void)
{
	// 更新前情報
	prePos_ = transform_.pos;

	// 各状態別処理
	update_();

	// ターゲット対象切替時のイージング進行
	if (easing_.term < 1.0f)
	{
		easing_.term += (SceneManager::GetInstance().GetDeltaTime() / easing_.endTime);
		easing_.term = std::clamp(easing_.term, 0.0f, 1.0f);
	}
}

void Camera::SetBeforeDraw(void)
{
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		transform_.pos, 
		targetPos_, 
		transform_.GetUp()
	);

	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();
}

void Camera::DrawDebug(void)
{
#ifdef _DEBUG

	if (followTransform_ == nullptr) { return; }

	VECTOR target = VSub(lockOnParam_.pos, VGet(followTransform_->pos.x, lockOnParam_.pos.y, followTransform_->pos.z));
	float tan = atan2f(target.x, target.z);
	VECTOR rotY = rotY_.ToEuler();

	DrawFormatString(0, 32, 0xffffff, "Camera(pos(%.1f, %.1f, %.1f), \nangle(%.1f°, %.1f°, %.1f°)),rotY(%.1f°, %.1f°, %.1f°))"
		, transform_.pos.x, transform_.pos.y, transform_.pos.z
		, UtilityMath::Rad2Deg(angles_.x), UtilityMath::Rad2Deg(angles_.y), UtilityMath::Rad2Deg(angles_.z)
		, UtilityMath::Rad2Deg(rotY.x), UtilityMath::Rad2Deg(rotY.y), UtilityMath::Rad2Deg(rotY.z));
#endif
}

VECTOR Camera::GetForward(void) const
{
	return UtilityMath::VNormalize((VSub(targetPos_, transform_.pos)));
}

void Camera::SetCameraMode(MODE _mode)
{
	// カメラの初期設定
	SetDefault();

	// カメラモードの変更
	mode_ = _mode;

	// 変更時の初期化処理
	switch (mode_)
	{
		// 定点カメラ状態
		case Camera::MODE::FIXED_POINT:
			update_ = std::bind(&Camera::SetBeforeDrawFixedPoint, this);
			break;

		// フリーカメラ
		case Camera::MODE::FREE:
			update_ = std::bind(&Camera::SetBeforeDrawFree, this);
			break;

		// プレイヤー追従
		case Camera::MODE::FOLLOW:
			update_ = std::bind(&Camera::SetBeforeDrawFollow, this);
			break;

		// トラッキング
		case Camera::MODE::TRACKING:
			update_ = std::bind(&Camera::SetBeforeDrawTracking, this);
			break;

		default:
			break;
	}

}

void Camera::SetLockOnTarget(const VECTOR& targetPos, int targetHp)
{
	if (targetHp <= 0)
	{
		// 対象が無効(HP0以下)の場合、対象を破棄
		ClearLockOnTarget();
		return;
	}

	if (!hasTarget_)
	{
		// 初めて登録される対象の場合、最大HPを記録
		lockOnParam_.maxHp = targetHp;
		hasTarget_ = true;
	}

	// 位置とHPを更新
	lockOnParam_.pos = targetPos;
	lockOnParam_.hp  = targetHp;
}

void Camera::ClearLockOnTarget(void)
{
	// ロックオン中なら先に解除する(解除時の同期処理で対象位置を使うため、情報の破棄より前に行う)
	if (isLockOn_)
	{
		SetIsLockOn(false);
	}

	hasTarget_ = false;
	lockOnParam_ = TargetParam();
}

bool Camera::CanLockOn(void) const
{
	// 追従対象が一定以上の高さにいる場合は不可
	if (!hasTarget_
		|| lockOnParam_.pos.y > TARGET_POS_MAX_Y) {
		return false;
	}

	// XZ平面上での、カメラと対象との距離
	VECTOR posTarget = VGet(lockOnParam_.pos.x, 0.0f, lockOnParam_.pos.z);
	VECTOR posCamera = VGet(transform_.pos.x, 0.0f, transform_.pos.z);
	float sizeXZ = VSize(VSub(posTarget, posCamera));

	// 追従有効範囲内か
	return (sizeXZ >= POS_SPACE_MIN && sizeXZ <= POS_SPACE_MAX);
}

void Camera::FollowLockOnPosition(void)
{
	if (!isLockOn_) { return; }

	// 対象が無効、または一定以上の高さにいる場合、ロックオン解除
	if (!hasTarget_ || lockOnParam_.pos.y > TARGET_POS_MAX_Y)
	{
		SetIsLockOn(false);
	}
}

void Camera::SetIsLockOn(bool _isLockOn)
{
	// 状態が変わらない場合は何もしない
	if (isLockOn_ == _isLockOn) { return; }

	// ロックオン開始は、有効な対象が範囲内にいる場合のみ
	if (_isLockOn && !CanLockOn()) { return; }

	// ロックオン解除時、アングルとY軸回転を同期
	if (!_isLockOn)
	{
		SyncAngleYFromRotY();
	}

	// 切り替え時、現在の位置と注視点をイージング開始点として記録
	easing_.fromPosition = transform_.pos;
	easing_.fromTarget = targetPos_;
	easing_.term = 0.0f;
	easing_.endTime = LOCKON_DURATION;

	isLockOn_ = _isLockOn;
}

void Camera::SetDefault(void)
{
	// カメラの初期設定
	transform_.pos = DERFAULT_POS;

	// カメラ角
	angles_ = DERFAULT_ANGLES;
	transform_.quaRot = Quaternion::Identity();

	// 注視点
	targetPos_ = UtilityMath::VECTOR_ZERO;
}

void Camera::SyncFollow(void)
{
	if (followTransform_ == nullptr) { return; }

	// 同期先の位置
	VECTOR pos = followTransform_->pos;

	// Y軸
	rotY_ = Quaternion::AngleAxis(angles_.y, UtilityMath::AXIS_Y);

	if (isLockOn_)
	{
		// 追従位置 = ロックオン位置 - 追従座標
		VECTOR target = followTransform_->pos;
		target.y = lockOnParam_.pos.y;
		VECTOR toTarget = VSub(lockOnParam_.pos, target);


		// XZ平面に投影（Y成分を無視）してY軸回転角を求める
		float yAngle = atan2(toTarget.x, toTarget.z);

		rotY_ = Quaternion::AngleAxis(yAngle, UtilityMath::AXIS_Y);
	}

	// Y軸 + X軸
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, UtilityMath::AXIS_X));

	// 注視点
	VECTOR localPos = transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS);
	VECTOR newTarget = VAdd(pos, localPos);


	if (isLockOn_)
	{
		VECTOR lockOn = VAdd(pos, VSub(lockOnParam_.pos, pos));
		lockOn.y = std::clamp(lockOn.y, -TARGET_POS_MAX_Y, TARGET_POS_MAX_Y);
		newTarget = lockOn;
	}

	// カメラ位置
	const VECTOR LOCAL_POS = ((isLockOn_) ? FOLLOW_LOCAL_POS_LOCKON : FOLLOW_LOCAL_POS);
	VECTOR scaledLocalPos = VScale(LOCAL_POS, followDistScale_);
	localPos = transform_.quaRot.PosAxis(scaledLocalPos);
	VECTOR newCameraPos = VAdd(pos, localPos);

	// イージング中は EasingChangeTarget() で補間、完了後はそのまま代入
	if (easing_.term < 1.0f)
	{
		transform_.pos = EasingChangeTarget(easing_.fromPosition, newCameraPos, easing_.term);
		targetPos_ = EasingChangeTarget(easing_.fromTarget, newTarget, easing_.term);
	}
	else
	{
		transform_.pos = newCameraPos;
		targetPos_ = newTarget;
	}
}

void Camera::SyncAngleYFromRotY(void)
{
	// rotY_が示す前方ベクトルをXZ平面上で求める
	VECTOR forward = rotY_.PosAxis(UtilityMath::DIR_FORWARD);

	// atan2でY軸角度を逆算して、angles_.yに書き戻す
	angles_.y = atan2f(forward.x, forward.z);

	SyncFollow();
}

void Camera::ProcessRot(bool _isLimit)
{
	// マウス回転
	RotationMouse(_isLimit);

	// 方向回転によるXYZの移動(ゲームパッド)
	RotationGamePad(_isLimit);
}

void Camera::SetBeforeDrawFixedPoint(void)
{
	// 何もしない
	transform_.Update();
}

void Camera::SetBeforeDrawFree(void)
{
	// カメラ操作(回転)
	ProcessRot(false);
	
	// カメラ操作(移動)
	ProcessMove();

	// Y軸
	rotY_ = Quaternion::AngleAxis(angles_.y, UtilityMath::AXIS_Y);

	// Y軸 + X軸
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, UtilityMath::AXIS_X));

	// 注視点更新
	targetPos_ = VAdd(transform_.pos, transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS));
}

void Camera::SetBeforeDrawFollow(void)
{
	if (InputManager::GetInstance().IsTrgDown(InputManager::TYPE::CAMERA_LOCKON))
	{
		// ロックオン切替
		SetIsLockOn(!isLockOn_);
	}

	// ロックオン中、対象が無効になったら解除する
	FollowLockOnPosition();

	// カメラ位置の補間
	transform_.pos = UtilityMath::Lerp(prePos_, transform_.pos, LERP_RATE_MOVE);

	// カメラ操作(回転)
	if (!isLockOn_)
	{
		ProcessRot(true);
	}

	// 追従対象との相対位置を同期
	SyncFollow();

	Collision();

	// 地面下に移動制限を掛ける
	if (transform_.pos.y < RESOLVE_FOLLOW_POS_MIN_Y)
	{
		transform_.pos.y = RESOLVE_FOLLOW_POS_MIN_Y;
	}
}

void Camera::SetBeforeDrawTracking(void)
{
	// イージング中は EasingChangeTarget() で補間、完了後はそのまま代入
	if (easing_.term < 1.0f)
	{
		transform_.pos = EasingChangeTarget(easing_.fromPosition, easing_.toPosition, easing_.term);
		targetPos_ = EasingChangeTarget(easing_.fromTarget, easing_.toTarget, easing_.term);
	}
	else
	{
		transform_.pos = easing_.toPosition;
		targetPos_ = easing_.toTarget;
	}

	// カメラ角度のイージング
	transform_.quaRot = Quaternion::Slerp(transform_.quaRot, easing_.toRotation, 0.2f);
}

void Camera::ProcessMove(void)
{
	VECTOR moveDir = InputManager::GetInstance().GetDirXZ_LStick();

	// 移動処理
	if (!UtilityMath::EqualsVZero(moveDir))
	{
		// 現在の向きからの進行方向を取得
		VECTOR direction = UtilityMath::VNormalize((transform_.quaRot.PosAxis(moveDir)));

		// 移動させたい方向に移動量をかける(=移動量)
		VECTOR movePow = VScale(direction, CAMERA_MOVE_SPEED);

		// カメラ位置も注視点も移動させる
		transform_.pos = VAdd(transform_.pos, movePow);
		targetPos_ = VAdd(targetPos_, movePow);
	}
}

void Camera::RotationMouse(bool _isLimit)
{
	// マウス移動量
	Vector2 mouseMove = InputManager::GetInstance().GetMouseMove();

	// マウス移動量がしきい値未満の場合０にする
	mouseMove.x = ((std::abs(mouseMove.x) > MOUSE_MOVE_THRESHOLD) ? mouseMove.x : 0);
	mouseMove.y = ((std::abs(mouseMove.y) > MOUSE_MOVE_THRESHOLD) ? mouseMove.y : 0);

	/*
	// マウス感度を適用
	auto sensitivity = InputManager::GetInstance().GetMouseSensitivity();
	mouseMove.x *= sensitivity.x;
	mouseMove.y *= sensitivity.y;

	// 操作反転を適用
	if (sensitivity.invertX)
	{
		mouseMove.x = -mouseMove.x;
	}
	if (sensitivity.invertY)
	{
		mouseMove.y = -mouseMove.y;
	}*/

	if (!UtilityMath::EqualsVZero(mouseMove))
	{
		// カメラ回転を
		angles_.x += (mouseMove.y * ROT_POW_MOUSE);
		angles_.y += (mouseMove.x * ROT_POW_MOUSE);

		if (_isLimit)
		{
			angles_.x = std::clamp(angles_.x, -LIMIT_X_DOWN, LIMIT_X_UP);
		}
	}
}

void Camera::RotationGamePad(bool _isLimit)
{
	VECTOR rightStick = InputManager::GetInstance().GetDirXZ_RStick();

	// 取得したX, Yの値をVECTOR型のX, Zに割り当てる
	VECTOR dir = VGet(rightStick.x, 0.0f, rightStick.y);

	if (!UtilityMath::EqualsVZero(dir))
	{
		// 右スティック左右の傾き
		angles_.y += (dir.x * ROT_POW_RAD);

		// 右スティック上下の傾き
		angles_.x += (dir.z * ROT_POW_RAD);

		if (_isLimit)
		{
			// 角度制限
			angles_.x = std::clamp(angles_.x, -LIMIT_X_DOWN, LIMIT_X_UP);
		}
	}
}

void Camera::Collision(void)
{
	// プレイヤーのルートフレーム
	VECTOR start = MV1GetFramePosition(followTransform_->modelId, 1);

	for (const auto& hitCol : hitColliders_)
	{
		// モデル以外は処理を飛ばす
		if (hitCol->GetShape() != ColliderBase::SHAPE::MODEL) continue;

		// 派生クラスへキャスト
		const ColliderModel* colliderModel = dynamic_cast<const ColliderModel*>(hitCol);

		if (colliderModel == nullptr) continue;

		// 線分で衝突判定
		//auto hitPoly = colliderModel->GetNearestHitPolyLine(transform_.pos, start, true);

		auto hits = MV1CollCheck_LineDim(
			colliderModel->GetFollow()->modelId,
			-1,
			transform_.pos,
			start
		);

		// 追従対象に一番近い衝突点を探す
		bool isCollision = false;
		MV1_COLL_RESULT_POLY hitPoly;
		double minDist = DBL_MAX;

		for (int i = 0; i < hits.HitNum; i++)
		{
			const auto& hit = hits.Dim[i];


			// 除外フレームは無視する
			if (colliderModel->IsExcludeFrame(hit.FrameIndex)) { continue; }
			//
			// 対象フレームは無視する
			if (!colliderModel->IsTargetFrame(hit.FrameIndex)) { continue; }


			// 衝突判定
			isCollision = true;

			// 距離判定
			float dist = VSize(VSub(hit.HitPosition, transform_.pos));

			if (minDist > dist && minDist > VIEW_NEAR)
			{
				// 追従対象に一番近い衝突点を優先
				minDist = dist;
				hitPoly = hit;
			}
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);

		if (!isCollision)
			//if (hitPoly.HitFlag == 0)
		{
			// 衝突していなければ次のコライダへ
			return;
		}
		// カメラ位置から注視点への方向
		VECTOR dirToTarget = UtilityMath::VNormalize(VSub(targetPos_, transform_.pos));

		// 衝突点の少し手前にカメラを置く
		transform_.pos =
			VAdd(hitPoly.HitPosition, VScale(dirToTarget, COLLISION_BACK_DIS));


		// カメラ位置の球体コライダ
		int typeSphere = static_cast<int>(COLLIDER_TYPE::SPHERE);

		// 球体コライダが無ければ処理を抜ける
		if (ownColliders_.count(typeSphere) == 0) { continue; }

		// 球体コライダ情報
		ColliderSphere* colliderSphere =
			dynamic_cast<ColliderSphere*>(ownColliders_.at(typeSphere));

		if (colliderSphere == nullptr) { return; }

		// 反発処理
		transform_.pos = colliderSphere->GetPosPushBackAlongNormal(hitPoly, CNT_TRY_COLLISION_CAMERA, COLLISION_BACK_DIS);
	}
}

void Camera::ResolveCollision(void)
{
	// プレイヤー追従中のみ押し出しを有効にする
	if (mode_ != MODE::FOLLOW) { return; }

	// 衝突判定
	Collision();

	// 地面下に移動制限を掛ける
	if (transform_.pos.y < RESOLVE_FOLLOW_POS_MIN_Y)
	{
		transform_.pos.y = RESOLVE_FOLLOW_POS_MIN_Y;
	}
}

void Camera::SetTrackingTarget(const VECTOR& fromPos, const VECTOR& fromDir, const VECTOR& fromTarget
	, const VECTOR& toPos, const VECTOR& toDir, const VECTOR& toTarget, float easingTime)
{
	SetCameraMode(MODE::TRACKING);
	easing_.term = 0.0f;

	// 開始地点
	transform_.quaRot = Quaternion::Euler(UtilityMath::VNormalize(fromDir));
	easing_.fromPosition = fromPos;
	easing_.fromTarget   = fromTarget;

	// 終了地点
	easing_.toPosition = toPos;
	easing_.toRotation = Quaternion::Euler(UtilityMath::VNormalize(toDir));
	easing_.toTarget   = toTarget;

	// イージング時間
	easing_.endTime = easingTime;
}

void Camera::SetTrackingTarget(const VECTOR& toPos, const VECTOR& toDir, const VECTOR& toTarget, float easingTime)
{
	SetCameraMode(MODE::TRACKING);

	easing_.term = 0.0f;

	// 開始地点
	easing_.fromPosition = transform_.pos;
	easing_.fromTarget = targetPos_;

	// 終了地点
	easing_.toPosition = toPos;
	easing_.toRotation = Quaternion::Euler(UtilityMath::VNormalize(toDir));
	easing_.toTarget   = toTarget;

	// イージング時間
	easing_.endTime = easingTime;
}

VECTOR Camera::EasingChangeTarget(const VECTOR& fromVec, const VECTOR& toVec, float term)
{
	/* QuadInOut イージング
	  t < 0.5 : 加速フェーズ (2t^2)
	  t >= 0.5: 減速フェーズ (1 - (-2t+2)^2 / 2)
   */
	term = std::clamp(term, 0.0f, 1.0f);

	// 開始イージング
	const float QUAD_IN = (2.0f * term * term);

	// 終了イージング
	const float QUAD_OUT = (1.0f - (-2.0f * term + 2.0f) * (-2.0f * term + 2.0f) / 2.0f);

	// イージング位置が中間を超えたらイージング方法を切り替える
	const float MIDDLE = 0.5f;
	const float easedT = ((term < MIDDLE) ? QUAD_IN : QUAD_OUT);

	return VGet(fromVec.x + (toVec.x - fromVec.x) * easedT
		, fromVec.y + (toVec.y - fromVec.y) * easedT
		, fromVec.z + (toVec.z - fromVec.z) * easedT);
}
