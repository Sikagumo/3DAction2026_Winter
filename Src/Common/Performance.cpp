#include "Performance.h"
#include <DxLib.h>
#include "../Utility/UtilityMath.h"

Performance::Performance(void)
	: shakePos_(UtilityMath::VECTOR2_ZERO)
{
	Initialize();
}

void Performance::Initialize(void)
{
	hitStopCnt_    = 0;
	slowCounter_   = 0;
	shakeCounterX_ = shakeCounterY_ = 0;
	shakePos_ = UtilityMath::VECTOR2_ZERO;
}

void Performance::Update(void)
{
	if (hitStopCnt_ <= 0) { return; }

	// カウンタ減少
	hitStopCnt_--;
	slowCounter_--;

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

void Performance::StartHitStrong(void)
{
	/*　撃破ヒットストップ（強）　*/
	hitStopCnt_    = HIT_STOP;
	shakeCounterX_ = shakeCounterY_ = 0;
	shakePos_.x    = SHAKE_WIDTH_X;
	shakePos_.y    = SHAKE_WIDTH_Y;
	slowCounter_   = SLOW_COUNT;
}

void Performance::StartHitWeak(void)
{
	/*　ヒットストップ（弱）　*/
	hitStopCnt_ = (HIT_STOP / 2);
}

bool Performance::IsHitStop(void) const
{
	return (hitStopCnt_ > 0);
}

bool Performance::IsSkipFrame(void) const
{
	return ((slowCounter_ % SLOW_INTERVAL) != 0);
}

Vector2 Performance::GetDrawOffset(void) const
{
	if (hitStopCnt_ > SHAKE_STOP_COUNT)
	{
		return shakePos_;
	}

	Vector2 zero = UtilityMath::VECTOR2_ZERO;
	return zero;
}
