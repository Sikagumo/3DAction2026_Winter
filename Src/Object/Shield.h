#pragma once
#include <memory>
#include "Common/Transform.h"
#include "ActorBase.h"
#include "../Renderer/Vertex/ModelRenderer.h"
#include "../Renderer/Vertex/ModelMaterial.h"
class Player;
class ModelMaterial;
class ModelRenderer;

class Shield : public ActorBase
{

public:

	// コンストラクタ
	Shield(void);

	// デストラクタ
	~Shield(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;


private:

	std::unique_ptr<ModelRenderer> renderer_;
	std::unique_ptr<ModelMaterial> material_;

	void AddShaderParamLight(void)override;
};
