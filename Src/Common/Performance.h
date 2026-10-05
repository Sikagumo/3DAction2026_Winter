#pragma once

#include "./Vector2.h"

class Performance
{
public:

	// ヒットストップ時間
	static constexpr float HIT_STOP_TIME = 0.5f;

	// 振動演出を行う最小カウント
	static constexpr float SHAKE_STOP_TIME = 0.17f;

	// 画面揺れ回数 切替間隔
	static constexpr int SHAKE_COUNT_X = 2;
	static constexpr int SHAKE_COUNT_Y = 4;

	// 揺れの幅
	static constexpr int SHAKE_WIDTH_X = 4;
	static constexpr int SHAKE_WIDTH_Y = 4;

	// 遅延カウンタ 初期値
	static constexpr int SLOW_TIME = 0.5f;

	// 遅延間隔（Nフレームに1回更新）
	static constexpr int SLOW_INTERVAL = 5;


	Performance(void);
	~Performance(void) = default;

	void Initialize(void);
	void Update(void);

	/// @brief ヒットストップ
	/// @param seconds 停止時間
	/// @param shakeWidthX 横振動値
	/// @param shakeWidthY 縦振動値
	/// @param slowTime 遅延時間
	void StartHitStop(float seconds, int shakeWidthX = SHAKE_WIDTH_X, int shakeWidthY = SHAKE_WIDTH_Y
		, float slowTime = SLOW_TIME);

	/// @brief ヒットストップ(強)
	void StartHitStrong(void);

	/// @brief ヒットストップ(弱)
	void StartHitWeak(void);

	/// @brief 遅延処理
	/// @param seconds 遅延時間
	/// @param interval フレームに1回だけ更新を掛ける値
	void StartHitSlow(float seconds, int interval = SLOW_INTERVAL);


	/// @brief スロー演出中か否か
	[[nodiscard("判定として利用してください")]]
	bool IsSlow(void) const;

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

	// 演出時間
	float hitStopTime_ = 0.0f;
	float slowTime_ = 0.0f;

	// 遅延の間引き判定用のフレームカウンタ
	int slowFrameCount_ = 0;
	int slowInterval_ = SLOW_INTERVAL;

	// 揺れ数カウンタ
	int shakeCounterX_ = 0;
	int shakeCounterY_ = 0;

	// 振動位置
	Vector2 shakePos_;

	void UpdateShake(void);
};
