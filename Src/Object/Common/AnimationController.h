#pragma once
#include <unordered_map>
#include <string>
#include "../../Utility/UtilityMath.h"

class AnimationController
{
public:

	enum class ANIMATION_TYPE
	{
		NONE,
		INTERNAL, // 内部アニメーション
		EXTERNAL, // 外部アニメーション
	};

	// アニメーションデータ
	struct Animation
	{
		ANIMATION_TYPE type = ANIMATION_TYPE::NONE;
		int modelId	        = -1;   // アニメーションモデル
		int attachNo        = -1;
		int animationIndex	= 0;    // モデル内アニメーション番号
		float speed		    = 0.0f; // 再生速度
		float totalTime     = 0.0f; // 最大再生時間
		float step          = 0.0f;	// 現在再生時間

		bool isLoadPath = false; // パスで読み込んでいるか否か
		bool isInPlace  = false; // アニメーションの位置を固定するか否か

		// 固定するアニメーションローカル位置
		VECTOR inPlaceLocalPos = UtilityMath::VECTOR_ZERO;
		VECTOR inPlaceLocalPosEnd = UtilityMath::VECTOR_ZERO;
	};

	static constexpr float ANIM_SPEED_DEFAULT = 30.0f;
	static constexpr float BLEND_TIME_DEFAULT = 0.175f;


	/// @brief コンストラクタ
	/// @param modelId アニメーション対象
	AnimationController(int modelId);

	/// @brief デストラクタ
	~AnimationController(void);


	/// @brief 同じモデル内のアニメーションを準備
	/// @param type アニメーション種類
	/// @param speed アニメーション速度 
	void AddInternal(int type, float speed = ANIM_SPEED_DEFAULT);

	/// @brief 同じモデル内のアニメーションを準備し、再生座標固定
	/// @param type アニメーション種類
	/// @param speed アニメーション速度 
	void AddInternal(int type, const VECTOR& localPos, float speed = ANIM_SPEED_DEFAULT);


	/// @brief 別の読み込み済みアニメーションモデルから準備
	/// @param type アニメーション種類
	/// @param handle アニメーションのハンドル
	/// @param speed アニメーション速度 
	void AddExternal(int type, int handle, float speed = ANIM_SPEED_DEFAULT);

	/// @brief 別の読み込み済みアニメーションモデルから準備し、再生座標固定
	/// @param type アニメーション種類
	/// @param speed アニメーション速度 
	/// @param handle アニメーションのハンドル
	/// @param placeLocalPos 固定するアニメーションローカル位置
	void AddExternal(int type, int handle
					, const VECTOR& localPos
					, float speed = ANIM_SPEED_DEFAULT);

	/// @brief 別の読み込み済みアニメーションモデルから準備し、再生座標固定
	/// @param type アニメーション種類
	/// @param speed アニメーション速度 
	/// @param handle アニメーションのハンドル
	/// @param placeLocalPos 固定するアニメーションローカル位置
	void AddExternal(int type, int handle
					, const VECTOR& localPos
					, const VECTOR& localPosEnd, float speed = ANIM_SPEED_DEFAULT);


	/// @brief アニメーション再生
	/// @param type アニメーションの種類
	/// @param isLoop ループするか否か @hint default = true
	/// @param playSpeed 再生速度 @hint default = initSpeed
	/// @param blendTime アニメーション遷移時間
	void Play(int type, bool isLoop = true, float playSpeed = -1.0f, float blendTime = BLEND_TIME_DEFAULT);

	/// @brief 更新処理
	void Update(void);

	/// @brief デバッグ描画処理
	void DrawDebug(void);

	/// @brief メモリ解放処理
	void Release(void);


	/// @brief アニメーションが終了しているか否か
	bool IsEnd(void) const;

	/// @brief 一定の位置に到達したかの判定
	/// @param pointStart 判定開始位置の割合(0.0f～1.0f)
	/// @param pointEnd 判定終了位置の割合(0.0f～1.0f)
	bool IsEndPoint(float pointStart, float pointEnd = 1.0f);

	/// @brief 再生中のアニメーションの再生割合を取得
	/// @return 割合(0.0～1.0)
	float GetPlayPointRate(void);

	/// @brief 再生中のアニメーション番号取得
	int GetPlayType(void)const { return playType_; };


	/// @brief アニメーション停止処理
	/// @param stopTime 停止時間
	void Stop(float stopTime);

	/// @brief 再生位置変更処理
	/// @param step 再生する位置
	void SetAnimationStep(float step = 0.0f);

	/// @brief 停止しているか否か
	bool IsStop(void)const { return isStop_; };

	/// @brief 再生位置変更
	/// @param rate 再生位置の割合(0.0f～1.0f)
	void SetAnimationStepRate(float rate);

	void SetModelId(int modelId);

	/// @brief 再生中のアニメーションの現在時間を取得
	float GetPlayTime(void);

	/// @brief 再生中のアニメーションの総再生時間を取得
	float GetPlayTimeTotal(void);


private:

	// アニメーションするモデルのハンドルID
	int modelId_ = -1;

	// 種類別のアニメーションデータ
	std::unordered_map<int, Animation> animations_;

	// 再生中のアニメーション状態
	int playType_ = -1;

	// 前回のアニメーション状態
	int prePlayType_ = -1;

	// ブレンド時間
	float blendTime_ = 0.0f;

	float playSpeed_ = 0.0f;

	// ブレンドのカウンタタイマー
	float curBlendTime_ = 0.0f;

	// ループするか否かの判定
	bool isLoop_ = false;

	// アニメーションを停止するか否か
	bool isStop_ = false;

	// 停止時間
	float timeStop_ = 0.0f;
	
	float term = 0.0f;
	
	// ブレンドアニメーションの前アニメーションのローカル位置
	VECTOR preAnimationLocalPos_;
	

	/// @brief 他アニメーションとのブレンドの影響を受けない単体の素のルート位置を取得
	/// @param target 位置を取得したいアニメーション
	/// @param other ブレンド対象の相方アニメーション(一時的にブレンド率0%にする)
	VECTOR GetRawAnimationRootPos(Animation& target, Animation& other);

	/// @brief 固定位置アニメーションの、現在の再生進行度に応じた目標位置を取得
	/// @param animation 対象の固定位置のアニメーション
	/// @return 開始位置から線形補間した位置終了した位置
	VECTOR GetInPlaceProgressPos(const Animation& animation) const;
	

	/// @brief アニメーション追加処理
	/// @param type アニメーションの種類
	/// @param animationIndex 格納するアニメーションリスト
	void Add(int type, Animation& animationIndex);

	/// @brief アニメーションが格納されているか判定
	/// @param type アニメーションの種類
	bool IsFindAnimation(int type);

	/// @brief 固定アニメーション処理
	/// @param prePlayAnimation 再生中のアニメーション
	/// @param curPlayAnimation 再生中のアニメーション
	void AnimationInPlace(Animation& prePlayAnimation, Animation& curPlayAnimation, float blendTime);
};