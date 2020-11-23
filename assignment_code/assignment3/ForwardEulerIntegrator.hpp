#ifndef FORWARD_EULER_INTEGRATOR_H_
#define FORWARD_EULER_INTEGRATOR_H_

#include "IntegratorBase.hpp"

namespace GLOO {
template <class TSystem, class TState>
class ForwardEulerIntegrator : public IntegratorBase<TSystem, TState> {
  TState Integrate(const TSystem& system,
                   const TState& state,
                   float start_time,
                   float dt) const override {
	  TState new_state = state + dt*system.ComputeTimeDerivative(state, start_time);
    return new_state;
  }
};
}  // namespace GLOO

#endif
