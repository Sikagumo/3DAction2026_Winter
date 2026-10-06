#pragma once
#include <DxLib.h>
#include "./ColliderBase.h"
class Transform;

class ColliderLine : public ColliderBase
{
public:

	// コンストラクタ
	ColliderLine(TAG tag, const Transform* follow,
				 const VECTOR& localPosStart, const VECTOR& localPosEnd);

	// デストラクタ
	~ColliderLine(void) override = default;


	/// @brief ローカル座標を割り当て
	void SetLocalPosStart(const VECTOR& posStart) { localPosStart_ = posStart; }
	void SetLocalPosEnd(const VECTOR& posEnd) { localPosEnd_ = posEnd; }

	/// @brief ローカル座標の取得
	const VECTOR& GetLocalPosStart(void) const { return localPosStart_;  }
	const VECTOR& GetLocalPosEnd(void) const { return localPosEnd_;  }

	/// @brief ワールド座標の取得
	VECTOR GetPosStart(void) const;
	VECTOR GetPosEnd(void) const;


protected:

	void DrawDebug(int color) override;

private:

	// 線分の開始座標(ローカル)
	VECTOR localPosStart_;

	// 線分の終了座標(ローカル)
	VECTOR localPosEnd_;
};