#pragma once
#include "window.h"
#include "Renderer.h"
#include "../render/globals.hpp"
#include "InputState.h"
#include "../math/ray.h"


namespace engine::windows
{
	class Controller
	{
		engine::windows::Window& wnd;
		engine::windows::Renderer& renderer;

		bool need_to_move_camera = false;
		WORD mouse_x = 0, mouse_y = 0;
		InputState input_state;

		//---OBJECT MOVER---//
		ray ray_clicked_to_object;
		uint32_t clicked_object_transform_id;
		engine::MeshIntersection nearest_clicked_object;
		float distance_object_to_camera;
		bool need_to_move_object = false;
		//-----------------//

		//---OBJECT ROTATOR---//
		float object_rotate_angle = 0.2f;
		bool need_to_rotate_object = false;
		bool need_to_rotate_object_relative_to_camera_axes = false;
		//-----------------//

		//---CAMERA MOVER---//
		bool need_to_rotate_camera = false;
		bool need_to_speed_up = false;
		float camera_move_offset_val = 1.0f;
		float camera_angle_offset_val = 3.f;
		float acceleration = 5.f;
		Vec3 start_rotation = Vec3(0, 0, 1), end_rotation = Vec3(0, 0, 1), dir_rotation = Vec3(0, 0, 1);
		//-----------------//

		void RotateCamera();
		void moveCamera(const Vec3& offset, const Angles& angles);
		void ProcessInput();
	public:
		float delta_time;
		Controller(engine::windows::Window &_wnd, engine::windows::Renderer& _scene) : wnd(_wnd), renderer(_scene){}

		void OnKeyDown(WPARAM key);
		void OnKeyUp(WPARAM key);

		void OnLMouseDown(WORD x, WORD y);
		void OnLMouseMove(WORD x, WORD y);
		void OnLMouseUp(WORD x, WORD y);

		void OnRMouseDown(WORD x, WORD y);
		void OnRMouseMove(WORD x, WORD y);
		void OnRMouseUp(WORD x, WORD y);

		void OnMouseWheel(short wheel_data);

		void Update();
		void Draw();
		void OnChangeWindowSize();
		void InitScene();
	};
}