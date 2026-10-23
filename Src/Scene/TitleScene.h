#pragma once
#include "./SceneBase.h"

class TitleScene : public SceneBase
{

public:

	TitleScene(void);

	~TitleScene(void)override;

	void Initialize(void) override;
	void Update(void) override;
	void Draw(void) override;


private:

	// ‰æ‘œ
	int imageTitle_ = -1;
	int imagePush_ = -1;
};
