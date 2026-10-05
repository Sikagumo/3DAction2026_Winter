#pragma once

#include "./Vector2.h"

class Performance
{
public:

	// ヒットストップ フレーム数
	static constexpr int HIT_STOP = 30;

	// 振動演出を行う最小カウント
	static constexpr int SHAKE_STOP_COUNT = 10;

	// 画面揺れ回数 切替間隔
	static constexpr int SHAKE_COUNT_X = 2;
	static constexpr int SHAKE_COUNT_Y = 4;

	// 揺れの幅
	static constexpr int SHAKE_WIDTH_X = 4;
	static constexpr int SHAKE_WIDTH_Y = 4;

	// 遅延カウンタ 初期値
	static constexpr int SLOW_COUNT = 30;

	// 遅延間隔（Nフレームに1回更新）
	static constexpr int SLOW_INTERVAL = 5;


	Performance(void);
	~Performance(void) = default;

	void Initialize(void);
	void Update(void);

	/// @brief ヒットストップ(強)
	void StartHitStrong(void);

	/// @brief ヒットストップ(弱)
	void StartHitWeak(void);

	/// @brief ヒットストップ中か否か
	[[nodiscard("判定として利用してください")]]
	bool IsHitStop(void) const;

	/// @brief 遅延処理で現在フレームの更新を飛ばすか否か
	[[nodiscard("判定として利用してください")]]
	bool IsSkipFrame(void) const;

	/// @brief 画面描画オフセット取得
	/// @return 画面描画位置(振動なしなら0.0)
	[[nodiscard("位置を利用してください")]]
	Vector2 GetDrawOffset(void) const;


private:

	// ヒットストップカウンタ
	int hitStopCnt_ = 0;

	// 画面遅延カウンタ
	int slowCounter_ = 0;

	// 揺れ数カウンタ
	int shakeCounterX_ = 0;
	int shakeCounterY_ = 0;

	// 振動位置
	Vector2 shakePos_;
};
