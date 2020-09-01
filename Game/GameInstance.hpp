#pragma once
#include <Engine/EntCamera.hpp>

class Game;
class InputManager;

class GameInstance
{
	Game* _game;
	EntCamera* _activeCamera;

	float _axisMoveForward;
	float _axisMoveRight;
	float _axisLookUp;
	float _axisLookRight;

	uint16 _w, _h;

public:
	Entity* world;

	static GameInstance& Instance()
	{
		static GameInstance instance;
		return instance;
	}

	GameInstance() : _activeCamera(nullptr), _axisMoveForward(0.f), _axisMoveRight(0.f), _axisLookUp(0.f), _axisLookRight(0.f), _w(0), _h(0) {}

	Game* GetGame() { return _game; }

	void Initialise(Game&);

	void OnResize(uint16 w, uint16 h);

	void SetActiveCamera(EntCamera* camera)		
	{ 
		_activeCamera = camera; 
		if (_activeCamera)
			OnResize(_w, _h);
	}

	const EntCamera* GetActiveCamera() const	{ return _activeCamera; }

	float GetAxisMoveForward() const	{ return _axisMoveForward; }
	float GetAxisMoveRight() const		{ return _axisMoveRight; }
	float GetAxisLookUp() const			{ return _axisLookUp; }
	float GetAxisLookRight() const		{ return _axisLookRight; }
};

