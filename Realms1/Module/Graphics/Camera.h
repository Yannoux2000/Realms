#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "../../Base/Math/VoxelMath.h"

#include <memory>

namespace rlms {
	class Camera {
	private:
		static constexpr glm::vec3 DEF_POSITION = glm::vec3 (0.0f, -100.0f, 16.0f);
		static constexpr float DEF_YAW = 90.0f;
		static constexpr float DEF_PITCH = 0.0f;
		static constexpr float DEF_ZOOM = 30.0f;
		static constexpr glm::vec3 WORLD_UP = glm::vec3 (0.0f, 0.0f, 1.0f);

	public:
		//OPTION DEFINED
		static constexpr float SENSITIVITY = 0.1f;

		glm::vec3 position;
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;

		float zoom;

		float pitch;
		float yaw;

		static std::shared_ptr<Camera> MainCamera;

		Camera (glm::vec3 pos = DEF_POSITION, float t_yaw = DEF_YAW, float t_pitch = DEF_PITCH, float zm = DEF_ZOOM);
		~Camera ();

		static void CreateMainCamera (glm::vec3 pos = DEF_POSITION, float t_yaw = DEF_YAW, float t_pitch = DEF_PITCH, float zm = DEF_ZOOM);

		static glm::mat4 getMainView ();
		static glm::mat4 getMainPerspectivePtr ();

		void setMainCamera ();
		glm::mat4 getView ();
		glm::mat4 getPerspective ();
		uint8_t const getBFCullingflags ();

		void reset ();
		void update ();
		void look (glm::ivec2 delta);
		void rotate (float rot);
		void rotation (float rot);
		void move (glm::vec3 pos);
		void postion (glm::vec3 pos);
	};
}
