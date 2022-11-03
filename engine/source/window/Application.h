#pragma once
#include "window.h"
#include "Renderer.h"
#include "InputState.h"
#include "../math/ray.h"
#include "../math/camera.h"
#include "../render/PostProcess.h"


namespace engine::windows
{
	class Application
	{
		Window& wnd;
		Renderer& renderer;
		Camera camera;
		PostProcess m_postProcess;

		bool time_stopped = false;
		std::chrono::steady_clock::time_point stop_time_point;
		std::chrono::steady_clock::time_point unstop_time_point;
		std::chrono::steady_clock::duration stop_duration = std::chrono::steady_clock::duration(0);

		std::chrono::steady_clock::time_point m_currentTime;

		bool need_to_create_dissoluble_samurai_material = true;
		std::vector<OpaqueInstances::Material> m_samuraiMaterial = std::vector<OpaqueInstances::Material>(8);
		std::vector<OpaqueInstances::Material> m_knightMaterial = std::vector<OpaqueInstances::Material>(9);

		std::vector<OpaqueInstances::Material> m_dissolubleSamuraiMaterial = std::vector<OpaqueInstances::Material>(8);
		std::vector<OpaqueInstances::Material> m_dissolubleSamuraiMaterial1 = std::vector<OpaqueInstances::Material>(8);
		std::vector<OpaqueInstances::Material> m_dissolubleKnightMaterial = std::vector<OpaqueInstances::Material>(9);
		std::vector<OpaqueInstances::Material> m_dissolubleKnightMaterial1 = std::vector<OpaqueInstances::Material>(9);
		std::vector<OpaqueInstances::Material> m_dissolubleKnightMaterial2 = std::vector<OpaqueInstances::Material>(9);


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
		void MoveCamera(const Vec3& offset);
		void ProcessInput();
		void CheckDissolutionObjects();
		void CheckIncinerationObjects();
		void UpdateCurrentTime();
	public:
		float delta_time = 0;
		Application(engine::windows::Window &_wnd, engine::windows::Renderer& _scene) : wnd(_wnd), renderer(_scene){}

		void OnKeyDown(WPARAM key);
		void OnKeyUp(WPARAM key);

		void OnMouseMove(WORD x, WORD y);

		void OnLMouseDown(WORD x, WORD y);
		void OnLMouseMove();
		void OnLMouseUp(WORD x, WORD y);

		void OnRMouseDown(WORD x, WORD y);
		void OnRMouseMove();
		void OnRMouseUp(WORD x, WORD y);

		void OnEnterSizeMove();
		void OnExitSizeMove();

		void OnMouseWheel(short wheel_data);

		void Draw();
		void OnChangeWindowSize();
		void Init();
	};
}