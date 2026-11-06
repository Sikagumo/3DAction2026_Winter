#pragma once
#include <DxLib.h>
class Transform;

class ModelFrameUtility
{
public:

	/// @brief 対象フレームのワールド行列を大きさ・回転・位置に分解してを取得する
	/// @param modelId 
	/// @param frameIdx 
	/// @param scl 
	/// @param matRot 
	/// @param pos 
	static void GetFrameWorldMatrix(
		int modelId, int frameIdx, VECTOR& scl, MATRIX& matRot, VECTOR& pos);
	
	/// @brief 対象フレームの位置にtargetを配置し、対象フレームの回転に加え、指定した相対座標・回転を加える
	/// @param follow 追従する対象
	/// @param followFrameIdx 追従するフレーム番号
	/// @param target 追従させる対象
	/// @param localPos 追従させる対象のローカル座標
	/// @param localRot 追従させる対象のローカル回転 
	static void SetFrameWorldMatrix(
		const Transform& follow, int followFrameIdx
		, Transform& target, VECTOR localPos, VECTOR localRot);
};
