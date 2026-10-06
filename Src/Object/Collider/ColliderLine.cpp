#include "ColliderLine.h"
#include "../Common/Transform.h"

namespace
{
	// デバッグ表示の球体半径
	static constexpr float RADIUS = 5.0f;

	// デバッグ表示の球体ポリゴン分割数
	static constexpr int DIV_NUM = 6;
};

ColliderLine::ColliderLine(TAG tag, const Transform* follow,
						   const VECTOR& localPosStart, const VECTOR& localPosEnd)
	: ColliderBase(SHAPE::LINE, tag, follow)
	, localPosStart_(localPosStart)
	, localPosEnd_(localPosEnd)
{
}

VECTOR ColliderLine::GetPosStart(void) const
{
	return GetRotPos(localPosStart_);
}

VECTOR ColliderLine::GetPosEnd(void) const
{
	return GetRotPos(localPosEnd_);
}

void ColliderLine::DrawDebug(int color)
{
	VECTOR start = GetPosStart();
	VECTOR end = GetPosEnd();

	// 線分を描画
	DrawLine3D(start, end, color);

	// 始点・終点を球体で補助表示
	DrawSphere3D(start, RADIUS, DIV_NUM, color, color, true);
	DrawSphere3D(end, RADIUS, DIV_NUM, color, color, true);
}