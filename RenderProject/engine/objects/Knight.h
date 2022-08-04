#pragma once
#include "../render/globals.hpp"
class Knight
{

public:
	Knight() {}
	static void Init()
	{
		s_model = engine::ModelManager::instance().LoadModel("source/assets/Knight/Knight.fbx");
	}
	engine::ModelID ID;
	static std::shared_ptr<Model> s_model;
};