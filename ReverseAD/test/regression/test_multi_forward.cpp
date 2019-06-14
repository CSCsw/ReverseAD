#include <memory>
#include <iostream>
#include "reversead/reversead.hpp"

using ReverseAD::adouble;
using ReverseAD::BaseActive;
using ReverseAD::TrivialTrace;
using ReverseAD::MultiForward;
using ReverseAD::trace_on;
using ReverseAD::trace_off;
using ReverseAD::BaseFunctionReplay;
using ReverseAD::BaseReverseHessian;
using ReverseAD::BaseReverseThird;
using ReverseAD::BaseReverseGeneric;
using ReverseAD::DerivativeTensor;

#define N 3
#define DIRECTION 5

double myEps = 1e-10;

void symmetric_third_vector(
    size_t n, size_t t_size, size_t** t_ind, double* t_value,
    double* v, double** tv);

template <typename T>
std::shared_ptr<TrivialTrace<T>> foo(T a, T b, T c) {
  typedef BaseActive<T> AT;
  T y;
  AT af, bf, cf, yf;
  ReverseAD::trace_on<T>();
  af <<= a;
  bf <<= b;
  cf <<= c;
  af = 20 * asin(sin(af/20));
  bf = 20 * acos(cos(bf/20));
  cf = 20 * atan(tan(cf/20));
  af--;
  bf++;
  cf *= erf(--af) * sqrt(pow(++bf, 2.0));
  
  yf = af * bf * cf;
  yf >>= y;
  return ReverseAD::trace_off<T>();
}
template <typename T>
std::shared_ptr<TrivialTrace<T>> bar(T a, T b, T c) {
  typedef BaseActive<T> AT;
  T y;
  AT af, bf, cf, yf;
  ReverseAD::trace_on<T>();
  af <<= a;
  bf <<= b;
  cf <<= c;
  yf = af * bf;
  yf += sin(cf);
  yf *= cos(af);
  yf /= log(bf);
  yf >>= y;
  return ReverseAD::trace_off<T>();
}

void check_result(double adjoint_init_values[][DIRECTION],
                  size_t t_size,
                  size_t** t_tind,
                  double* t_values,
                  std::shared_ptr<DerivativeTensor<size_t, MultiForward<DIRECTION>>> m_tensor) {
  double** tv = new double*[N];
  double** ctv = new double*[N];
  for (size_t i=0; i<N; i++) {
    tv[i] = new double[N];
    ctv[i] = new double[N];
  }

  size_t size = 0;
  size_t** tind = nullptr;
  MultiForward<DIRECTION>* values = nullptr;
  m_tensor->get_internal_coordinate_list(0, 2, &size, &tind, &values);
/*
  for (size_t i = 0; i < size; i++) {
    std::cout << "H["<< tind[i][0]<<", "<<tind[i][1] << "] = "
              << values[i] << std::endl;
  }
*/
  double aa[N] = {0, 0, 0};
  for (size_t k = 0; k < DIRECTION; k++) {
    for (size_t i = 0; i < N; i++) {
      aa[i] = adjoint_init_values[i][k];
    }
    symmetric_third_vector(N, t_size, t_tind, t_values, aa, ctv);
    for (size_t i = 0; i < N; i++) {
      for (size_t j = 0; j < N; j++) {
        tv[i][j] = 0.0;
      }
    }    
    for (size_t i = 0;i < size; i++) {
      tv[tind[i][0]][tind[i][1]] = values[i].getDer(k);
    }
    for (size_t i = 0; i < N; i++) {
      for (size_t j = 0; j < N; j++) {
        if (fabs(tv[i][j] - ctv[i][j]) > myEps) {
          std::cout << "Multi Forward over reverse error!" << std::endl;
          exit(-1);
        }
      }
    }
  }
  for (size_t i = 0; i < N; i++) {
    delete[] tv[i];
    delete[] ctv[i];
  }
  delete[] tv;
  delete[] ctv;
  std::cout << "Multi Forward over reverse OK!" << std::endl;
}

void check_forward_over_second(
    std::shared_ptr<TrivialTrace<double>> trace,
    size_t ind_num, size_t dep_num,
    double ind_init_values[],
    double adjoint_init_values[][5]) {
  BaseReverseThird<double> third(trace);
  std::shared_ptr<DerivativeTensor<size_t, double>> tensor =
      third.compute(ind_num, dep_num);
  size_t t_size = 0;
  size_t** t_tind = nullptr;
  double* t_value = nullptr;
  tensor->get_internal_coordinate_list(0, 3, &t_size, &t_tind, &t_value);

  void* raw_memory = ::operator new[] (ind_num * sizeof(MultiForward<5>));
  MultiForward<5>* x = static_cast<MultiForward<DIRECTION>*>(raw_memory);
  for (size_t i = 0; i < ind_num; i++) {
    new(&x[i]) MultiForward<DIRECTION>(ind_init_values[i], adjoint_init_values[i]);
  }
  std::shared_ptr<TrivialTrace<MultiForward<DIRECTION>>> new_trace = 
      BaseFunctionReplay::replay_forward<double, MultiForward<5>>(
          trace, x, ind_num);

  std::cout << "BaseReverseHessian : ";
  BaseReverseHessian<MultiForward<DIRECTION>> hessian(new_trace);
  std::shared_ptr<DerivativeTensor<size_t, MultiForward<DIRECTION>>> m_tensor =
      hessian.compute(ind_num, dep_num);
  check_result(adjoint_init_values, t_size, t_tind, t_value, m_tensor);

  std::cout << "BaseReverseGeneric : ";
  BaseReverseGeneric<MultiForward<DIRECTION>> generic(new_trace, 2);
  std::shared_ptr<DerivativeTensor<size_t, MultiForward<DIRECTION>>> g_tensor = generic.compute(ind_num, dep_num);
  check_result(adjoint_init_values, t_size, t_tind, t_value, g_tensor);

  std::cout << "BaseReverseTensor : ";
  BaseReverseGeneric<MultiForward<DIRECTION>> generator(new_trace, 2);
  std::shared_ptr<DerivativeTensor<size_t, MultiForward<DIRECTION>>> t_tensor = generator.compute(ind_num, dep_num);
  check_result(adjoint_init_values, t_size, t_tind, t_value, t_tensor);

}
int main() {
  std::shared_ptr<TrivialTrace<double>> trace = foo<double>(1,2,3);
  double x[3] = {1, 2, 3};
  double a[3][5] = {{10, 4, 1, 0, 0},
                    {20, 5, 0, 1, 0},
                    {30, 6, 0, 0, 1}};
  check_forward_over_second(trace, 3, 1, x, a);
  trace = bar<double>(1, 2, 3);
  check_forward_over_second(trace, 3, 1, x, a);
  return 0;
}
