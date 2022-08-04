#pragma once
#include "../render/globals.hpp"

class Samurai
{
public:
	Samurai(){}
	static void Init()
	{
		s_model = engine::ModelManager::instance().LoadModel("source/assets/Samurai/Samurai.fbx");
	}
	engine::ModelID ID;
	static std::shared_ptr<Model> s_model;
};