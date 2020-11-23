#ifndef RK4_INTEGRATOR_H_
#define RK4_INTEGRATOR_H_

#include "IntegratorBase.hpp"

namespace GLOO {
	template <class TSystem, class TState>
	class RK4Integrator : public IntegratorBase<TSystem, TState> {
		TState Integrate(const TSystem& system,
			const TState& state,
			float start_time,
			float dt) const override {
			//glm::vec3 new_state = state + dt * system.ComputeTimeDerivative(state, start_time);
			//return new_state;
			TState k1 = system.ComputeTimeDerivative(state, start_time);
			TState k2 = system.ComputeTimeDerivative(state + dt/2.f*k1, start_time + dt/2.f);
			TState k3 = system.ComputeTimeDerivative(state + dt / 2.f * k1, start_time + dt / 2.f);
			TState k4 = system.ComputeTimeDerivative(state + dt*k3, start_time + dt);
			TState new_state = state + dt / 6.f * (k1 + 2.f * k2 + 2.f * k3 + k4);
			return new_state;
		}
	};
}  // namespace GLOO

#endif
