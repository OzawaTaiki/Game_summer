#include "GameScene.h"
#include "AxisIndicator.h"
#include "MyLib.h"
#include "PlayerBullet.h"
#include "TextureManager.h"
#include <cassert>
#include <fstream>
#include <string>

GameScene::GameScene() { scene_ = game; }

GameScene::~GameScene() {}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	if (scene_ == title) {
		isSceneChange_ = false;
		titleScene_ = std::make_unique<TitleScene>();
		titleScene_->Initilize();
	} else if (scene_ == game)
		InGameInit();
}

void GameScene::Update() {
	switch (scene_) {
	case title:
		isSceneChange_ = titleScene_->Update();
		break;
	case game: {
#ifdef _DEBUG
		if (input_->PushKey(DIK_LSHIFT) && input_->TriggerKey(DIK_0))
			debugCameraActive = debugCameraActive ? false : true;

#endif // _DEBUG

		railCamera->Update();

		skydome->Update();

		enemyManager_->Update();
		beartsCircleManager_->Update();

		// レティクルとビーツの判定,タイミング判定,ロックオン処理
		// ダメージ倍率出す
		player_->Update(viewProjection, beartsCircleManager_->GetBeatsList(), deltaTime_);
		float enemyDamege = 0.0f;
		XINPUT_STATE joyState;
		if (Input::GetInstance()->GetJoystickState(0, joyState) && (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
			enemyDamege = player_->ReleseLockOnTarget();
		}

		enemyManager_->Damage(enemyDamege);
		// 敵ダメージ，敵死す
		CheckCollsion();

		ImGui::Begin("debug");

		ImGui::Text("damege: %f", damageMagnification_);
		// ImGui::Text("enemy size:%d", enemies_.size());

		ImGui::End();

		catmulRomEdit->Update(viewProjection);

		if (debugCameraActive) {
			// if (!catmulRomEdit.get()->GetIsMove())
			debugCamera->Update();
			viewProjection.matProjection = debugCamera->GetViewProjection().matProjection;
			viewProjection.matView = debugCamera->GetViewProjection().matView;
			viewProjection.TransferMatrix();
		} else {
			viewProjection.matProjection = railCamera->GetViewProjectoin().matProjection;
			viewProjection.matView = railCamera->GetViewProjectoin().matView;
			viewProjection.TransferMatrix();
		}

		break;
	}
	default:
		break;
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	if (scene_ == title)
		titleScene_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	if (scene_ == game) {
		skydome->Draw(viewProjection);
		beartsCircleManager_->Draw(viewProjection);

		enemyManager_->Draw(viewProjection);
		catmulRomEdit->Draw(viewProjection);
		player_->Draw(viewProjection);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

	if (scene_ == game)
		catmulRomEdit->Drawpline();

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	if (scene_ == game) {
		player_->DrawReticle();
		enemyManager_->DrawMoveLanes();
	}

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
	if (isSceneChange_)
		SceneCheage();
}

void GameScene::SceneCheage() {

	isSceneChange_ = false;
	switch (scene_) {
	case title:
		InGameInit();
		scene_ = game;
		break;
	case game:

		break;
	default:
		break;
	}
}

void GameScene::CheckCollsion() {
	Sphere obj1;
	Sphere obj2;

	Vector2 rpos = player_->GetReticleScreenPosition();
	float rRadius = 32.0f;

	for (auto& enemy : enemyManager_->GetEnemyList()) {
		obj1.center = player_->GetPosition();
		obj1.radius = 1.0f;

		obj2.center = enemy->GetPosition();
		obj2.radius = 1.0f;
		// player  enemy
		if (IsCollision(obj1, obj2)) {
			player_->OnCollsion();
		}

		// player  e_bullet
		/*for (auto& eBullet:enemy->)
		    obj2.center =*/

		// enemy   p_bullet
		obj1.center = enemy->GetPosition();
		obj1.radius = 1.0f;

		for (auto& pBullet : player_->GetBullets()) {
			obj2.center = pBullet->GetWorldPositoin();
			obj2.radius = 1.0f;
			if (IsCollision(obj1, obj2)) {
				enemy->OnCollision(player_->GetBulletDamage_());
				pBullet->OnCollision();
			}
		}

		// TODO : 当たる，ｐｔｒ取得，この後の処理
		// そこに向かって弾を飛ばす clear
		// ビーツのロックオン仕様変更したいかも ボタン押してタイミング判定に変更
		Vector2 epos = enemy->GetScreenPos(viewProjection);
		float eRadius = 32.0f;
		float distance = Length(epos - rpos);
		if (distance <= eRadius + rRadius) {
			player_->ReticleCollision(enemy.get());
		}
	}
}

void GameScene::InGameInit() {
	viewProjection.Initialize();

	deltaTime_ = 1.0f / 60.0f;

	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection);

	railCamera = std::make_unique<RailCamera>();
	Vector3 railCameraPos = {0, 20.0f, -30.0f};
	Vector3 railCameraRota = {0.36f, 0, 0};
	railCamera->Initialize(railCameraPos, railCameraRota);

	skydome = std::make_unique<Skydome>();
	modelSkydome.reset(Model::CreateFromOBJ("skydome", true));
	skydome->Initialze(modelSkydome.get());

	debugCamera = std::make_unique<DebugCamera>(WinApp::kWindowWidth, WinApp::kWindowHeight);
	debugCamera->SetFarZ(2000.0f);
	AxisIndicator::GetInstance()->SetVisible(1);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection);

	playerModel_.reset(Model::Create());
	// playerTextureHandle_ = TextureManager::GetInstance()->Load("");
	reticleTextureHandle_ = TextureManager::GetInstance()->Load("2DReticle.png");

	player_ = std::make_unique<Player>();
	bulletModel_.reset(Model::CreateSphere());
	player_->Initialize(playerModel_.get(), bulletModel_.get(), 0, reticleTextureHandle_);

	// circleModel_.reset(Model::CreateFromOBJ("plane"));
	circleModel_.reset(Model::CreateSphere());
	targetModel_.reset(Model::CreateSphere());

	enemyModel_.reset(Model::Create());
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(enemyModel_.get(), 0);

	beartsCircleManager_ = std::make_unique<BeatsCircleManager>();
	beartsCircleManager_->Initialize(targetModel_.get(), circleModel_.get(), deltaTime_);
	damageMagnification_ = 0.0f;

	PrimitiveDrawer::GetInstance()->Initialize();
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection);
	catmulRomEdit = std::make_unique<CatmulRom>();
	catmulRomEdit->Initialize("data/pointData.json", railCamera.get());
}
