#include <cassert>

#include "reversead/activetype/base_active.hpp"
#include "reversead/checkpointing/iterative_func_fixed.hpp"

namespace ReverseAD {

IterativeFuncFixed::IterativeFuncFixed(
    size_t x_num, size_t t_num, size_t y_num,
    void (*set_up)(),
    void (*tear_down)(),
    void (*initial_step)(adouble*, size_t, adouble*, size_t),
    void (*iteration_step)(adouble*, size_t),
    void (*final_step)(adouble*, size_t, adouble*, size_t),
    size_t max_num_iter) :
  IterativeFuncBase(x_num, t_num, y_num, set_up, tear_down,
                    initial_step, iteration_step, final_step) {

  _max_num_iter = max_num_iter;
}

void IterativeFuncFixed::iteration_init() {
  _curr_iter = 0;
}

bool IterativeFuncFixed::iteration_done(adouble* t_values, size_t t_num) {
  assert(t_num == _t_num);
  return (_curr_iter++) >= _max_num_iter;
}

} // namespace ReverseAD
