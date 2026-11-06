#include "UtilityMath.h"
#include "../Object/Common/Transform.h"
#include "ModelFrameUtility.h"

void ModelFrameUtility::GetFrameWorldMatrix(
	int modelId, int frameIdx, VECTOR& scl, MATRIX& matRot, VECTOR& pos)
{

	// 対象フレームのローカル座標からワールド座標に変換する行列を得る
	// ( 大きさ、回転、位置 )
	auto mat = MV1GetFrameLocalWorldMatrix(modelId, frameIdx);

	// 拡大縮小成分
	scl = MGetSize(mat);

	// 回転成分＋拡大縮小成分
	matRot = MGetRotElem(mat);

	// 回転成分のみにする
	auto revScl = VGet(1.0f / scl.x, 1.0f / scl.y, 1.0f / scl.z);
	matRot = MMult(matRot, MGetScale(revScl));

	// 移動成分
	pos = MGetTranslateElem(mat);

}

void ModelFrameUtility::SetFrameWorldMatrix(
	const Transform& follow, int followFrameIdx, 
	Transform& target, VECTOR localPos, VECTOR localRot)
{

	// 対象フレームのローカル座標からワールド座標に変換する行列を得る
	// ( 大きさ、回転、位置 )
	MATRIX worldMatMix = 
		MV1GetFrameLocalWorldMatrix(follow.modelId, followFrameIdx);

	// 回転行列を取り出す際に、大きさを1に戻す
	VECTOR scl = follow.scl;
	scl = VGet(1.0f / scl.x, 1.0f / scl.y, 1.0f / scl.z);
	MATRIX rotMat = MMult(MGetRotElem(worldMatMix), MGetScale(scl));

	// 取り出した回転に、指定された回転を加える
	// 軸は取り出した回転からみたローカルのX軸、Y軸、Z軸とする

	// 対象フレームのX軸回転
	rotMat = MMult(rotMat,
		MGetRotAxis(
			VNorm(VTransformSR(UtilityMath::DIR_RIGHT, worldMatMix)),
			localRot.x
		)
	);

	// 対象フレームのY軸回転
	rotMat = MMult(rotMat,
		MGetRotAxis(
			VNorm(VTransformSR(UtilityMath::DIR_UP, worldMatMix)),
			localRot.y
		)
	);

	// 対象フレームのZ軸回転
	rotMat = MMult(rotMat,
		MGetRotAxis(
			VNorm(VTransformSR(UtilityMath::DIR_FORWARD, worldMatMix)),
			localRot.z
		)
	);

	// 回転行列反映
	target.matRot = rotMat;

	// 対象フレームのワールド座標を取得
	VECTOR followFramePos = 
		MV1GetFramePosition(follow.modelId, followFrameIdx);

	// 指定されたローカル座標を回転させ、ワールド座標に加算する
	target.pos = 
		VAdd(followFramePos, VTransform(localPos, target.matRot));

}
