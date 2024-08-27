#pragma once

#include "Sprite.h"
#include <memory>

class TitleScene {
public:
	void Initilize();
	bool Update();
	void Draw();

private:
	std::unique_ptr<Sprite> sprite_;
	uint32_t textureHandle_;

	bool changeScene_;
};