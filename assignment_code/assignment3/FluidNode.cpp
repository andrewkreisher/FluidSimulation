#include "FluidNode.hpp"
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

	FluidNode::FluidNode(IntegratorType type, float step_size) {
		integrator_ = IntegratorFactory::CreateIntegrator<PendulumSystem, ParticleState>(type);
		sphere_mesh_ = PrimitiveFactory::CreateSphere(0.05f, 25, 25);
		shader_ = std::make_shared<PhongShader>();
		system_ = make_unique<PendulumSystem>();
		time_ = 0.0;
		step_size_ = step_size;
		dimension_ = 4;
		lines_ = std::make_shared<VertexObject>();
		Setup();
	};

	int FluidNode::IndexOf(int i, int j, int z) {
		return dimension_*dimension_ * i + dimension_ * j + z;
	}

	void FluidNode::Setup() {
		for (int i = 0; i < dimension_; i++) {
			for (int j = 0; j < dimension_; j++) {
				for (int z = 0; z < dimension_; z++) {
					auto node = make_unique<SceneNode>();
					node->CreateComponent<ShadingComponent>(shader_);
					node->CreateComponent<RenderingComponent>(sphere_mesh_);
					auto material = std::make_shared<Material>(Material::GetDefault());
					node->CreateComponent<MaterialComponent>(material);
					//glm::vec3 pos = { 1 + 0.15 * j,  0,1 - 0.15 * i };
					glm::vec3 pos = { 0.15 * z, 0.15 * j , 0.15 * i };
					node->GetTransform().SetPosition(pos);
					system_->AddParticle(0.1);
					//positions->push_back(pos);
					state_.positions.push_back(pos);
					state_.velocities.push_back({ 0,0,0 });
					Material& mat = node->GetComponentPtr<MaterialComponent>()->GetMaterial();
					mat.SetAmbientColor(glm::vec3(0.0f, 0.0f, 1.0f));
					mat.SetDiffuseColor(glm::vec3(0.0f, 0.0f, 1.0f));
					AddChild(std::move(node));
				}
			}
		}


		for (int i = 0; i < dimension_; i++) {
			for (int j = 0; j < dimension_; j++) {
				for (int z = 0; z < dimension_; z++) {
					if (i != dimension_ - 1) {
						system_->AddSpring(IndexOf(i, j, z), IndexOf(i + 1, j, z));
						//indices_->push_back(IndexOf(i, j, z));
						//indices_->push_back(IndexOf(i + 1, j, z));
					}
					if (j != dimension_ - 1) {
						system_->AddSpring(IndexOf(i, j, z), IndexOf(i, j + 1, z));
						//indices_->push_back(IndexOf(i, j, z));
						//indices_->push_back(IndexOf(i, j + 1, z));
					}
					if (z != dimension_ - 1) {
						system_->AddSpring(IndexOf(i, j, z), IndexOf(i, j, z+1));
						//indices_->push_back(IndexOf(i, j, z));
						//indices_->push_back(IndexOf(i, j, z+1));
					}
				}
				
			}
		}

		init_state_ = state_;

		/*Material& mat = GetChild(IndexOf(0,0,3)).GetComponentPtr<MaterialComponent>()->GetMaterial();
		mat.SetAmbientColor(glm::vec3(0.0f, 1.0f, 0.0f));
		mat.SetDiffuseColor(glm::vec3(0.0f, 1.0f, 0.0f));
		Material& mat1 = GetChild(IndexOf(0, 2, 0)).GetComponentPtr<MaterialComponent>()->GetMaterial();
		mat1.SetAmbientColor(glm::vec3(0.0f, 1.0f, 0.0f));
		mat1.SetDiffuseColor(glm::vec3(0.0f, 1.0f, 0.0f));
		Material& mat2 = GetChild(IndexOf(1,0,0)).GetComponentPtr<MaterialComponent>()->GetMaterial();
		mat2.SetAmbientColor(glm::vec3(0.0f, 1.0f, 0.0f));
		mat2.SetDiffuseColor(glm::vec3(0.0f, 1.0f, 0.0f));*/
	}

	void FluidNode::Update(double delta_time) {
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
			//auto positions = make_unique<PositionArray>();
			//for (int j = 0; j < state_.positions.size(); j++) {
				//positions->push_back(state_.positions.at(j));
			//}
			//lines_->UpdatePositions(std::move(positions));
			time_ += step_size_;
		}
	}

}