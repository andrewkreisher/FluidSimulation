#include "gloo/SceneNode.hpp"
#include "SingleParticleSystem.hpp"
#include "IntegratorBase.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "IntegratorFactory.hpp"
#include "IntegratorType.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "glm/gtx/string_cast.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "SingleNode.hpp"


namespace GLOO {

	SingleNode::SingleNode(IntegratorType type, float step_size) {
		glm::vec3 start = { -1.3,0,0 };
		integrator_ = IntegratorFactory::CreateIntegrator<SingleParticleSystem, glm::vec3>(type);
		sphere_mesh_ = PrimitiveFactory::CreateSphere(0.1f, 25, 25);
		shader_ = std::make_shared<PhongShader>();
		CreateComponent<ShadingComponent>(shader_);
		CreateComponent<RenderingComponent>(sphere_mesh_);
		GetTransform().SetPosition({ -1.3,0,0 }); 
		system_ = make_unique<SingleParticleSystem>();
		state_ = { 0.2, 0.2, 0.0 };
		time_ = 0.0;
		step_size_ = step_size;
		printf("%f", step_size);
	};

	void SingleNode::Update(double delta_time) {
		//glm::vec3 new_pos = { state_[0] + delta_time*-state_[1], state_[1]+delta_time*state_[0], state_[2]}; 
		//std::cout << glm::to_string(pos) << std::endl;

		for (float i = 0; i <= int(delta_time/step_size_); i ++) {
			float step = step_size_ < delta_time ? step_size_ : delta_time;
			glm::vec3 new_pos = integrator_->Integrate(*system_, state_, time_, step);
			state_ = new_pos;
			this->GetTransform().SetPosition(state_);
			time_ += step_size_; 
		}
		//glm::vec3 new_pos = integrator_->Integrate(*system_, state_, time_, delta_time);
		////std::cout << glm::to_string(new_pos) << std::endl;
		//state_ = new_pos; 
		//GetTransform().SetPosition(state_); 
	}
} 