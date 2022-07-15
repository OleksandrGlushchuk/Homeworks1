#pragma once
#include "Scene.h"
#include "../render/globals.hpp"
#include "InputState.h"
#include "../objects/IntersectionQuery.h"
#include "../math/ray.h"
#include "../d3dobjects/PerFrameBuffer.h"

namespace engine::windows
{
	class ControllerD3D
	{
		engine::windows::WindowD3D& wnd;
		engine::windows::SceneD3D& scene;
		engine::DxResPtr<ID3D11Buffer> m_perFrameBuffer;

		WORD mouse_x = 0, mouse_y = 0;
		InputState input_state;

		//---OBJECT MOVER---//
		ray ray_clicked_to_object;
		IntersectionQuery nearest_clicked_object;
		IntersectionQuery new_object_intersection;
		float distance_object_to_camera;
		bool need_to_move_object = false;
		//-----------------//

		//---OBJECT ROTATOR---//
		float object_rotate_angle = 0.2f;
		bool need_to_rotate_object = false;
		bool need_to_rotate_object_relative_to_camera_axes = false;
		//-----------------//

		//---CAMERA MOVER---//
		bool need_to_rotate = false;
		bool need_to_speed_up = false;
		float camera_move_offset_val = 1.0f;
		float camera_angle_offset_val = 3.f;
		float acceleration = 5.f;
		Vec3 start_rotation = Vec3(0, 0, 1), end_rotation = Vec3(0, 0, 1), dir_rotation = Vec3(0, 0, 1);
		//-----------------//


		void InitCameraBuffer();
		void UpdateCameraBuffer();
	public:
		float delta_time;
		ControllerD3D(engine::windows::WindowD3D &_wnd, engine::windows::SceneD3D& _scene) : wnd(_wnd), scene(_scene){}

		void RotateCamera();
		void moveCamera(const Vec3& offset, const Angles& angles);

		void ProcessInput();
		void OnKeyDown(WPARAM key);
		void OnKeyUp(WPARAM key);

		void OnLMouseDown(WORD x, WORD y);
		void OnLMouseMove(WORD x, WORD y);
		void OnLMouseUp(WORD x, WORD y);

		void OnRMouseDown(WORD x, WORD y);
		void OnRMouseMove(WORD x, WORD y);
		void OnRMouseUp(WORD x, WORD y);

		void OnMouseWheel(short wheel_data);

		void DrawScene();
		void OnChangeWindowSize();
		void InitSamplerStates();
		void InitTextures();
		void InitScene();
	};
}