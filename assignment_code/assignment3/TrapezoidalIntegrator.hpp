#ifndef TRAPEZOIDAL_INTEGRATOR_H_
#define TRAPEZOIDAL_INTEGRATOR_H_

#include "IntegratorBase.hpp"

namespace GLOO {
	template <class TSystem, class TState>
	class TrapezoidalIntegrator : public IntegratorBase<TSystem, TState> {
		TState Integrate(const TSystem& system,
			const TState& state,
			float start_time,
			float dt) const override {
			//glm::vec3 new_state = state + dt * system.ComputeTimeDerivative(state, start_time);
			//return new_state;
			TState f_0 = system.ComputeTimeDerivative(state, start_time);
			TState f_1 = system.ComputeTimeDerivative(state + dt * f_0, start_time + dt);
			TState new_state = state + float(dt) / float(2.0) * (f_0 + f_1);
			return new_state;
		}
	};
}  // namespace GLOO

#endif
