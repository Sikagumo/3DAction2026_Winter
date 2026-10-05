#pragma once
#include <DxLib.h>
#include "../../Common/Quaternion.h"
#include "../../Utility/UtilityMath.h"

/// <summary>
/// モデル制御の基本情報
/// 大きさ：VECTOR基準
/// 回転　：Quaternion基準
/// 位置　：VECTOR基準
/// </summary>
class Transform
{

public:

	// モデルのハンドルID
	int modelId = -1;

	// 大きさ
	VECTOR scl;

	// 回転
	VECTOR rot;

	// 位置
	VECTOR pos;
	VECTOR localPos;
	VECTOR prePos;

	// 行列
	MATRIX matScl;
	MATRIX matRot;
	MATRIX matPos;

	// 回転
	Quaternion quaRot;

	// ローカル回転
	Quaternion quaRotLocal;


	Transform(void);
	~Transform(void) = default;

	/// @brief モデル制御の基本情報更新
	void Update(void);
	
	void DrawModelDir(void);

	/// @brief モデルのハンドルIDを設定
	void SetModel(int modelHandleId);

	/// @brief 数値初期
	void InitTransform(const VECTOR& scl,const Quaternion& rot, const Quaternion& rotLocal
						, const VECTOR& pos, const VECTOR& posLocal = UtilityMath::VECTOR_ZERO);
	void InitTransform(float scl,const Quaternion& rot, const Quaternion& rotLocal
						, const VECTOR& pos, const VECTOR& posLocal = UtilityMath::VECTOR_ZERO);
	void InitTransform(float scl,const Quaternion& rot, const Quaternion& rotLocal);
	void InitTransform(void);

	/// @brief 移動処理
	/// @param movePow 移動量
	void Translate(const VECTOR& movePow);
	void Translate(const VECTOR& dir, float movePow);

	/// @brief 回転処理
	/// @param axis 回転方向
	/// @param pow 速度
	void Rotate(const VECTOR& axis, float pow);
	void Rotate(const Quaternion& rot);

	/// @brief モデルのスケールを割り当て
	void SetScale(float scale);
	void SetScale(float scaleX, float scaleY, float scaleZ);

	/// @brief 前方方向を取得
	VECTOR GetForward(void) const;

	/// @brief 後方方向を取得
	VECTOR GetBack(void) const;

	/// @brief 右方向を取得
	VECTOR GetRight(void) const;

	/// @brief 左方向を取得
	VECTOR GetLeft(void) const;

	/// @brief 上方向を取得
	VECTOR GetUp(void) const;

	/// @brief 下方向を取得
	VECTOR GetDown(void) const;

	/// @brief 対象方向を取得
	VECTOR GetDir(const VECTOR& dir) const;

	void GetScale(float scale);
};
