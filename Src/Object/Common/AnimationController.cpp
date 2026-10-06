#include "AnimationController.h"
#include <DxLib.h>
#include <string>
#include <cassert>
#include <algorithm>
#include "../../Manager/SceneManager.h"

namespace
{
	// ROOTフレーム番号
	constexpr int FRAME_ROOT_NUM = 1;
};

AnimationController::AnimationController(int modelId)
	: animations_{}
	, modelId_(modelId)
	, preAnimationLocalPos_(UtilityMath::VECTOR_ZERO)
{
}

AnimationController::~AnimationController(void)
{
	Release();
}

void AnimationController::AddInternal(int type, float _speed)
{
	/* 内部のアニメーションの追加 */
	Animation animation = Animation();

	animation.animationIndex = type;

	// アニメーション速度割り当て
	animation.speed = _speed;

	// アニメーション状態割り当て
	animation.type = ANIMATION_TYPE::INTERNAL;

	animation.step = 0.0f;

	animation.isInPlace = false;

	// アニメーション追加処理
	Add(type, animation);
}
void AnimationController::AddInternal(int type, const VECTOR& _localPos, float _speed)
{
	/* 内部のアニメーションの追加 */
	Animation animation = Animation();

	animation.animationIndex = type;

	// アニメーション速度割り当て
	animation.speed = _speed;

	// アニメーション状態割り当て
	animation.type = ANIMATION_TYPE::INTERNAL;

	animation.step = 0.0f;

	animation.isInPlace = true;

	if (!UtilityMath::EqualsVZero(_localPos))
	{
		animation.inPlaceLocalPos = _localPos;
		
		animation.inPlaceLocalPosEnd = _localPos;
	}

	// アニメーション追加処理
	Add(type, animation);
}

void AnimationController::AddExternal(int type, int _handle, float _speed)
{
	/* 外部のアニメーションの追加 */
	Animation animation = Animation();

	animation.modelId = _handle;

	// アニメーション速度割り当て
	animation.speed = _speed;

	// アニメーション状態割り当て
	animation.type = ANIMATION_TYPE::EXTERNAL;

	animation.step = 0.0f;

	animation.isInPlace = false;
	
	// アニメーション追加処理
	Add(type, animation);
}
void AnimationController::AddExternal(int type, int _handle
									  , const VECTOR& _localPos, float _speed)
{
	/* 外部のアニメーションの追加 */
	Animation animation = Animation();

	animation.modelId = _handle;

	// アニメーション速度割り当て
	animation.speed = _speed;

	// アニメーション状態割り当て
	animation.type = ANIMATION_TYPE::EXTERNAL;

	animation.step = 0.0f;

	animation.isInPlace = true;

	if (!UtilityMath::EqualsVZero(_localPos))
	{
		animation.inPlaceLocalPos = _localPos;
		animation.inPlaceLocalPosEnd = _localPos;
	}
	
	// アニメーション追加処理
	Add(type, animation);
}
void AnimationController::AddExternal(int type, int _handle
									  , const VECTOR& _localPos, const VECTOR& _localPosEnd, float _speed)
{
	/* 外部のアニメーションの追加 */
	Animation animation = Animation();

	animation.modelId = _handle;

	// アニメーション速度割り当て
	animation.speed = _speed;

	// アニメーション状態割り当て
	animation.type = ANIMATION_TYPE::EXTERNAL;

	animation.step = 0.0f;

	animation.isInPlace = true;

	if (!UtilityMath::EqualsVZero(_localPos))
	{
		animation.inPlaceLocalPos = _localPos;
	}
	if (!UtilityMath::Equals(_localPos, _localPosEnd))
	{
		animation.inPlaceLocalPosEnd = _localPosEnd;
	}
	
	// アニメーション追加処理
	Add(type, animation);
}


void AnimationController::Play(int type, bool _isLoop, float playSpeed, float _blendTime)
{
	// 同じアニメーション時、処理を終了
	if (playType_ == type || type == -1) { return; }

	if (prePlayType_ != -1)
	{
		// モデルからアニメーションを外す
		auto& preAnim = animations_[prePlayType_];
		MV1DetachAnim(modelId_, preAnim.attachNo);
		prePlayType_ = -1;
	}

	// 現在のアニメを前回に割り当て
	if (playType_ != -1)
	{
		prePlayType_ = playType_;
		
		auto& stateFromAnimation = animations_[prePlayType_];
		if (stateFromAnimation.isInPlace)
		{
			// 遷移元が固定位置アニメの場合、固定値をそのまま使用
			preAnimationLocalPos_ = stateFromAnimation.inPlaceLocalPos;
		}
		else
		{
			// 遷移元が非固定の場合、現在のルートフレームの実際のローカル位置を取得
			MATRIX curFrameMat = MV1GetFrameLocalMatrix(modelId_, FRAME_ROOT_NUM);
			preAnimationLocalPos_ = MGetTranslateElem(curFrameMat);
		}
	}

	// アニメーションループ
	isLoop_ = _isLoop;

	// 停止を解除
	isStop_ = false;

	// アニメーション種別を変更
	playType_ = type;

	// ブレンド時間初期化
	curBlendTime_ = 0.0f;

	blendTime_ = _blendTime;


	auto& playAnimation = animations_[type];

	// 初期化
	playAnimation.step = 0.0f;

	// 再生速度割り当て
	playSpeed_ = ((playSpeed >= 0.0f) ? playSpeed : playAnimation.speed);
	

	// モデルにアニメーションを付ける
	if (playAnimation.type == ANIMATION_TYPE::INTERNAL)
	{
		// モデルと同じファイルからアニメーションをアタッチする
		playAnimation.attachNo = MV1AttachAnim(modelId_, playAnimation.animationIndex);
	}
	else
	{
		// 別のモデルファイルからアニメーションをアタッチする
		// DxModelViewerを確認すること(大体0か1)
		int animIdx = 0;
		playAnimation.attachNo = MV1AttachAnim(modelId_, animIdx, playAnimation.modelId);
	}

	// アニメーション総時間の取得
	playAnimation.totalTime = MV1GetAttachAnimTotalTime(modelId_, playAnimation.attachNo);


	// 前回のアニメーションがある時、ブレンド率を1.0f(100%)にする
	float blendRate = ((prePlayType_ == -1) ? 1.0f : 0.0f);

	// ブレンドアニメーションの割合を割り当て
	MV1SetAttachAnimBlendRate(modelId_, playAnimation.attachNo, blendRate);
}

void AnimationController::Update(void)
{
	// 経過時間の取得
	float deltaTime = SceneManager::GetInstance().GetDeltaTime();
	auto& curAnimation = animations_[playType_];
	auto& preAnimation = animations_[prePlayType_];


	// 停止時に処理終了
	if (isStop_ && timeStop_ > 0.0f)
	{
		timeStop_ -= deltaTime;
		if (timeStop_ <= 0.0f) { isStop_ = false; }

		return;
	}

	// 再生中のアニメーション
	if (playType_ != -1)
	{
		// アニメーション進行処理
		curAnimation.step += (deltaTime * playSpeed_);

		if (curAnimation.step >= curAnimation.totalTime && isLoop_)
		{
			// 再生がループ状態で終了時、初期位置に戻す
			curAnimation.step = 0.0f;
		}

		// アニメーション更新
		MV1SetAttachAnimTime(modelId_, curAnimation.attachNo, curAnimation.step);
	}


	// ブレンド時間が割り当てているときは、現在時間と最大時間の割合を、それ以外はタイマーを終了
	term = ((blendTime_ > 0.0f) ? (curBlendTime_ / blendTime_) : 1.0f);

	if (prePlayType_ != -1)
	{
		// ブレンドタイマー増加
		curBlendTime_ += deltaTime;



		// 旧・新規アニメーションのブレンド率を割り当て
		MV1SetAttachAnimBlendRate(modelId_, preAnimation.attachNo, (1.0f - term));
		MV1SetAttachAnimBlendRate(modelId_, curAnimation.attachNo, term);

		// ブレンドアニメーション終了時
		if (term >= 1.0f)
		{
			// 前アニメーションをデタッチ
			MV1DetachAnim(modelId_, preAnimation.attachNo);
			prePlayType_ = -1;

			// 新規アニメーションのブレンド率を100%にする
			MV1SetAttachAnimBlendRate(modelId_, curAnimation.attachNo, 1.0f);
		}
	}

	// 対象フレームのローカル行列を初期値にリセットする
	MV1ResetFrameUserLocalMatrix(modelId_, FRAME_ROOT_NUM);

	
	if (preAnimation.isInPlace || curAnimation.isInPlace)
	{
		// 遷移前/遷移後アニメーションが固定の場合、アニメーション位置固定処理
		AnimationInPlace(preAnimation, curAnimation, term);
	}
}
void AnimationController::AnimationInPlace(Animation& prePlayAnimation, Animation& curPlayAnimation, float _rate)
{
	// 対象フレームのローカル行列を初期値にリセットす
	MV1ResetFrameUserLocalMatrix(modelId_, FRAME_ROOT_NUM);
	
	// アニメーションブレンド率
	float rate = std::clamp(_rate, 0.0f, 1.0f);

	// 遷移の位置
	VECTOR preBase = preAnimationLocalPos_;
	VECTOR curBase = UtilityMath::VECTOR_ZERO;
	
	if (curPlayAnimation.isInPlace)
	{
		// 固定位置アニメーションの場合、自身の再生進行度(step/totalTime)に応じて
		// inPlaceLocalPos → inPlaceLocalPosEnd へ徐々に移動した目標位置を使用
		curBase = GetInPlaceProgressPos(curPlayAnimation);
	}
	else
	{
		curBase = GetRawAnimationRootPos(curPlayAnimation, prePlayAnimation);
		
		// ブレンド率に戻す
		if (curPlayAnimation.attachNo != -1)
			{ MV1SetAttachAnimBlendRate(modelId_, curPlayAnimation.attachNo, rate); }
			
		if (prePlayAnimation.attachNo != -1)
			{ MV1SetAttachAnimBlendRate(modelId_, prePlayAnimation.attachNo, (1.0f - rate)); }
	}
		
	// アニメーション位置変更の線形補間用座標
	VECTOR localPos = UtilityMath::VECTOR_ZERO;
	localPos.x = (preBase.x + (curBase.x - preBase.x) * rate);
	localPos.y = (preBase.y + (curBase.y - preBase.y) * rate);
	localPos.z = (preBase.z + (curBase.z - preBase.z) * rate);

	// 対象フレームのローカル行列(大きさ、回転)はモデル本来の値を使用
	MATRIX mat = MV1GetFrameLocalMatrix(modelId_, FRAME_ROOT_NUM);
	VECTOR scl = MGetSize(mat);
	MATRIX rot = MGetRotElem(mat);

	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale(scl));
	mix = MMult(mix, rot);
	mix = MMult(mix, MGetTranslate(localPos));

	// 対象フレームにセットし直し、アニメーションの移動値を無効化
	MV1SetFrameUserLocalMatrix(modelId_, FRAME_ROOT_NUM, mix);
}
VECTOR AnimationController::GetRawAnimationRootPos(Animation& target, Animation& other)
{
	if (target.attachNo != -1)
	{
		MV1SetAttachAnimBlendRate(modelId_, target.attachNo, 1.0f);
	}
	if (other.attachNo != -1)
	{
		MV1SetAttachAnimBlendRate(modelId_, other.attachNo, 0.0f);
	}

	MATRIX mat = MV1GetFrameLocalMatrix(modelId_, FRAME_ROOT_NUM);
	return MGetTranslateElem(mat);
}

VECTOR AnimationController::GetInPlaceProgressPos(const Animation& animation) const
{
	// 総再生時間が未取得(0以下)の間は開始位置をそのまま返す
	if (animation.totalTime <= 0.0f) { return animation.inPlaceLocalPos; }

	// アニメーション自身の再生進行度(0.0～1.0)
	float animRate = std::clamp((animation.step / animation.totalTime), 0.0f, 1.0f);

	// 開始位置(inPlaceLocalPos) → 終了位置(inPlaceLocalPosEnd) へ進行度で線形補間
	VECTOR pos = UtilityMath::VECTOR_ZERO;
	pos.x = animation.inPlaceLocalPos.x + (animation.inPlaceLocalPosEnd.x - animation.inPlaceLocalPos.x) * animRate;
	pos.y = animation.inPlaceLocalPos.y + (animation.inPlaceLocalPosEnd.y - animation.inPlaceLocalPos.y) * animRate;
	pos.z = animation.inPlaceLocalPos.z + (animation.inPlaceLocalPosEnd.z - animation.inPlaceLocalPos.z) * animRate;

	return pos;
}


void AnimationController::DrawDebug(void)
{
#ifdef _DEBUG
	if (playType_ == -1) { return; }
	auto& animation = animations_.at(playType_);

	// アニメーションの描画
	DrawFormatString(0,64,0xFF0000,"animationTime:%.2f, term : %.2f",animation.step, term);
#endif // _DEBUG
}

void AnimationController::Release(void)
{
	if (animations_.empty()) { return; }

	// ロードしたアニメーションを解放
	for (auto& [type, animation] : animations_)
	{
		// アニメーションをリセット
		MV1DetachAnim(modelId_, animation.attachNo);

		// パス読み込みでの外部アニメーション時
		if (animation.type == ANIMATION_TYPE::EXTERNAL &&
			animation.isLoadPath)
		{
			// アニメーション解放
			MV1DeleteModel(animation.modelId);
		}
	}

	// リスト解放
	animations_.clear();
}

bool AnimationController::IsEnd(void) const
{
	// アニメーションが再生されていない・ループアニメーション時、false
	if (playType_ == -1 || isLoop_) { return false; }

	auto& animation = animations_.at(playType_);

	// 再生時間が最大再生時間を超えたら、true
	return (animation.step >= animation.totalTime);
}

bool AnimationController::IsEndPoint(float _pointStart, float _pointEnd)
{
	if (playType_ == -1) { return false; }

	Animation& animation = animations_.at(playType_);

	// 再生位置
	float start = std::clamp(_pointStart, 0.0f, 1.0f);

	// 終了位置
	float end = std::clamp(_pointEnd, 0.0f, 1.0f);

	// 再生位置の割合
	float curRate = (animation.step / animation.totalTime);

	// 再生位置が指定の割合になったときtrue
	return (curRate >= start && curRate < end);
}
float AnimationController::GetPlayPointRate(void)
{
	/* アニメーション再生割合を取得 */

	if (playType_ == -1) { return 0.0f; }

	Animation& animation = animations_.at(playType_);

	return (animation.step / animation.totalTime);
}

void AnimationController::Stop(float _stopTime)
{
	isStop_ = true;
	timeStop_ = _stopTime;
}

void AnimationController::SetAnimationStep(float _step)
{
	if (playType_ == -1) { return; }

	auto& animation = animations_.at(playType_);

	// 再生位置の制限
	float step = std::clamp(_step, 0.0f, animation.totalTime);

	// 再生位置割り当て
	animation.step = _step;
}

void AnimationController::SetAnimationStepRate(float _rate)
{
	if (playType_ == -1) { return; }

	auto& animation = animations_.at(playType_);

	float step = std::clamp(_rate, 0.0f, 1.0f);

	// 再生位置割り当て
	float rate = (1.0f / animation.totalTime);
	animation.step = (rate * step);
}

void AnimationController::SetModelId(int modelId)
{
	modelId_ = modelId;
	for (auto& animation : animations_)
	{
		animation.second.modelId = modelId;
	}
}

float AnimationController::GetPlayTime(void)
{
	auto& animation = animations_.at(playType_);
	float time = -1;
	if (playType_ != -1)
	{
		time = animation.step;
	}

#ifdef _DEBUG
	else
	{
		OutputDebugString("\nアニメーションが割り当てられていないため、再生時間が取得出来ませんでした；；\n");
		assert(false); // 例外スロー
	}
#endif

	return time;
}

float AnimationController::GetPlayTimeTotal(void)
{
	auto& animation = animations_.at(playType_);
	float time = -1;
	if (playType_ != -1)
	{
		time = animation.totalTime;
	}

#ifdef _DEBUG
	else
	{
		OutputDebugString("\nアニメーションが割り当てられていないため、総再生時間が取得出来ませんでした；；\n");
		assert(false); // 例外スロー
	}
#endif

	return time;
}


void AnimationController::Add(int type, Animation& animation)
{
	if (animations_.count(type) == 0)
	{
		// 動的配列に追加
		animations_.emplace(type, animation);
	}
}

bool AnimationController::IsFindAnimation(int type)
{
	auto it = animations_.find(type);
	if (it != animations_.end())
	{
		// 発見
		return true;
	}
#ifdef _DEBUG
	else
	{
		OutputDebugString("\nアニメーションの情報がありません。\n");
	}
#endif

	return false;
}