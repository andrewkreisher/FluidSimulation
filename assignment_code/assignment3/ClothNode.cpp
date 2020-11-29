#include "ClothNode.hpp"
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
#include "gloo/shaders/SimpleShader.hpp"



namespace GLOO {

	ClothNode::ClothNode(IntegratorType type, float step_size) {
		integrator_ = IntegratorFactory::CreateIntegrator<PendulumSystem, ParticleState>(type);
		sphere_mesh_ = PrimitiveFactory::CreateSphere(0.010f, 25, 25);
		shader_ = std::make_shared<PhongShader>();
		system_ = make_unique<PendulumSystem>();
		time_ = 0.0;
		step_size_ = step_size;
		dimension_ = 16;
		lines_ =  std::make_shared<VertexObject>();
		Setup();
	};

	int ClothNode::IndexOf(int i, int j) {
		return dimension_ * j + i;
	}

	void ClothNode::Setup() {
		glm::vec3 start = { 0.5,0.5,0.0 };

		auto lines = std::make_shared<VertexObject>();
		auto positions = make_unique<PositionArray>();
		indices_ = make_unique<IndexArray>();
		

		for (int i = 0; i < dimension_; i++) {
			for (int j = 0; j < dimension_; j++) {
				auto node = make_unique<SceneNode>();
				node->CreateComponent<ShadingComponent>(shader_);
				node->CreateComponent<RenderingComponent>(sphere_mesh_);
				//glm::vec3 pos = { 1 + 0.15 * j,  0,1 - 0.15 * i };
				glm::vec3 pos = { 1 + 0.15 * j,  1 - 0.15 * i , 0};
				node->GetTransform().SetPosition(pos);
				system_->AddParticle(0.1);
				positions->push_back(pos);
				state_.positions.push_back(pos);
				state_.velocities.push_back({ 0,0,0 });
				AddChild(std::move(node));
			}
		}

		//structural springs

		for (int i = 0; i < dimension_; i++) {
			for (int j = 0; j < dimension_; j++) {
				if (i != dimension_ - 1) {
					system_->AddSpring(IndexOf(i, j), IndexOf(i + 1, j));
					indices_->push_back(IndexOf(i, j));
					indices_->push_back(IndexOf(i + 1, j));
				}
				if (j != dimension_ - 1) {
					system_->AddSpring(IndexOf(i, j), IndexOf(i, j + 1));
					indices_->push_back(IndexOf(i, j));
					indices_->push_back(IndexOf(i, j + 1));
				}
			}
		}

		// shear springs


		for (int i = 0; i < dimension_; i++) {
			for (int j = 0; j < dimension_-1; j++) {
				if (i != 0) {
					system_->AddSpring(IndexOf(i, j), IndexOf(i - 1, j+1));
					//indices_->push_back(IndexOf(i, j));
					//indices_->push_back(IndexOf(i -1, j+1));
				}
				if (i != dimension_ - 1) {
					system_->AddSpring(IndexOf(i, j), IndexOf(i + 1, j + 1));
					//indices_->push_back(IndexOf(i, j));
					//indices_->push_back(IndexOf(i + 1, j + 1));
				}
			}
		}

		//flex springs 

		for (int i = 0; i < dimension_; i++) {
			for (int j = 0; j < dimension_; j++) {
				if (i < dimension_ - 2) {
					system_->AddSpring(IndexOf(i, j), IndexOf(i + 2, j));
					//indices_->push_back(IndexOf(i, j));
					//indices_->push_back(IndexOf(i + 2, j));
				}
				if (j < dimension_ - 2) {
					system_->AddSpring(IndexOf(i, j), IndexOf(i, j + 2));
					//indices_->push_back(IndexOf(i, j));
					//indices_->push_back(IndexOf(i, j + 2));
				}
			}
		}


		//draw lines
		
		lines_->UpdatePositions(std::move(positions));
		lines_->UpdateIndices(std::move(indices_));
		auto line_node = make_unique<SceneNode>();
		auto shader = std::make_shared<SimpleShader>();
		line_node->CreateComponent<ShadingComponent>(shader);
		auto& rc = line_node->CreateComponent<RenderingComponent>(lines_);
		rc.SetDrawMode(DrawMode::Lines);
		glm::vec3 color(1.f, 0.f, 0.f);
		auto material = std::make_shared<Material>(color, color, color, 0);
		line_node->CreateComponent<MaterialComponent>(material);
		AddChild(std::move(line_node));

		//fix ends
		system_->FixPositionForParticle(0);
		system_->FixPositionForParticle(dimension_-1);


		init_state_ = state_;
	}


	void ClothNode::Update(double delta_time) {
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
			auto positions = make_unique<PositionArray>();
			for (int j = 0; j < state_.positions.size(); j++) {
				positions->push_back(state_.positions.at(j));
			}
			lines_->UpdatePositions(std::move(positions));	
			time_ += step_size_;
		}
	}
}