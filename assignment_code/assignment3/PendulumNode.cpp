#include "PendulumNode.hpp"
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
#include "ParticleState.hpp"
#include "gloo/InputManager.hpp"


namespace GLOO {

	PendulumNode::PendulumNode(IntegratorType type, float step_size) {
		integrator_ = IntegratorFactory::CreateIntegrator<PendulumSystem, ParticleState>(type);
		sphere_mesh_ = PrimitiveFactory::CreateSphere(0.025f, 25, 25);
		shader_ = std::make_shared<PhongShader>();
		system_ = make_unique<PendulumSystem>();	
		time_ = 0.0;
		step_size_ = step_size;
		Setup(); 
	};

	void PendulumNode::Setup() {
		glm::vec3 start = { 0.5,0.5,0.0 };

		auto fixed_node = make_unique<SceneNode>(); 
		fixed_node->CreateComponent<ShadingComponent>(shader_); 
		fixed_node->CreateComponent<RenderingComponent>(sphere_mesh_);
		fixed_node->GetTransform().SetPosition(start);
		AddChild(std::move(fixed_node));
		system_->AddParticle(0.5); 
		system_->FixPositionForParticle(0);
		state_.positions.push_back(start);
		state_.velocities.push_back({ 0,0,0 });


		auto node1 = make_unique<SceneNode>();
		node1->CreateComponent<ShadingComponent>(shader_);
		node1->CreateComponent<RenderingComponent>(sphere_mesh_);
		node1->GetTransform().SetPosition({ 0.35,0.35,0.35 });
		AddChild(std::move(node1));
		system_->AddParticle(0.5);
		system_->FixPositionForParticle(0);
		system_->AddSpring(0, 1); 
		state_.positions.push_back({ 0.35,0.35,0.35 });
		state_.velocities.push_back({ 0,0,0 });

		auto node2 = make_unique<SceneNode>();
		node2->CreateComponent<ShadingComponent>(shader_);
		node2->CreateComponent<RenderingComponent>(sphere_mesh_);
		node2->GetTransform().SetPosition({ 0.65,0.35,0 });
		AddChild(std::move(node2));
		system_->AddParticle(0.5);
		system_->AddSpring(1, 2);
		state_.positions.push_back({ 0.65,0.35,0 });
		state_.velocities.push_back({ 0,0,0 });

		auto node3 = make_unique<SceneNode>();
		node3->CreateComponent<ShadingComponent>(shader_);
		node3->CreateComponent<RenderingComponent>(sphere_mesh_);
		node3->GetTransform().SetPosition({ 0.65,0.35,0.15 });
		AddChild(std::move(node3));
		system_->AddParticle(0.5);
		system_->AddSpring(2, 3);
		state_.positions.push_back({ 0.65,0.35,0.15 });
		state_.velocities.push_back({ 0,0,0 });

		auto node4 = make_unique<SceneNode>();
		node4->CreateComponent<ShadingComponent>(shader_);
		node4->CreateComponent<RenderingComponent>(sphere_mesh_);
		node4->GetTransform().SetPosition({ 0.65,0.35,-0.15 });
		AddChild(std::move(node4));
		system_->AddParticle(0.5);
		system_->AddSpring(3, 4);
		state_.positions.push_back({ 0.65,0.35,-0.15 });
		state_.velocities.push_back({ 0,0,0 });

		init_state_ = state_;
	}


	void PendulumNode::Update(double delta_time) {
		static bool prev_released = true;
		if (InputManager::GetInstance().IsKeyPressed('R')) {
			if (prev_released) {
				state_ = init_state_;
			}
			prev_released = false;
		}
		else if (InputManager::GetInstance().IsKeyReleased('R')) {
			prev_released = true;
		}

		for (float i = 0; i <= int(delta_time / step_size_); i++) {
			float step = step_size_ < delta_time ? step_size_ : delta_time;
			ParticleState new_state = integrator_->Integrate(*system_, state_, time_, step);
			state_ = new_state;
			for (int i = 0; i < state_.positions.size(); i++) {
				GetChild(i).GetTransform().SetPosition(state_.positions[i]);
			}
			time_ += step_size_;
		}
	}
}