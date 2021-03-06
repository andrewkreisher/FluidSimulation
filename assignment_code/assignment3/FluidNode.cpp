#include "FluidNode.hpp"
#include "FluidSystem.hpp"
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
		integrator_ = IntegratorFactory::CreateIntegrator<FluidSystem, ParticleState>(type);
		sphere_mesh_ = PrimitiveFactory::CreateSphere(0.08f, 25, 25);
		shader_ = std::make_shared<PhongShader>();
		system_ = make_unique<FluidSystem>();
		time_ = 0.0;
		step_size_ = step_size;
		dimension_ = 6;
		lines_ = std::make_shared<VertexObject>();
		floor_w_ = 2;
		Setup();
	};

	int FluidNode::IndexOf(int i, int j, int z) {
		return dimension_*dimension_ * i + dimension_ * j + z;
	}

	void FluidNode::Setup() {
		for (int i = 0; i < dimension_; i++) {
			for (int j = 0; j < 8; j++) {
				for (int z = 0; z < dimension_; z++) {
					auto node = make_unique<SceneNode>();
					node->CreateComponent<ShadingComponent>(shader_);
					node->CreateComponent<RenderingComponent>(sphere_mesh_);
					auto material = std::make_shared<Material>(Material::GetDefault());
					node->CreateComponent<MaterialComponent>(material);
					//glm::vec3 pos = { 1 + 0.15 * j,  0,1 - 0.15 * i };
					float sep_x = 0.10;
					float sep_y = 0.12;
					float sep_z = 0.10;
					glm::vec3 pos = { sep_x * z, 0.5 + sep_y * j , sep_z * i };
					node->GetTransform().SetPosition(pos);
					system_->AddParticle(0.2);
					//positions->push_back(pos);
					state_.positions.push_back(pos);
					state_.velocities.push_back({ 0,0,0 });
					Material& mat = node->GetComponentPtr<MaterialComponent>()->GetMaterial();
					mat.SetAmbientColor(glm::vec3(0, 0.5, 1));
					mat.SetDiffuseColor(glm::vec3(0, 0.5, 1));
					AddChild(std::move(node));
				}
			}
		}

		//

		//draw floor
		auto lines_ = std::make_shared<VertexObject>();
		auto positions = make_unique<PositionArray>();
		auto indices = make_unique<IndexArray>();
		
		float z = -0.1;
		positions->push_back({ -floor_w_, z, -floor_w_ });
		positions->push_back({ -floor_w_, z, floor_w_ });
		positions->push_back({ floor_w_, z, -floor_w_ });
		positions->push_back({ floor_w_, z, floor_w_ });
		
		indices->push_back(0); 
		indices->push_back(1);
		indices->push_back(2);
		indices->push_back(1);
		indices->push_back(2);
		indices->push_back(3);

		lines_->UpdatePositions(std::move(positions));
		lines_->UpdateIndices(std::move(indices));
		auto line_node = make_unique<SceneNode>();
		auto shader = std::make_shared<SimpleShader>();
		line_node->CreateComponent<ShadingComponent>(shader);
		auto& rc = line_node->CreateComponent<RenderingComponent>(lines_);
		rc.SetDrawMode(DrawMode::Triangles);
		glm::vec3 color(0.2, 0.2, 0.2);
		auto material = std::make_shared<Material>(color, color, color, 0);
		line_node->CreateComponent<MaterialComponent>(material);
		AddChild(std::move(line_node));
		
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
				if (state_.positions[i][1] < 0.00 && abs(state_.positions[i][2]) <= floor_w_ 
					&& abs(state_.positions[i][0]) <= floor_w_) {
						glm::vec3 bound_norm = { 0,1,0 };
						glm::vec3 vel_i = state_.velocities[i];
						vel_i = vel_i - (1 + 0.4f) * (glm::dot(vel_i, bound_norm)) * bound_norm;
						state_.velocities[i] = vel_i;
						state_.positions[i][1] = -state_.positions[i][1];
				}
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