#include "TitleScene.h"
#include "Input.h"
#include "TextureManager.h"

void TitleScene::Initilize() {
	textureHandle_ = TextureManager::GetInstance()->Load("title.png");

	sprite_ = std::make_unique<Sprite>();
	sprite_.reset(Sprite::Create(textureHandle_, {0, 0}));

	changeScene_ = false;
}

bool TitleScene::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		changeScene_ = true;
	}
	return changeScene_;
}

void TitleScene::Draw() { sprite_->Draw(); }
