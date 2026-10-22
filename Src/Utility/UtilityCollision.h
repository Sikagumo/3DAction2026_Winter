#pragma once
#include "../Common/Vector2.h"
#include <DxLib.h>
class ColliderSphere;
class ColliderCapsule;

class UtilityCollision
{
public:

	
	/// @brief 2D矩形当たり判定
	/// @param _pos1 対象１の2点座標
	/// @param _size1 対象１のスケール
	/// @param _pos2 対象２の2点座標
	/// @param _size2 対象２のスケール
	static bool IsHitBoxToBox(const Vector2& _pos1, const Vector2& _size1, const Vector2& _pos2, const Vector2& _size2);

	/// @brief ボックス同士の当たり判定
	/// @param _pos1 対象１の中央座標
	/// @param _size1 対象１のスケール
	/// @param _pos2 対象２の中央座標
	/// @param _size2 対象２のスケール
	static bool IsHitBoxToBox(const VECTOR& _pos1, const VECTOR& _size1, const VECTOR& _pos2, const VECTOR& _size2);


	/// @brief 2D円形当たり判定
	/// @param _pos1 対象１の座標
	/// @param capsuleRadius1 対象１の半径
	/// @param _pos2 対象２の座標
	/// @param capsuleRadius2 対象２の半径
	static bool IsHitCircleCollision2D(const Vector2& _pos1, float capsuleRadius1, const Vector2& _pos2, float capsuleRadius2);

	
	/// @brief 球形同士の当たり判定
	/// @param _pos1 対象１の座標
	/// @param capsuleRadius1 対象１の半径
	/// @param _pos2 対象２の座標
	/// @param capsuleRadius2 対象２の半径
	static bool IsHitSphereToSphere(const VECTOR& _pos1, float capsuleRadius1, const VECTOR& _pos2, float capsuleRadius2);

	/// @brief 球体とカプセルの衝突判定
	/// @param _sphPos 球体の位置
	/// @param _sphRadius 球体の半径
	/// @param capPos1 カプセルの位置１
	/// @param capPos2 カプセルの位置２
	/// @param capRadius カプセルの半径
	static bool IsHitSphereToCapsule(const VECTOR& _sphPos, float _sphRadius,
		                             const VECTOR& capPos1, const VECTOR& capPos2, float capRadius);

	/// @brief 球体とカプセルの衝突判定
	/// @param sphere 球
	/// @param capsule カプセル
	static bool IsHitSphereToCapsule(const ColliderSphere& sphere, const ColliderCapsule& capsule);

	/// @brief カプセル同士の衝突判定
	/// @param capTop1 カプセル１の上位置
	/// @param capBottom1 カプセル１の下位置
	/// @param capsuleRadius1 カプセル１の半径
	/// @param capTop2 カプセル２の上位置
	/// @param capBottom2 カプセル２の下位置
	/// @param capsuleRadius2 カプセル２の半径
	static bool IsHitCapsuleToCapsule(const VECTOR& capTop1, const VECTOR& capBottom1, float capsuleRadius1,
		                              const VECTOR& capTop2, const VECTOR& capBottom2, float capsuleRadius2);

	/// @brief カプセル同士の衝突判定
	/// @param capsule1 カプセル１
	/// @param capsule2 カプセル２
	static bool IsHitCapsuleToCapsule(const ColliderCapsule& capsule1, const ColliderCapsule& capsule2);

	/// @brief 横反発処理(両者を反発)
	/// @param _targetPos1 対象１の位置
	/// @param _targetRad1 対象１の半径
	/// @param _targetPos2 対象２の位置
	/// @param _targetRad2 対象２の半径
	static void CollisionsReflectXZ(VECTOR& _targetPos1, float _targetRad1, VECTOR& _targetPos2, float _targetRad2);

	/// @brief 横反発処理(反発対象のみ反発)
	/// @param _targetPos 対象の位置
	/// @param _fromRad 反発対象の半径
	/// @param _toPos 反発しない対象の位置
	/// @param _toRad 反発しないの半径
	/// @returns 反発ベクトル
	static const VECTOR& CollisionReflectXZ(float _targetPosY, const VECTOR& _fromPos, float _fromRad, const VECTOR& _toPos, float _toRad);


private:

	/// @brief カプセル同士の当たり判定
	/// @param capTop1 カプセル１の上位置
	/// @param capBottom1 カプセル１の下位置
	/// @param capCtc1 カプセル１の接点
	/// @param capTop2 カプセル２の上位置
	/// @param capBottom2 カプセル２下位置
	/// @param capCtc2 カプセル２の接点
	static float _CapPointBitween(const VECTOR& capTop1, const VECTOR& capBottom1, VECTOR& capCtc1,
		                          const VECTOR& capTop2, const VECTOR& capBottom2, VECTOR& capCtc2);
};