#pragma once

class SceneBase
{
public:

	SceneBase(void);
	virtual ~SceneBase(void) = 0;

	virtual void Initialize(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void)   = 0;
};
