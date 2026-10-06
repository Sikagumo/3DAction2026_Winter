#pragma once
#pragma once
#include <DxLib.h>
class Transform;

class ColliderBase
{
public:

	// 形状
	enum class SHAPE
	{
		NONE,
		LINE,
		SPHERE,
		CAPSULE,
		MODEL,
	};

	// 衝突種別
	enum class TAG
	{
		STAGE,
		PLAYER,
		CAMERA,
		ENEMY,
		VIEW_RANGE,
	};


	/// @brief コンストラクタ
	/// @param shape 当たり判定の形状
	/// @param tag 当たり判定のタグ
	/// @param follow 追従対象
	ColliderBase(SHAPE shape, TAG tag, const Transform* follow);

	virtual ~ColliderBase(void) = default;

	void Draw(void);

	/// @brief 追従先取得
	const Transform* GetFollow(void) const { return follow_; }

	/// @brief 追従先の再設定
	void SetFollow(Transform* follow) { follow_ = follow_; }

	/// @brief 当たり判定の形状取得
	SHAPE GetShape(void) const { return shape_; }

	/// @brief タグの種類取得
	TAG GetTag(void) const { return tag_; }


protected:

	// デバッグ表示の色
	static constexpr int COLOR_VALID = 0xff0000;
	static constexpr int COLOR_INVALID = 0xaaaaaa;

	// 形状
	SHAPE shape_;

	// 衝突種別
	TAG tag_;

	// 追従先
	const Transform* follow_;

	// 有効フラグ
	bool isValid_;


	// ローカル座標をワールド座標に変換
	VECTOR GetRotPos(const VECTOR& localPos) const;

	// デバッグ用描画
	virtual void DrawDebug(int color) = 0;
};
