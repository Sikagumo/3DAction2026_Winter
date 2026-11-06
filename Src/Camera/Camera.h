#pragma once
#include "../Object/Actor/ActorBase.h"
#include <DxLib.h>
#include <map>
#include <functional>
#include <memory>

class Transform;

class Camera : public ActorBase
{

public:

	// ロックオン切替イージング終了時間
	static constexpr float LOCKON_DURATION = 0.2f;

	// カメラのクリップ範囲
	static constexpr float VIEW_NEAR = 10.0f;
	static constexpr float VIEW_FAR = 20000.0f;

	// 追従位置からカメラ位置までの相対座標
	static constexpr VECTOR FOLLOW_LOCAL_POS = { 25.0f, 125.0f, -110.0f };
	static constexpr VECTOR FOLLOW_LOCAL_POS_LOCKON = { 60.0f, 45.0f, -150.0f };

	// 追従位置から注視点までの相対座標
	static constexpr VECTOR FOLLOW_TARGET_LOCAL_POS = { 50.0f, 75.0f, 50.0f };


	// カメラのX回転上限度角
	static constexpr float LIMIT_X_UP = 40.0f * (DX_PI_F / 180.0f);
	static constexpr float LIMIT_X_DOWN = 35.0f * (DX_PI_F / 180.0f);
	
	/// @brief カメラモード
	enum class MODE
	{
		NONE = -1,   // 無効状態

		FIXED_POINT, // 定点カメラ
		FREE,        // フリー移動カメラ
		FOLLOW,      // プレイヤー追従
		TRACKING,    // トラッキング用カメラ
	};

	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		SPHERE,
		MAX,
	};

	/// @brief ロックオン対象の情報(対象は常に1つだけ)
	struct TargetParam
	{
		VECTOR pos = { 0.0f, 0.0f, 0.0f };
		int hp = 0;
		int maxHp = 0;
	};

	Camera(void);
	~Camera(void)override = default;

	void Update(void)override;

	/// @brief 描画前のカメラ設定
	void SetBeforeDraw(void);

	/// @brief デバッグ用描画
	void DrawDebug(void);

	/// @brief 座標の取得
	const VECTOR& GetPos(void) const { return transform_.pos; };

	/// @brief 角度の取得
	const VECTOR& GetAngles(void) const { return angles_;  };

	/// @brief X回転を抜いたY軸のみのカメラ角度
	const Quaternion& GetQuaRotY(void) const { return rotY_; };
	
	/// @brief 注視点の取得
	const VECTOR& GetTargetPos(void) const { return targetPos_;  };

	/// @brief カメラの前方方向取得
	VECTOR GetForward(void) const;

	/// @brief カメラモードの変更
	/// @param mode 遷移後のモード
	void SetCameraMode(MODE mode);

	/// @brief 追従対象の設定
	/// @param _follow 追従対象
	void SetFollow(const Transform* _follow) { followTransform_ = _follow; };

	/// @brief クォータニオン角度の割り当て
	void SetQuaternionRot(Quaternion _rot) { transform_.quaRot = _rot; }

	/// @brief ロックオン対象割り当て
	/// @param targetPos 追従位置
	/// @param targetHp 追従位置
	void SetLockOnTarget(const VECTOR& targetPos, int targetHp);

	/// @brief ロックオン対象を破棄する(ロックオン中なら解除も行う)
	void ClearLockOnTarget(void);

	/// @brief ロックオン対象の有効性確認と、無効時のロックオン解除 
	void FollowLockOnPosition(void);

	// ロックオンするか否かの設定
	void SetIsLockOn(bool _isLockOn);
	bool GetIsLockOn(void)const { return isLockOn_; };

	/// @brief ロックオン中の対象の位置を取得
	const VECTOR& GetLockOnPos(void)const { return lockOnParam_.pos; };
	int GetLockOnHp(void)const { return lockOnParam_.hp; };
	int GetLockOnMaxHp(void)const { return lockOnParam_.maxHp; };

	/// @brief イージング遷移中か否か
	bool IsEasingState(void)const { return (easing_.term < 1.0f); };

	/// @brief 押し返し処理
	void ResolveCollision(void);
	
	/// @brief トラッキング対象の設定
	/// @param fromPos 開始位置
	/// @param fromDir 開始方向
	/// @param fromTarget 開始注視点位置
	/// @param toPos 終了位置
	/// @param toDir 終了方向
	/// @param toTarget 終了注視点位置
	/// @param easingTime イージング時間
	void SetTrackingTarget(const VECTOR& fromPos, const VECTOR& fromDir, const VECTOR& fromTarget
		, const VECTOR& toPos, const VECTOR& toDir, const VECTOR& toTarget, float easingTime);

	/// @brief トラッキング対象の設定
	/// @param toPos 終了位置
	/// @param toDir 終了方向
	/// @param toTarget 終了注視点位置
	/// @param easingTime イージング時間
	void SetTrackingTarget(const VECTOR& toPos, const VECTOR& toDir, const VECTOR& toTarget, float easingTime);


protected:

	void InitLoad(void) override {};

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override {};

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override {};

	// 初期化後の個別処理
	void InitPost(void) override;


private:

	//ボスの追従時の座標
	static constexpr VECTOR BOSS_CAMERA_POS = { 0,10,-500 };

	// 衝突時の押し戻し試行回数
	static constexpr int CNT_TRY_COLLISION_CAMERA = 30;

	// 衝突時の押し戻し量
	static constexpr float COLLISION_BACK_DIS = 2.0f;

	// 衝突判定用球体半径
	static constexpr float COL_CAPSULE_SPHERE = 30.0f;

	// カメラの補間移動率
	static constexpr float LERP_RATE_MOVE = 0.1f;

	// 壁衝突時の追従距離の最小倍率
	static constexpr float FOLLOW_DIST_MIN_SCALE = 0.5f;

	// 追従距離の補間速度
	static constexpr float FOLLOW_DIST_LERP_SPEED = 5.0f;


	// 更新処理
	std::function<void(void)> update_ = nullptr;

	// カメラの更新前位置
	VECTOR prePos_;

	// カメラが追従対象とするTransform
	const Transform* followTransform_;

	// カメラモード
	MODE mode_;

	// カメラ角度(rad)
	VECTOR angles_;

	// カメラ角度(Y軸のみ)
	Quaternion rotY_;

	// 注視点
	VECTOR targetPos_;

	// ターゲット
	TargetParam lockOnParam_;

	// 有効な対象が登録されているか否か
	bool hasTarget_ = false;

	// ロックオン中か否か
	bool isLockOn_ = false;

	struct EasingParam
	{
		// イージング経過値
		float term = 0.0f;

		// イージング終了時間
		float endTime = 0.0f;

		// イージング開始地点
		VECTOR fromPosition = {}; // カメラ位置
		VECTOR fromTarget   = {}; // 注視点

		// イージング終了地点
		VECTOR toPosition = {}; // カメラ位置
		VECTOR toTarget   = {}; // 注視点
		Quaternion toRotation = {}; // カメラ角度

	};
	EasingParam easing_;


	// 壁衝突時の追従距離倍率
	float followDistScale_;
	

	/// @brief カメラを初期位置に戻す
	void SetDefault(void);

	/// @brief 追従対象との位置同期を取る
	void SyncFollow(void);

	/// @brief アングルとY軸回転の同期処理
	void SyncAngleYFromRotY(void);

	/// @brief 対象が有効かつ範囲内で、ロックオン開始が可能か
	bool CanLockOn(void) const;

	/// @brief カメラ操作
	void ProcessRot(bool _isLimit);
	void ProcessMove(void);

	/// @brief カメラ回転
	/// @param _isLimit 
	void RotationMouse(bool _isLimit);
	void RotationGamePad(bool _isLimit);

	/// @brief モード別更新ステップ
	void SetBeforeDrawFixedPoint(void); // 定点カメラ
	void SetBeforeDrawFree(void);       // フリー移動カメラ
	void SetBeforeDrawFollow(void);     // プレイヤー追従カメラ
	void SetBeforeDrawTracking(void);   // トラッキングカメラ

	/// @brief 衝突判定
	void Collision(void);

	VECTOR EasingChangeTarget(const VECTOR& fromVec, const VECTOR& toVec, float term);
};
