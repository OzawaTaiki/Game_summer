#pragma once

#include "Audio.h"
#include "BeatsCircleManager.h"
#include "CatmulRom.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "EnemyManager.h"
#include "Input.h"
#include "Model.h"
#include "Player.h"
#include "RailCamera.h"
#include "Sprite.h"
#include "TitleScene.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "skydome.h"

enum Scene { title, game };

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void SceneCheage();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>

	Scene scene_;
	bool isSceneChange_;
	std::unique_ptr<TitleScene> titleScene_;

	// ビュープロジェクション
	ViewProjection viewProjection;

	std::unique_ptr<Skydome> skydome;
	std::unique_ptr<Model> modelSkydome;

	std::unique_ptr<RailCamera> railCamera;

	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> playerModel_;
	uint32_t playerTextureHandle_;
	uint32_t reticleTextureHandle_;
	std::unique_ptr<Model> bulletModel_;

	std::unique_ptr<BeatsCircleManager> beartsCircleManager_;
	std::unique_ptr<Model> targetModel_;
	std::unique_ptr<Model> circleModel_;
	float damageMagnification_;

	std::unique_ptr<EnemyManager> enemyManager_;
	std::unique_ptr<Model> enemyModel_;

	std::unique_ptr<DebugCamera> debugCamera;
	bool debugCameraActive = false;

	std::unique_ptr<CatmulRom> catmulRomEdit;

	float deltaTime_;

private: // メンバ関数
	void CheckCollsion();

	void InGameInit();
};
