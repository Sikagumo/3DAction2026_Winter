#pragma once
#include "SceneBase.h"
#include <memory>
#include <vector>
#include <DxLib.h>
//#include "../Object/Shield.h"
//class Stage;
class SkyDome;
class Player;
//class PixelMaterial;
//class PixelRenderer;
//class Shield;
//class PointLightObj;
//class RimPlanet;

class GameScene : public SceneBase
{
public:
	
	// ポストエフェクトモード
	enum class MODE
	{
		MAIN,
		MONO,
		SCAN,
		LENS,
		VINE,
		MAX
	};

	GameScene(void);
	~GameScene(void)override;

	void Initialize(void) override;
	void Update(void) override;
	void Draw(void) override;


private:

	// ステージ
	//std::unique_ptr<Stage> stage_ = nullptr;

	// プレイヤー
	std::unique_ptr<Player> player_ = nullptr;

	// スカイドーム
	std::unique_ptr<SkyDome> skyDome_ = nullptr;

	// ポストエフェクトモード
	MODE mode_;

	// ポストエフェクト用スクリーン
	//int postEffectScreen_ = -1;
	
	// ポストエフェクト用(モノクロ)
	//std::unique_ptr<PixelMaterial> monoMaterial_;
	//std::unique_ptr<PixelRenderer> monoRenderer_;

	/// @brief シェーダーを画面に割り当て
	void SetShaderScreen(void);
};
