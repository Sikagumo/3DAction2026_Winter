#pragma once
#include <DxLib.h>
#include "ColliderBase.h"
class Transform;
class ColliderModel;

class ColliderCapsule : public ColliderBase
{
public:

	/// @brief コンストラクタ
	/// @param tag 指定対象
	/// @param follow 追従対象
	/// @param localPosTop 始点位置
	/// @param localPosDown 終点位置
	/// @param radius 当たり判定の半径
	ColliderCapsule(TAG tag, const Transform* follow,
					const VECTOR& localPosTop, const VECTOR& localPosDown, float radius);

	~ColliderCapsule(void) = default;


	// 親Transformからの相対位置取得
	const VECTOR& GetLocalPosTop(void) const { return localPosTop_; }
	const VECTOR& GetLocalPosDown(void) const { return localPosDown_; }

	// 親Transformからの相対位置を割り当て
	void SetLocalPosTop(const VECTOR& pos) { localPosTop_ = pos; }
	void SetLocalPosDown(const VECTOR& pos) { localPosDown_ = pos; }

	/// @brief ワールド座標取得
	VECTOR GetPosTop(void) const;
	VECTOR GetPosDown(void) const;

	/// @brief 始点終点の半径
	float GetRadius(void) const { return radius_; }
	void SetRadius(float _radius) { radius_ = _radius; }

	/// @brief 始点終点間の高さ取得
	float GetHeight(void) const { return localPosTop_.y; }

	/// @brief カプセルの中心座標取得
	VECTOR GetCenter(void) const;

	// 指定された回数と距離で三角形の法線方向に押し戻した座標を取得
	VECTOR GetPosPushBackAlongNormal(
		const MV1_COLL_RESULT_POLY& hitColPoly,
		int maxTryCnt,
		float pushDistance) const;

	// 指定された回数と距離で三角形の法線方向に押し戻す
	void PushBackAlongNormal(
		const ColliderModel* _colliderModel, Transform* _transform,
		int _maxTryCnt, float _pushDistance,
		bool isExclude = false, bool isTarget = false) const;
	
	/// @brief モデルと衝突しているか否か
	/// @param colliderModel 対象のモデル
	/// @param isExclude 除外リストを無視するか否か 
	/// @param isTarget 対象フレーム以外は無視するか否か
	bool IsHit(const ColliderModel* colliderModel,
				bool isExclude = false, bool isTarget = false) const;


protected:

	void DrawDebug(int color) override;


private:

	// 親Transformからの相対位置(上側)
	VECTOR localPosTop_;

	// 親Transformからの相対位置(下側)
	VECTOR localPosDown_;

	// 半径
	float radius_;
};
