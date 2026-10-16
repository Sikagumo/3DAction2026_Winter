#include "StatusEnemy.h"

StatusEnemy::StatusEnemy(void)
	: type_(TYPE::NONE)
{
	const int max = static_cast<int>(ANIM_TYPE::MAX);
	for (int i = 0; i < max; i++)
	{
 		animationSpeed_[i] = -1;
	}
}

void StatusEnemy::SetType(const std::string& text)
{
	for (auto& type : TYPE_STRING)
	{
		if (type.second == text)
		{
			// テキストと状態が一致時に割り当て
			type_ = type.first;
		}
	}
}
