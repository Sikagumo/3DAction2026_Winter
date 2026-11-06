#include "Resource.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>

Resource::Resource(void)
	: resType_(LOAD_TYPE::NONE)
	, path_("")
	, allNum_(0), numX_(0), numY_(0)
	, handleId_(-1), handleIds_{}
{
}

Resource::Resource(LOAD_TYPE type, const std::string& path)
	: resType_(type)
	, path_(path)
	, allNum_(0), numX_(0), numY_(0)
	, handleId_(-1), handleIds_{}
{
}

Resource::Resource(LOAD_TYPE type, const std::string& path
				   , int allNum, int numX, int numY)
	: resType_(type)
	, path_(path)
	, allNum_(allNum), numX_(numX), numY_(numY)
	, handleId_(-1), handleIds_(allNum, -1)
{
	
}



void Resource::Load(void)
{
	/* リソース読み込み処理 */

	switch (resType_)
	{
		case LOAD_TYPE::IMAGE: case LOAD_TYPE::MOVIE:
			LoadImageAndMovie();
		break;

		case LOAD_TYPE::IMAGES:
			LoadImages();
		break;

		case LOAD_TYPE::MODEL: case LOAD_TYPE::ANIMATION:
			LoadModelAndAnimation();
		break;
		
		case LOAD_TYPE::EFFECT:
			LoadEffect();
		break;

		case LOAD_TYPE::SOUND:
			LoadSound();
		break;

		case LOAD_TYPE::VERTEX_SHADER:
			LoadShaderVertex();
		break;

		case LOAD_TYPE::PIXEL_SHADER:
			LoadShaderPixel();
		break;
	}

}

void Resource::LoadImageAndMovie(void)
{
	/* 単一画像・映像ファイルの読み込み */
	handleId_ = LoadGraph(path_.c_str());
}
void Resource::LoadImages(void)
{
	/* 複数画像の読み込み */
	int handle = LoadGraph(path_.c_str());
	int sizeX, sizeY = 0;

	// 画像単体のサイズを取得
	GetGraphSize(handle, &sizeX, &sizeY);
	sizeX /= numX_;
	sizeY /= numY_;

	LoadDivGraph(path_.c_str(),
		allNum_, numX_, numY_,
		sizeX, sizeY,
		(&handleIds_[0]));
}
void Resource::LoadModelAndAnimation(void)
{
	/* 3Dモデル・アニメーションファイルの読み込み */
	handleId_ = MV1LoadModel(path_.c_str());
}
void Resource::LoadEffect(void)
{
	/* エフェクトの読み込み */
	// 1. 現在の非同期フラグの状態を一時的に保存しつつ、一時的に「同期読み込み(false)」にする
	int prevAsyncFlag = GetUseASyncLoadFlag();
	SetUseASyncLoadFlag(FALSE);

	// 2. 安全にその場でエフェクトを読み込む
	handleId_ = LoadEffekseerEffect(path_.c_str());

	// 3. 読み込みが終わったら、元の非同期フラグの状態に戻す
	SetUseASyncLoadFlag(prevAsyncFlag);
}
void Resource::LoadSound(void)
{
	/* 音声ファイルの読み込み */
	SetCreate3DSoundFlag(resType_ == LOAD_TYPE::SOUND_3D);
	handleId_ = LoadSoundMem(path_.c_str());
	SetCreate3DSoundFlag(FALSE);
}

void Resource::LoadShaderVertex(void)
{
	/* 頂点シェーダファイルの読み込み */
	handleId_ = LoadVertexShader(path_.c_str());
}

void Resource::LoadShaderPixel(void)
{
	/* ピクセルシェーダファイルの読み込み */
	handleId_ = LoadPixelShader(path_.c_str());
}


void Resource::Release(void)
{
	/* リソース解放処理 */

	switch (resType_)
	{
		case LOAD_TYPE::IMAGE: case LOAD_TYPE::MOVIE:
			ReleaseImageAndMovie();
		break;

		case LOAD_TYPE::IMAGES:
			ReleaseImages();
		break;

		case LOAD_TYPE::MODEL: case LOAD_TYPE::ANIMATION:
			ReleaseModelAndAnimation();
		break;

		case LOAD_TYPE::EFFECT:
			ReleaseEffect();
		break;

		case LOAD_TYPE::SOUND:
			ReleaseSound();
		break;

		case LOAD_TYPE::VERTEX_SHADER:
			ReleaseShaderVertex();
			break;

		case LOAD_TYPE::PIXEL_SHADER:
			ReleaseShaderPixel();
			break;
	}
}
void Resource::ReleaseImageAndMovie(void)const
{
	/* 単一画像・映像ファイルの解放 */
	DeleteGraph(handleId_);
}
void Resource::ReleaseImages(void)
{
	// 複数画像の解放
	for (int& handle : handleIds_)
	{
		DeleteGraph(handle);
	}
}
void Resource::ReleaseModelAndAnimation(void)const
{
	// 3Dモデル・アニメーションファイルの解放
	MV1DeleteModel(handleId_);

	auto& ids = duplicationModelIds_;

	for (auto& id : ids)
	{
#if _DEBUG
		if (id == -1)
		{
			OutputDebugString("\nモデルが未割当です\n");
		}
#endif
		MV1DeleteModel(id);
	}
}
void Resource::ReleaseEffect(void)const
{
	// エフェクトの解放
	DeleteEffekseerEffect(handleId_);
}
void Resource::ReleaseSound(void)const
{
	// 音声ファイルの解放
	if (CheckSoundMem(handleId_) == 1)
	{
		// 再生時、再生中の音声を停止
		StopSoundMem(handleId_);
	}

	// 音声ファイルを解放
	DeleteSoundMem(handleId_);
}

void Resource::ReleaseShaderVertex(void) const
{
	// 頂点シェーダの解放
	DeleteShader(handleId_);
}

void Resource::ReleaseShaderPixel(void) const
{
	// ピクセルシェーダの解放
	DeleteShader(handleId_);
}


void Resource::CopyHandle(int* _images)
{
	/* ハンドルコピー処理 */
	for (int i = 0; i < allNum_; i++)
	{
		_images[i] = handleIds_[i];
	}
}

void Resource::SetDuplicateModelId(int _id)
{
	// IDをリストの末尾に追加
	duplicationModelIds_.push_back(_id);
}

int Resource::GetHandleImagesId(int _imageNum) const
{
	int handle = -1;
	if (_imageNum >= handleIds_.size() ||
		_imageNum < 0)
	{
		return handle;
	}

	handle = handleIds_[_imageNum];

	return handle;
}
