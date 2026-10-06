#include "Stage.h"
#include <DxLib.h>
#include "../Utility/UtilityMath.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "Player.h"
#include "Planet.h"
#include "Common/Collider.h"
#include "Common/Transform.h"

Stage::Stage(std::unique_ptr<Player>& player)
	: player_(player)
{
}

Stage::~Stage(void)
{
}

void Stage::Initialize(void)
{

}

void Stage::Update(void)
{
}

void Stage::Draw(void)
{

}