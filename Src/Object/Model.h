#pragma once
#include "ActorBase.h"

class ModelMaterial;
class ModelRenderer;

class Model : public ActorBase
{
public:

	enum class NAME
	{
		SHIELD,
		WATER,
		GATE,
		GATE_MIST,
	};

	// コンストラクタ
	Model(const Model::NAME& name, const Transform& transform);

	// デストラクタ
	~Model(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

private:

	float taim_;

	int text;

	// モデル
	Model::NAME name_;

	// 頂点シェーダー
	std::unique_ptr<ModelMaterial> modelMaterial_;
	std::unique_ptr<ModelRenderer> modelRenderer_;

};

