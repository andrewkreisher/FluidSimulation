#include "FluidNode3.hpp"
#include "FluidNode2.hpp"
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

	FluidNode3::FluidNode3(IntegratorType type, float step_size) {
		integrator_ = IntegratorFactory::CreateIntegrator<FluidSystem, ParticleState>(type);
		sphere_mesh_ = PrimitiveFactory::CreateSphere(0.08f, 25, 25);
		shader_ = std::make_shared<PhongShader>();
		system_ = make_unique<FluidSystem>();
		time_ = 0.0;
		step_size_ = step_size;
		dimension_ = 5;
		lines_ = std::make_shared<VertexObject>();
		floor_w_ = 2;
		box_dim_ = 0.65;
		Setup();
	};

	int FluidNode3::IndexOf(int i, int j, int z) {
		return dimension_ * dimension_ * i + dimension_ * j + z;
	}

	void FluidNode3::Setup() {
		for (int i = 0; i < dimension_; i++) {
			for (int j = 0; j < dimension_; j++) {
				for (int z = 0; z < dimension_; z++) {
					auto node = make_unique<SceneNode>();
					node->CreateComponent<ShadingComponent>(shader_);
					node->CreateComponent<RenderingComponent>(sphere_mesh_);
					auto material = std::make_shared<Material>(Material::GetDefault());
					node->CreateComponent<MaterialComponent>(material);
					//glm::vec3 pos = { 1 + 0.15 * j,  0,1 - 0.15 * i };
					float sep_x = 0.2;
					float sep_y = 0.2;
					float sep_z = 0.2;
					glm::vec3 pos = { -0.35 + sep_x * z,  1.5 + sep_y * j , 0.25 + sep_z * i };
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

		drawBox(true);
		drawBox(false);

		init_state_ = state_;
	}

	void FluidNode3::drawBox(bool filled) {

		//draw floor
		auto lines_ = std::make_shared<VertexObject>();
		auto positions = make_unique<PositionArray>();
		auto indices = make_unique<IndexArray>();
		auto normals = make_unique<NormalArray>();
		float z = -0.1;
		float r = 0.075;
		float box_dim_c = box_dim_ + r;
		if (!filled) {
			positions->push_back({ -r, -r, -r }); //0
			positions->push_back({ -r, -r, box_dim_c }); //1 
			positions->push_back({ -r, box_dim_c, -r }); //2 
			positions->push_back({ box_dim_c, -r, -r }); //3
			positions->push_back({ box_dim_c, box_dim_c, -r }); //4 
			positions->push_back({ box_dim_c, -r, box_dim_c }); // 5
			positions->push_back({ -r, box_dim_c, box_dim_c }); //6 
			positions->push_back({ box_dim_c, box_dim_c, box_dim_c }); // 7
		}
		else {
			positions->push_back({ -r-0.01, -r, -r-0.01 }); //0
			positions->push_back({ -r, -r, box_dim_c }); //1 
			positions->push_back({ -r - 0.01, box_dim_c, -r - 0.01 }); //2 
			positions->push_back({ box_dim_c, -r, -r }); //3
			positions->push_back({ box_dim_c, box_dim_c, -r }); //4 
			positions->push_back({ box_dim_c, -r, box_dim_c }); // 5
			positions->push_back({ -r, box_dim_c, box_dim_c }); //6 
			positions->push_back({ box_dim_c, box_dim_c, box_dim_c }); // 7
		}
		

	

		//wireframe
		if (!filled) {
			indices->push_back(0);
			indices->push_back(1);
			indices->push_back(0);
			indices->push_back(2);
			indices->push_back(0);
			indices->push_back(3);
			indices->push_back(2);
			indices->push_back(4);
			indices->push_back(3);
			indices->push_back(4);
			indices->push_back(1);
			indices->push_back(5);
			indices->push_back(3);
			indices->push_back(5);
			indices->push_back(1);
			indices->push_back(6);
			indices->push_back(2);
			indices->push_back(6);
			indices->push_back(4);
			indices->push_back(7);
			indices->push_back(5);
			indices->push_back(7);
			indices->push_back(6);
			indices->push_back(7);
		}
		else {
			//faces
			indices->push_back(0);
			indices->push_back(1);
			indices->push_back(2);
			normals->push_back({ -1,0,0 });

			indices->push_back(1);
			indices->push_back(2);
			indices->push_back(6);
			normals->push_back({ -1,0,0 });

			indices->push_back(0);
			indices->push_back(2);
			indices->push_back(3);
			normals->push_back({ 0,0,-1});


			indices->push_back(2);
			indices->push_back(3);
			indices->push_back(4);
			normals->push_back({0,0,-1});


			indices->push_back(0);
			indices->push_back(1);
			indices->push_back(5);
			normals->push_back({ 0,1,0 });


			indices->push_back(0);
			indices->push_back(3);
			indices->push_back(5);
			normals->push_back({ 0,1,0 });


			/*indices->push_back(3);
			indices->push_back(5);
			indices->push_back(7);


			indices->push_back(1);
			indices->push_back(5);
			indices->push_back(7)*/;


			//indices->push_back(1);
			//indices->push_back(6);
			//indices->push_back(7);


			//indices->push_back(3);
			//indices->push_back(4);
			//indices->push_back(7);


		}





		lines_->UpdatePositions(std::move(positions));
		lines_->UpdateIndices(std::move(indices));
		lines_->UpdateNormals(std::move(normals));
		auto line_node = make_unique<SceneNode>();
		std::shared_ptr<ShaderProgram> shader;
		if (filled) {
			 shader = std::make_shared<SimpleShader>();
		}
		else {
			 shader = std::make_shared<SimpleShader>();
		}
			
		line_node->CreateComponent<ShadingComponent>(shader);
		auto& rc = line_node->CreateComponent<RenderingComponent>(lines_);
		if (filled) {
			rc.SetDrawMode(DrawMode::Triangles);
		}
		else {
			rc.SetDrawMode(DrawMode::Lines);
		}
		glm::vec3 color;
		if (filled) {
			color = { 0.2,0.2,0.2 };
		}
		else {
			color = { 1,1,0 };
		}
		//glm::vec3 color(0.2, 0.2, 0.2);
		auto material = std::make_shared<Material>(color, color, color, 0);
		line_node->CreateComponent<MaterialComponent>(material);
		AddChild(std::move(line_node));
	}

	void FluidNode3::Update(double delta_time) {
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
				if (state_.positions[i][1] < 0.00 && state_.positions[i][0] >= 0.00
					&& state_.positions[i][0] < box_dim_ + 0.075 && state_.positions[i][2] >= 0.00
					&& state_.positions[i][2] < box_dim_ + 0.075) {

					glm::vec3 bound_norm = { 0,1,0 };
					glm::vec3 vel_i = state_.velocities[i];
					vel_i = vel_i - (1 + 0.4f) * (glm::dot(vel_i, bound_norm)) * bound_norm;
					state_.velocities[i] = vel_i;
					state_.positions[i][1] = -state_.positions[i][1];

				}
				else if (state_.positions[i][1] < box_dim_ && state_.positions[i][2] >= 0.00
					&& state_.positions[i][2] < box_dim_) {
					if (state_.positions[i][0] < 0.00 && state_.positions[i][0] > -0.2) {
						glm::vec3 bound_norm = { 1,0,0 };
						glm::vec3 vel_i = state_.velocities[i];
						vel_i = vel_i - (1 + 0.4f) * (glm::dot(vel_i, bound_norm)) * bound_norm;
						state_.velocities[i] = vel_i;
						state_.positions[i][0] = -state_.positions[i][0];
					}
					/*if (state_.positions[i][0] > box_dim_ && state_.positions[i][0] < box_dim_ + 0.2) {
						glm::vec3 bound_norm = { -1,0,0 };
						glm::vec3 vel_i = state_.velocities[i];
						vel_i = vel_i - (1 + 0.4f) * (glm::dot(vel_i, bound_norm)) * bound_norm;
						state_.velocities[i] = vel_i;
						state_.positions[i][0] = 2 * box_dim_ - state_.positions[i][0];
					}*/
				}
				else if (state_.positions[i][1] < box_dim_ && state_.positions[i][0] >= 0.00
					&& state_.positions[i][0] < box_dim_) {
					if (state_.positions[i][2] < 0.00 && state_.positions[i][2] > -0.2) {
						glm::vec3 bound_norm = { 0,0,1 };
						glm::vec3 vel_i = state_.velocities[i];
						vel_i = vel_i - (1 + 0.4f) * (glm::dot(vel_i, bound_norm)) * bound_norm;
						state_.velocities[i] = vel_i;
						state_.positions[i][2] = -state_.positions[i][2];
					}
					/*if (state_.positions[i][2] > box_dim_ && state_.positions[i][2] < box_dim_ + 0.2) {
						glm::vec3 bound_norm = { 0,0,-1 };
						glm::vec3 vel_i = state_.velocities[i];
						vel_i = vel_i - (1 + 0.4f) * (glm::dot(vel_i, bound_norm)) * bound_norm;
						state_.velocities[i] = vel_i;
						state_.positions[i][2] = 2 * box_dim_ - state_.positions[i][2];
					}*/
				}


				GetChild(i).GetTransform().SetPosition(state_.positions[i]);
			}

			time_ += step_size_;
		}
	}

}