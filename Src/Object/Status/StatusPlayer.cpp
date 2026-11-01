#include "StatusPlayer.h"

StatusPlayer::StatusPlayer(void)
	: playerParam_{}
{
	action_.clear();
}

void StatusPlayer::SetAnimationSpeed(float idle, float walk, float run)
{
	animationSpeedIdle_ = idle;
	animationSpeedWalk_ = walk;
	animationSpeedRun_ = run;
}