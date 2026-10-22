#include "StatusEnemy.h"

StatusEnemy::StatusEnemy(void)
	: enemyParam_{}
{
	const int max = static_cast<int>(ANIM_TYPE::MAX);
	for (int i = 0; i < max; i++)
	{
 		enemyParam_.animationSpeed[i] = -1;
	}
}

void StatusEnemy::SetType(const std::string& text)
{
	for (auto& type : TYPE_STRING)
	{
		if (type.second == text)
		{
			// テキストと状態が一致時に割り当て
			enemyParam_.type = type.first;
		}
	}
}
