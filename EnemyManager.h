#pragma once
#include "Enemy.h"
#include "MyLib.h"
#include "ViewProjection.h"
#include <fstream>

class EnemyManager {

public:
	void Initialize(Model* _model, uint32_t _textrueHandle);
	void Update();
	void Draw(const ViewProjection& _viewProjection);
	void DrawMoveLanes();

	void Damage(float _damage);

	const std::list<std::unique_ptr<Enemy>>& GetEnemyList() { return enemies_; };

private:
	void LoadPopData();
	void PopCommandUpdate();
	void Pop(const Vector3& _pos,uint32_t _laneNum);
	Vector3 GetPositionOnLane(uint32_t _lane, float _t);

	void CalculateLane();

	Model* model_;
	uint32_t textureHandle_;

	std::list<std::unique_ptr<Enemy>> enemies_;

	std::ifstream file_;

	uint32_t waitTime_;
	bool isWaitig_;

	size_t laneNum_;
	Vector3 centerLane_;
	float laneMargin_;
	std::vector<Line> moveLanes_;
};