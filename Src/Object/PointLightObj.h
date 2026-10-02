#pragma once
#include "ActorBase.h"
#include <DxLib.h>
#include "../Renderer/Vertex/ModelRenderer.h"
#include "../Renderer/Vertex/ModelMaterial.h"
#include "../Utility/AsoUtility.h"
#include <vector>
class ModelMaterial;
class ModelRenderer;

class PointLightObj : public ActorBase
{
public:

    PointLightObj(const VECTOR& _pos, float _scale);

    ~PointLightObj(void);
    void Init(void)override;
    void Update(void)override;
    void Draw(void)override;


private:

    float scale_;

    std::unique_ptr<ModelRenderer> renderer_;
    std::unique_ptr<ModelMaterial> material_;

    void AddShaderParamLight(void)override;
};



