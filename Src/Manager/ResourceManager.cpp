#include "ResourceManager.h"
#include "Resource.h"
#include <DxLib.h>
#include <string>
#include <unordered_map>
#include "../Application.h"
#include "../Utility/UtilityMath.h"

ResourceManager* ResourceManager::instance_ = nullptr;

// リソースファイルのパス
#ifdef _DEBUG
const std::string PATH_DATA = "Data/";

// 暗号化済みのリソースフォルダパス
#else

const std::string PATH_DATA = "ResourceData/";
//const std::string PATH_DATA = "Data/";
#endif


// ファイルパスの割り当て
const std::string ResourceManager::PATH_EFFECT    = PATH_DATA + "Effect/";
const std::string ResourceManager::PATH_IMAGE     = PATH_DATA + "Image/";
const std::string ResourceManager::PATH_MODEL     = PATH_DATA + "Model/";
const std::string ResourceManager::PATH_ANIMATION = PATH_DATA + "Model/Animation/";
const std::string ResourceManager::PATH_SE        = PATH_DATA + "Sound/SE/";
const std::string ResourceManager::PATH_BGM       = PATH_DATA + "Sound/BGM/";
const std::string ResourceManager::PATH_CSV       = PATH_DATA + "CSV/";
const std::string ResourceManager::PATH_MOVIE     = PATH_DATA + "Movie/";
const std::string ResourceManager::PATH_SHADER    = PATH_DATA + "Shader/";


void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}

	instance_->Initialize();
}

ResourceManager::ResourceManager(void)
{
	
}


void ResourceManager::Initialize(void)
{
	using LOAD_TYPE = Resource::LOAD_TYPE;

	/* 画像 */
	
	// タイトル画像
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMAGE_TITLE, PATH_IMAGE + "Title.png");

	// 丸影
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMAGE_SHADOW, PATH_IMAGE + "Shadow.png");
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMAGE_UI_PUSH_SPACE, PATH_IMAGE + "PushSpace.png");

	//_SetResource(LOAD_TYPE::IMAGE, SRC::IMAGE_INFO, PATH_IMAGE + "sousa.png");

	/* 複数画像 */

	
	// 画像枚数
	int imagesAllNum = 0;

	// １画像の横枚数
	int imagesNumX = 0, imagesNumY = 0;

	// プレイヤー対象HPバー
	imagesAllNum = 2;
	imagesNumX = 1;
	imagesNumY = 2;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_HP_PLAYER, PATH_IMAGE + "PlayerHpBer.png"
		, imagesAllNum, imagesNumX, imagesNumY);

	/*
	imagesAllNum = 11;
	imagesNumX = 1;
	imagesNumY = 11;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_TEXTS, PATH_IMAGE + "Text.png"
				, imagesAllNum, imagesNumX, imagesNumY);

	// タイトル文字
	imagesAllNum = 8;
	imagesNumX = 1;
	imagesNumY = 8;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_TITLE_TEXT, PATH_IMAGE + "TextsTitle.png"
				, imagesAllNum, imagesNumX, imagesNumY);

	// 選択文字
	imagesAllNum = 18;
	imagesNumX = 2;
	imagesNumY = 9;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_SELECT, PATH_IMAGE + "SelectImages.png"
				, imagesAllNum, imagesNumX, imagesNumY);

	// ゲームシーンの文字
	imagesAllNum = 1;
	imagesNumX = 1;
	imagesNumY = 1;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_GAME_TEXT, PATH_IMAGE + "TextsGame.png"
				, imagesAllNum, imagesNumX, imagesNumY);

	imagesAllNum = 4;
	imagesNumX = 1;
	imagesNumY = 4;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_RESULT, PATH_IMAGE + "EndText.png"
				, imagesAllNum, imagesNumX, imagesNumY);
	
	// 武器説明の文字
	imagesAllNum = 4;
	imagesNumX = 1;
	imagesNumY = 4;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_PLAYER_WEAPON_INFO, PATH_IMAGE + "WeaponInfo.png"
		, imagesAllNum, imagesNumX, imagesNumY);

	// ポーズシーンの文字
	imagesAllNum = 4;
	imagesNumX = 1;
	imagesNumY = 4;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_POUSE_TEX, PATH_IMAGE + "PauseNomal.png"
		, imagesAllNum, imagesNumX, imagesNumY);

	// ポーズシーンの選択時の文字
	imagesAllNum = 4;
	imagesNumX = 1;
	imagesNumY = 4;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_SELECT_PUSE_TEX, PATH_IMAGE + "PauseRain.png"
		, imagesAllNum, imagesNumX, imagesNumY);
	*/

	/* エフェクト */
	

	/* モデル */
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_SKYDOME, PATH_MODEL + "SkyDome/SkyDome.mv1");

	// プレイヤーモデル
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_PLAYER, PATH_MODEL + "Player/Player.mv1");

	// ステージモデル
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_STAGE, PATH_MODEL + "Stage/Stage.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_STAGE_COLLISION, PATH_MODEL + "Stage/StageCollision.mv1");

	// ボスモデル
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_BOSS, PATH_MODEL + "Boss/Boss.mv1");

	// 剣モデル
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_PLAYER_SWORD, PATH_MODEL + "Sword/Player/Sword.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_ENEMY_SWORD, PATH_MODEL + "Sword/Enemy/Sword.mv1");

	/* アニメーション */
	_SetResource(LOAD_TYPE::ANIMATION, SRC::ANIMATION_DAMAGE, PATH_ANIMATION + "Damage.mv1");
	_SetResource(LOAD_TYPE::ANIMATION, SRC::ANIMATION_DEFEAT, PATH_ANIMATION + "Defeat.mv1");
	_SetResource(LOAD_TYPE::ANIMATION, SRC::ANIMATION_DRAWING_SWORD, PATH_ANIMATION + "DrawingSword.mv1");
	_SetResource(LOAD_TYPE::ANIMATION, SRC::ANIMATION_PLAYER_IDLE, PATH_ANIMATION + "Idle.mv1");
	_SetResource(LOAD_TYPE::ANIMATION, SRC::ANIMATION_PLAYER_WALK, PATH_ANIMATION + "Walk.mv1");
	_SetResource(LOAD_TYPE::ANIMATION, SRC::ANIMATION_PLAYER_RUN, PATH_ANIMATION + "Run.mv1");
	_SetResource(LOAD_TYPE::ANIMATION, SRC::ANIMATION_PLAYER_DODGE, PATH_ANIMATION + "Rolling.mv1");
	_SetResource(LOAD_TYPE::ANIMATION, SRC::ANIMATION_PLAYER_DEFENSE, PATH_ANIMATION + "DefenseHold.mv1");
	//_SetResource(LOAD_TYPE::ANIMATION, SRC::ANIMATION_PLAYER_DEFENSE, PATH_ANIMATION + "DefenseActive.mv1");
	_SetResource(LOAD_TYPE::ANIMATION, SRC::ANIMATION_PLAYER_DEFEAT, PATH_ANIMATION + "Defeat.mv1");
	_SetResource(LOAD_TYPE::ANIMATION, SRC::ANIMATION_PLAYER_SLASH_LIGHT, PATH_ANIMATION + "SlashLight.mv1");
	_SetResource(LOAD_TYPE::ANIMATION, SRC::ANIMATION_PLAYER_SLASH_HEAVY, PATH_ANIMATION + "SlashHeavy.mv1");
	_SetResource(LOAD_TYPE::ANIMATION, SRC::ANIMATION_BOSS_STAND_UP, PATH_ANIMATION + "StandUp.mv1");

	/* BGM */
	_SetResource(LOAD_TYPE::SOUND, SRC::BGM_TITLE, PATH_BGM + "TitleBGM.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::BGM_GAME, PATH_BGM + "GameBGM.mp3");
	//_SetResource(LOAD_TYPE::SOUND, SRC::BGM_RESULT, PATH_BGM + "Result.mp3");
	//_SetResource(LOAD_TYPE::SOUND, SRC::BGM_LOBBY, PATH_BGM + "BGM_LOBBY.mp3");

	/* 効果音 */
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_SELECT, PATH_SE + "Select.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_PARRY, PATH_SE + "Parry.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_SLASH_LIGHT, PATH_SE + "SlashLight.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_UI_CANCEL, PATH_SE + "Cancel.mp3");

	/* シェーダ */
	_SetResource(LOAD_TYPE::PIXEL_SHADER, SRC::PS_NORMAL_MAP, PATH_SHADER + "NormalMap.cso");
	_SetResource(LOAD_TYPE::PIXEL_SHADER, SRC::PS_RAINY, PATH_SHADER + "Rainy.cso");
	_SetResource(LOAD_TYPE::PIXEL_SHADER, SRC::PS_TEX_SCALE, PATH_SHADER + "TexScalePS.cso");
	_SetResource(LOAD_TYPE::VERTEX_SHADER, SRC::VS_TEX_SCALE, PATH_SHADER + "TexScaleVS.cso");

}
void ResourceManager::InitializeTitleScene(void)
{

}
void ResourceManager::InitializeGameScene(void)
{

}

void ResourceManager::_SetResource(Resource::LOAD_TYPE _loadType, SRC src, std::string path
								   , int allNum, int numX, int numY)
{
	if (allNum == -1)
	{
		// その他読み込み
		resourcesMap_.emplace(src, Resource(_loadType, path));
	}
	else
	{
		// 複数画像読み込み
		resourcesMap_.emplace(src,
			Resource(_loadType, path, allNum, numX, numY));
	}
	
}


void ResourceManager::Release(void)
{
	/* メモリ解放処理 */

	if (!resourcesMap_.empty())
	{
		// リソースリストをクリア(空の時は行わない)
		resourcesMap_.clear();
	}
	if (!loadedMap_.empty())
	{
		for (auto& [src, resource] : loadedMap_)
		{
			// 読み込み済みリソース解放
			resource->Release();
			delete resource;
		}

		// 読み込み済みリソースリストをクリア
		loadedMap_.clear();
	}
}
void ResourceManager::DestroyInstance(void)
{
	/*　インスタンス削除処理　*/
	instance_->Release();
	delete instance_;
}


Resource ResourceManager::Load(SRC src)
{
	/* 読み込み処理 */
	Resource* res = _Load(src);

	if (res == nullptr) return Resource();

	return *res;
}
const int ResourceManager::LoadHandleId(SRC src)
{
	// リソースの
	return Load(src).LoadHandleId();
}
void ResourceManager::LoadHandleIds(SRC src, int* target)
{
	// 複数画像ではない場合、処理終了
	Resource::LOAD_TYPE loadType = resourcesMap_[src].GetLoadType();
	if (loadType != Resource::LOAD_TYPE::IMAGES) { return; }

	// 複数画像の対象にコピー
	Load(src).CopyHandle(target);

#ifdef _DEBUG
	if (*target == -1)
	{
		OutputDebugString("\n複数画像が読み込まれませんでした。画像数/画像１枚のサイズ/画像パス名を確認してください。\n");
	}
#endif
}

std::string ResourceManager::GetHandlePath(SRC src)
{
	return Load(src).GetHandlePath();
}

Resource* ResourceManager::_Load(SRC src)
{
	// 読み込み済みリストを検索
	const auto& loaded = loadedMap_.find(src);

	//読み込み済みリストに対象がある時、要素を返す
	if (loaded != loadedMap_.end()) return loaded->second;


	// リソースリスト内を検索
	const auto& resource = resourcesMap_.find(src);

	// リソースリストに登録されてない時、NULLを返す
	if (resource == resourcesMap_.end()) return nullptr;


	// リソースリスト登録済み時、読み込み処理
	resource->second.Load();

	// 念のためにコピーコンストラクタ
	Resource* ret = new Resource(resource->second);

	// 読み込み済みリストに格納
	loadedMap_.emplace(src, ret);

	return ret;
}


const int ResourceManager::LoadHandleIdsOnce(SRC src, int _imageNum)
{
	return Load(src).GetHandleImagesId(_imageNum);
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	/* 3Dモデル重複利用時の読み込み */

	// 読み込み処理
	Resource* resource = _Load(src);

	// 読み込み失敗
	if (resource == nullptr)
	{
		return -1;
	}

	// 重複するモデルのハンドルを取得
	int id = MV1DuplicateModel(resource->LoadHandleId());

	// 重複モデルリストにハンドル追加
	resource->SetDuplicateModelId(id);

	return id;
}