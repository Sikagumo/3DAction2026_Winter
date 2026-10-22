#include "UtilityCollision.h"
#include <DxLib.h>
#include "../Common/Vector2.h"
#include "./UtilityMath.h"
#include "../Object/Collider/ColliderSphere.h"
#include "../Object/Collider/ColliderCapsule.h"


bool UtilityCollision::IsHitBoxToBox(const Vector2& pos1, const Vector2& _size1, const Vector2& pos2, const Vector2& _size2)
{
	/*　2D矩形当たり判定処理　*/
	bool isX = (pos1.x < (pos2.x + _size2.x)) && ((pos1.x + _size1.x) > pos2.x);
	bool isY = (pos1.y < (pos2.y + _size2.y)) && ((pos1.y + _size1.y) > pos2.y);

	return (isX && isY);
}
bool UtilityCollision::IsHitBoxToBox(const VECTOR& pos1, const VECTOR& _size1, const VECTOR& pos2, const VECTOR& _size2)
{
	/*　3D矩形当たり判定処理　*/

	bool isX = (pos1.x < (pos2.x + _size2.x)) && ((pos1.x + _size1.x) > _size2.x);
	bool isY = (pos1.y < (pos2.y + _size2.y)) && ((pos1.y + _size1.y) > _size2.y);
	bool isZ = (pos1.z < (pos2.z + _size2.z)) && ((pos1.z + _size1.z) > _size2.z);

	return (isX && isY && isZ);
}

bool UtilityCollision::IsHitCircleCollision2D(const Vector2& pos1, float capsuleRadius1, const Vector2& pos2, float capsuleRadius2)
{
	/*　2D円形当たり判定処理　*/

	// 両者の座標の距離を取得
	Vector2 dis = pos1;
	dis.x -= pos2.x;
	dis.y -= pos2.y;


	float midPos = ((dis.x * dis.x) + (dis.y * dis.y));
	float rad = (capsuleRadius1 + capsuleRadius2); // 両者の半径

	// 両者の距離が半径よりも短い時、true
	return (midPos <= (rad * rad));
}

bool UtilityCollision::IsHitSphereToSphere(const VECTOR& pos1, float capsuleRadius1,
	                                       const VECTOR& pos2, float capsuleRadius2)
{
	/*　3D円形同士の当たり判定処理　*/

	// 両者の座標の距離を取得
	VECTOR dis = pos1;

	// 座標を減算
	dis = VSub(dis, pos2);


	// ベクトル間の長さ
	float length = ((dis.x * dis.x) + (dis.y * dis.y) + (dis.z * dis.z));

	// 両者の半径
	float rad = (capsuleRadius1 + capsuleRadius2);

	// 両者の距離が半径よりも短い時、true
	return (length <= (rad * rad));
}

bool UtilityCollision::IsHitSphereToCapsule(const VECTOR& spherePos, float sphereRadius,
	                                        const VECTOR& capTopPos1, const VECTOR& capBottomPos2, float capRadius)
{
	/*　球体とカプセルの当たり判定　*/

	// 当たり判定半径が0の場合、false
	if (sphereRadius <= 0.0f || capRadius <= 0.0f) { return false; }

	bool ret = false;

	// カプセル球体の中心を繋ぐベクトル
	VECTOR cap1to2 = VSub(capBottomPos2, capTopPos1);

	// ベクトルを正規化
	VECTOR cap1to2ENor = VNorm(cap1to2);


	// カプセル繋ぎの単位ベクトルと、
	// そのベクトル元から球体へのベクトルの内積を取る
	float dot = VDot(cap1to2ENor, VSub(spherePos, capTopPos1));

	// 内積で求めた射影距離を使って、カプセル繋ぎ上の座標を取る
	VECTOR capRidePos = VAdd(capTopPos1, VScale(cap1to2ENor, dot));

	// カプセル繋ぎのベクトルの長さを取る
	float len = UtilityMath::Magnitude(cap1to2);

	// 球体がカプセル繋ぎ上にいるか判別するため、比率を取る
	float rate = (dot / len);

	VECTOR centerPos;


	// 球体の位置が３エリアに分割されたカプセル形状のどこにいるか判別
	if (rate > 0.0f && rate <= 1.0f)
	{
		// ①球体がカプセル繋ぎ上にいる
		centerPos = VAdd(capTopPos1, VScale(cap1to2ENor, dot));
	}
	else if (rate > 1.0f)
	{
		// ②球体がカプセルの終点側にいる
		centerPos = capBottomPos2;
	}
	else if (rate < 0.0f)
	{
		// ③球体がカプセルの始点側にいる
		centerPos = capTopPos1;
	}
	else
	{
		// ここにきてはいけない
		return false;
	}

	// 球体同士の当たり判定
	if (IsHitSphereToSphere(centerPos, capRadius, spherePos, sphereRadius))
	{
		ret = true;
	}

	return ret;
}
bool UtilityCollision::IsHitSphereToCapsule(const ColliderSphere& sphere, const ColliderCapsule& capsule)
{
	return (IsHitSphereToCapsule(sphere.GetPos(), sphere.GetRadius()
		, capsule.GetPosTop(), capsule.GetPosDown(), capsule.GetRadius()));
}

bool UtilityCollision::IsHitCapsuleToCapsule(const VECTOR& capsuleTop1, const VECTOR& capBottom1, float capsuleRadius1,
	                                         const VECTOR& capsuleTop2, const VECTOR& capBottom2, float capsuleRadius2)
{
	bool ret = false;

	// 最短距離
	VECTOR near1, near2;

	float dis = _CapPointBitween(
		capsuleTop1, capBottom1, near1,
		capsuleTop2, capBottom2, near2);

	// カプセルの最短距離が半径の合計以下の時、衝突
	if (dis <= (capsuleRadius1 + capsuleRadius2))
	{
		ret = true;
	}
	return ret;
}
bool UtilityCollision::IsHitCapsuleToCapsule(const ColliderCapsule& capsule1, const ColliderCapsule& capsule2)
{
	return IsHitCapsuleToCapsule(capsule1.GetLocalPosTop(), capsule1.GetLocalPosDown(), capsule1.GetRadius()
		, capsule2.GetLocalPosTop(), capsule2.GetLocalPosDown(), capsule2.GetRadius());
}

void UtilityCollision::CollisionsReflectXZ(VECTOR& _targetPos1, float _targetRad1, VECTOR& _targetPos2, float _targetRad2)
{
	VECTOR target1XZ = { _targetPos1.x, 0.0f, _targetPos1.z };
	VECTOR target2XZ = { _targetPos2.x, 0.0f, _targetPos2.z };;
	// 反発方向
	VECTOR target1RefDir = VNorm(VSub(target2XZ, target1XZ));
	VECTOR target2RefDir = VNorm(VSub(target1XZ, target2XZ));

	// オブジェクト間の距離
	float size = VSize(VSub(target1XZ, target2XZ));

	float offset = ((_targetRad1 + _targetRad2) - abs(size));

	// 両者を均等に反発させる
	target1XZ = VAdd(target1XZ, VScale(target1RefDir, (offset / 2)));
	target2XZ = VAdd(target2XZ, VScale(target2RefDir, (offset / 2)));

	// 位置を反映
	_targetPos1 = { target1XZ.x, _targetPos1.y, target1XZ.z };
	_targetPos2 = { target2XZ.x, _targetPos2.y, target2XZ.z };
}
const VECTOR& UtilityCollision::CollisionReflectXZ(float _targetPosY, const VECTOR& _fromPos, float _fromRad, const VECTOR& _toPos, float _toRad)
{
	VECTOR ret = UtilityMath::VECTOR_ZERO;

	// オブジェクト間の距離
	VECTOR diff = VSub(_fromPos, _toPos);
	diff.y = 0.0f;

	// 反発方向
	VECTOR refDir = VNorm(diff);

	float size = VSize(diff);
	/*
	// 反発する値(オブジェクト同士の半径-距離)
	float offset = ((_fromRad + _toRad) - size)	;

	if (offset > 0.0f)
	{
		// 横反発方向を正規化
		VECTOR refDirXZ = VNorm(diff);
		ret = VScale(refDirXZ, offset);
		ret.y = 0.0f;
	}*/

	// 反発後の位置を割り当て
	ret = VAdd(refDir, ret);
	ret.y = _targetPosY;

	return ret;
}

float UtilityCollision::_CapPointBitween(const VECTOR& capsuleTop1, const VECTOR& capBottom1, VECTOR& capCtc1,
	                                     const VECTOR& capsuleTop2, const VECTOR& capBottom2, VECTOR& capCtc2)
{
	/*　カプセル同士の当たり判定　*/


	bool ret = false;

	// カプセル１の方向ベクトル
	VECTOR capVec1 = VSub(capsuleTop1, capBottom1);

	// カプセル２の方向ベクトル
	VECTOR capVec2 = VSub(capsuleTop2, capBottom2);

	// ２つの半径の合計
	VECTOR radius = VSub(capsuleTop1, capsuleTop2);


	// 内積
	float capLine1 = VDot(capVec1, capVec1);
	float capLine2 = VDot(capVec2, capVec2);
	float capRad = VDot(capVec2, radius);


	float s, t;

	// 両方の線分が点の場合(計算誤差を考慮)
	if (capLine1 <= 1e-6f && capLine2 <= 1e-6f)
	{
		s = t = 0.0f;

		capCtc1 = capsuleTop1;
		capCtc2 = capsuleTop2;

		// ベクトル同士の接点同士の距離を返す
		return VSize(VSub(capCtc1, capCtc2));
	}

	// 線分１が点の場合(計算誤差を考慮)
	if (capLine1 <= 1e-6f)
	{
		s = 0.0f;
		t = (capRad / capLine1);


		t = fmaxf(0.0f, fminf(1.0f, t));
	}

	// 線分２が点の場合(計算誤差を考慮)
	else if (capLine2 <= 1e-6f)
	{
		t = 0.0f;
		s = -VDot(capVec1, radius) / capLine1;

		t = fmaxf(0.0f, fminf(1.0f, s));
	}

	// 通常時
	else
	{
		float b = VDot(capVec1, capVec2);

		float denom = (capLine1 * capLine2) - (b * b);

		if (denom != 0)
		{
			s = fmaxf(0.0f, fminf(1.0f, (b * capRad - VDot(capVec1, radius) * capLine2)));
		}
		else
		{
			s = 0.0f;
		}

		t = (b * s + capRad) / capLine2;

		if (t < 0.0f)
		{
			t = 0.0f;
			s = fmaxf(0.0f, fminf(1.0f, -VDot(capVec1, radius) / capLine1));
		}
		else if (t > 1.0f)
		{
			t = 1.0f;
			s = fmaxf(0.0f, fminf(1.0f, (b - VDot(capVec1, radius)) / capLine1));
		}
	}

	capCtc1 = VAdd(capsuleTop1, VScale(capVec1, s));
	capCtc2 = VAdd(capsuleTop2, VScale(capVec2, t));

	return VSize(VSub(capCtc1, capCtc2));
}