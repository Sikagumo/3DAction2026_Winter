#pragma once
#include <DxLib.h>
#include "ColliderBase.h"
class Transform;


class ColliderSphere : public ColliderBase
{
public:

	/// @brief コンストラクタ
	/// @param tag タグ
	/// @param follow 追従対象
	/// @param localPos 調整位置
	/// @param radius 当たり判定の半径
	ColliderSphere(TAG tag, const Transform* follow, const VECTOR& localPos, float radius);

	~ColliderSphere(void)override = default;


	/// @brief 親Transformからの相対位置を取得
	const VECTOR& GetLocalPos(void) const { return localPos_; }

	/// @brief 親Transformからの相対位置を割り当て
	void SetLocalPos(const VECTOR& localPos) { localPos_ = localPos; }

	/// @brief ワールド座標を取得
	VECTOR GetPos(void) const;

	/// @brief 半径
	float GetRadius(void) const { return radius_; };
	void SetRadius(float radius) { radius_ = radius; };

	/// @brief 指定された回数と距離で三角形の法線方向に押し戻した座標を取得
	/// @param hitColPoly 衝突したポリゴン情報
	/// @param maxTryCnt 反発試行回数
	/// @param pushDistance 反発量
	/// @return 押し戻し後の地点
	VECTOR GetPosPushBackAlongNormal(const MV1_COLL_RESULT_POLY& hitColPoly
		, int maxTryCnt, float pushDistance) const;


protected:

	void DrawDebug(int color) override;


private:

	// 親Transformからの相対位置(下側)
	VECTOR localPos_;

	// 半径
	float radius_;
};