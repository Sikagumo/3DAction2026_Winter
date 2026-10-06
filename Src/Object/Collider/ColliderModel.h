#pragma once
#include "./ColliderBase.h"
#include <vector>
#include <string>

class ColliderModel : public ColliderBase
{
public:

	/// @brief コンストラクタ
	/// @param tag 当たり判定のタグ
	/// @param follow 追従対象
	ColliderModel(TAG tag, const Transform * follow);

	~ColliderModel(void) override = default;


	/// @brief 指定された文字を含むフレームを衝突判定から除外
	/// @param frameName フレーム名
	void AddExcludeFrameIds(const std::string& frameName);

	/// @brief 衝突判定から除外するフレームをクリアする
	void ClearExcludeFrame(void);

	/// @brief 除外フレーム判定
	/// @param frameIdx フレーム番号
	bool IsExcludeFrame(int frameIdx) const;

	/// @brief 指定された文字を含むフレームを衝突判定対象とする
	/// @param frameName フレーム名
	void AddTargetFrameIds(const std::string& frameName);

	/// @brief 衝突判定の対象するフレームをクリアする
	void ClearTargetFrame(void);

	/// @brief 対象フレーム判定
	bool IsTargetFrame(int _frameIdx) const;

	/// @brief 線分とモデルの最近接(startに近い)衝突ポリゴンを取得
	/// @param start 線分始点
	/// @param end 線分終点
	/// @param isExclude 除外リストを無視するか否か 
	/// @param isTarget 対象フレーム以外は無視するか否か
	MV1_COLL_RESULT_POLY GetNearestHitPolyLine(
		const VECTOR& start,
		const VECTOR& end,
		bool isExclude = false, bool isTarget = false) const;


protected:

	// 衝突判定から除外するフレーム番号
	std::vector<int> excludeFrameIds_;

	// 衝突判定の対象とするフレーム番号
	std::vector<int> targetFrameIds_;


	void DrawDebug(int color) override {}

};