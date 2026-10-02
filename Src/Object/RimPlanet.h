#pragma once
#include <memory>
#include "ActorBase.h"
#include "../Renderer/Vertex/ModelRenderer.h"
#include "../Renderer/Vertex/ModelMaterial.h"
class ModelMaterial;
class ModelRenderer;

class RimPlanet : public ActorBase
{
public:

	RimPlanet(void);

	~RimPlanet(void)override;

	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;

private:

	struct Param
	{
		const COLOR_F diffuse;
		const COLOR_F ambient;
		const COLOR_F rimColor;
		VECTOR rimPow;
		VECTOR lightDir;
		VECTOR cameraPos;
	};
	std::unique_ptr<ModelRenderer> renderer_;
	std::unique_ptr<ModelMaterial> material_;

	void AddShaderParamLight(void)override;
};

