#pragma once

class FrameRate
{
public:

	/// @brief デフォルトコンストラクタ
	FrameRate(void);

	/// @brief デフォルトデストラクタ
	~FrameRate(void) = default;


	/// @brief 更新処理
	void Update(void);

	/// @brief 描画処理
	void Draw(void);


	/// @brief フレームレート割り当て
	void SetFrameRate(void);

	/// @brief フレームレート制限 判定
	/// @returns 制御するか否か
	bool GetLimitFrameRate(void) { return ((curTime_ - lateTime_) < FRAME_RATE); };


private:

	// 平均フレームレート
	static constexpr int FRAME_RATE_AVE = 60;

	// 設定フレームレート(ミリ秒単位)
	static constexpr float FRAME_RATE = (1000 / FRAME_RATE_AVE);

	// テキスト横位置
	static constexpr int TEXT_POS_X = 70;

	// テキスト縦位置
	static constexpr int TEXT_POS_Y = 25;


	// 現在の時間
	int curTime_;

	// 前フレーム実行時間
	int lateTime_;

	// フレームカウンタ
	int frameCnt_;

	// フレーム更新時間
	int updateTime_;

	// フレームレート(描画用)
	float viewFramelate_;
};