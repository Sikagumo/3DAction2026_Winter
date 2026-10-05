#include "Performance.h"
#define NOMINMAX
#include <DxLib.h>
#include <algorithm>
#include "../Utility/UtilityMath.h"
#include "../Manager/SceneManager.h"

Performance::Performance(void)
	: shakePos_(UtilityMath::VECTOR2_ZERO)
{
	Initialize();
}

void Performance::Initialize(void)
{
	hitStopTime_ = 0.0f;
	slowTime_    = 0.0f;
	slowFrameCount_ = 0;
	shakeCounterX_  = shakeCounterY_ = 0;
	shakePos_ = UtilityMath::VECTOR2_ZERO;
}

void Performance::Update(void)
{
	// どちらの演出も無ければ何もしない
	if (hitStopTime_ <= 0.0f && slowTime_ <= 0.0f) { return; }

	float deltaTime = SceneManager::GetInstance().GetDeltaTime();

	/* ヒットストップ (時間 + 画面振動) */
	if (hitStopTime_ > 0.0f)
	{
		hitStopTime_ = std::max(hitStopTime_ - deltaTime, 0.0f);
		UpdateShake();
	}

	/* スロー (時間 + 間引き用フレームカウンタ)*/
	if (slowTime_ > 0.0f)
	{
		slowTime_ = std::max(slowTime_ - deltaTime, 0.0f);
		slowFrameCount_++;
	}
}

void Performance::StartHitStop(float seconds, int shakeWidthX, int shakeWidthY, float slowTime)
{
	hitStopTime_ = std::max(seconds, 0.0f);

	slowTime_ = std::max(slowTime, 0.0f);
	slowFrameCount_ = 0;

	shakeCounterX_ = shakeCounterY_ = 0;
	shakePos_.x = shakeWidthX;
	shakePos_.y = shakeWidthY;
}

void Performance::StartHitStrong(void)
{
	/*　撃破ヒットストップ（強）　*/
	hitStopTime_   = HIT_STOP_TIME;
	shakeCounterX_ = shakeCounterY_ = 0;
	shakePos_.x    = SHAKE_WIDTH_X;
	shakePos_.y    = SHAKE_WIDTH_Y;
	slowTime_   = SLOW_TIME;
}

void Performance::StartHitWeak(void)
{
	/*　ヒットストップ（弱）　*/
	hitStopTime_ = (HIT_STOP_TIME / 2);
}

void Performance::StartHitSlow(float seconds, int interval)
{
	slowTime_ = std::max(seconds, 0.0f);
	slowInterval_ = std::max(interval, 1); // 0以下は剰余でゼロ除算になるため1以上
	slowFrameCount_ = 0;
}

bool Performance::IsSlow(void) const
{
	return (slowTime_ > 0.0f);
}

bool Performance::IsHitStop(void) const
{
	return (hitStopTime_ > 0.0f);
}

bool Performance::IsSkipFrame(void) const
{
	if (slowTime_ <= 0.0f) { return false; }

	return ((slowFrameCount_ % slowInterval_) != 0);
}

Vector2 Performance::GetDrawOffset(void) const
{
	if (hitStopTime_ > SHAKE_STOP_TIME)
	{
		return shakePos_;
	}

	Vector2 zero = UtilityMath::VECTOR2_ZERO;
	return zero;
}

void Performance::UpdateShake(void)
{
	// 画面揺れの間隔変数値増加
	++shakeCounterX_;
	++shakeCounterY_;

	/*　横揺れ　*/
	if (shakeCounterX_ >= SHAKE_COUNT_X)
	{
		shakeCounterX_ = 0;

		if (shakePos_.x == SHAKE_WIDTH_X)
		{
			shakePos_.x = -SHAKE_WIDTH_X;
		}
		else if (shakePos_.x == -SHAKE_WIDTH_X)
		{
			shakePos_.x = SHAKE_WIDTH_X;
		}
	}

	/*　縦揺れ　*/
	if (shakeCounterY_ >= SHAKE_COUNT_Y)
	{
		shakeCounterY_ = 0;

		if (shakePos_.y == SHAKE_WIDTH_Y)
		{
			shakePos_.y = -SHAKE_WIDTH_Y;
		}
		else if (shakePos_.y == -SHAKE_WIDTH_Y)
		{
			shakePos_.y = SHAKE_WIDTH_Y;
		}
	}
}
