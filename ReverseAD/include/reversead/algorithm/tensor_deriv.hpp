#ifndef REVERSEAD_TENSOR_DERIV_H_
#define REVERSEAD_TENSOR_DERIV_H_

#include <vector>
#include <map>
#include <memory>

#include "reversead/algorithm/algorithm_common.hpp"
#include "reversead/algorithm/tensor_index.hpp"

namespace ReverseAD {

template <typename LocType, typename Base>
class TensorDeriv {
 public:
  TensorDeriv(): _order(0) {initial_template_tensor();};
  TensorDeriv(size_t order) : _order(order) {initial_template_tensor();};
  void initial_template_tensor() {
    tensor.clear();
    for (size_t i = 0; i <= _order; i++) {
      switch (i) {
        case 0:
          //tensor.push_back(new SymmetryTensor<LocType, Base, 0>());
          tensor.push_back(
              std::make_shared<SymmetryTensor<LocType, Base, 0>>());
          break;
        case 1:
          tensor.push_back(
              std::make_shared<SymmetryTensor<LocType, Base, 1>>());
          break;
        case 2:
          tensor.push_back(
              std::make_shared<SymmetryTensor<LocType, Base, 2>>());
          break;
        case 3:
          tensor.push_back(
              std::make_shared<SymmetryTensor<LocType, Base, 3>>());
          break;
        case 4:
          tensor.push_back(
              std::make_shared<SymmetryTensor<LocType, Base, 4>>());
          break;
        case 5:
          tensor.push_back(
              std::make_shared<SymmetryTensor<LocType, Base, 5>>());
          break;
        case 6:
          tensor.push_back(
              std::make_shared<SymmetryTensor<LocType, Base, 6>>());
          break;
      }
    }
  }
  void increase(TensorIndex<LocType>& index, Base v) {
    if (IsZero(v)) {
      return;
    }
//    printf("increase <%d> : [%d, %d, %d] += %.5lf\n",
//           index.size(), index._data[0], index._data[1], index._data[2], v);
    if (index.size() > 0 && index.size() <= _order) {
      tensor[index.size()]->increase(index.get_array(), v);
    }
  }
  void get_and_erase(LocType x, TensorDeriv<LocType, Base>& td) {
    switch (_order) {
      case 6:
        {
          SymmetryTensor<LocType, Base, 5>& tensor5 =
              dynamic_cast<SymmetryTensor<LocType, Base, 5>&>(*(td.tensor[5]));
          tensor5 = (std::dynamic_pointer_cast<SymmetryTensor<LocType, Base, 6>>(tensor[6]))->get_and_erase(x);
        }
      case 5:
        {
          SymmetryTensor<LocType, Base, 4>& tensor4 =
              dynamic_cast<SymmetryTensor<LocType, Base, 4>&>(*(td.tensor[4]));
          tensor4 = (std::dynamic_pointer_cast<SymmetryTensor<LocType, Base, 5>>(tensor[5]))->get_and_erase(x);
        }
      case 4:
        {
          SymmetryTensor<LocType, Base, 3>& tensor3 =
              dynamic_cast<SymmetryTensor<LocType, Base, 3>&>(*(td.tensor[3]));
          tensor3 = (std::dynamic_pointer_cast<SymmetryTensor<LocType, Base, 4>>(tensor[4]))->get_and_erase(x);
        }
      case 3:
        {
          SymmetryTensor<LocType, Base, 2>& tensor2 =
              dynamic_cast<SymmetryTensor<LocType, Base, 2>&>(*(td.tensor[2]));
          tensor2 = (std::dynamic_pointer_cast<SymmetryTensor<LocType, Base, 3>>(tensor[3]))->get_and_erase(x);
        }
      case 2:
        {
          SymmetryTensor<LocType, Base, 1>& tensor1 =
              dynamic_cast<SymmetryTensor<LocType, Base, 1>&>(*(td.tensor[1]));
          tensor1 = (std::dynamic_pointer_cast<SymmetryTensor<LocType, Base, 2>>(tensor[2]))->get_and_erase(x);
        }
      case 1:
        {
          SymmetryTensor<LocType, Base, 0>& tensor0 =
              dynamic_cast<SymmetryTensor<LocType, Base, 0>&>(*(td.tensor[0]));
          tensor0 = (std::dynamic_pointer_cast<SymmetryTensor<LocType, Base, 1>>(tensor[1]))->get_and_erase(x);
        }
    }
    //td.tensor0 = this->tensor1.get_and_erase(x);
    //td.tensor1 = this->tensor2.get_and_erase(x);
    //td.tensor2 = this->tensor3.get_and_erase(x);
    //td.tensor3 = this->tensor4.get_and_erase(x);
  }
  void debug() {
    std::cout << "=====" << std::endl;
    for (size_t i = 0; i <= _order; i++) {
      tensor[i]->debug();
    }
    //tensor0.debug();
    //tensor1.debug();
    //tensor2.debug();
    //tensor3.debug();
    //tensor4.debug();
  }
  size_t _order;
  std::vector<std::shared_ptr<SymmetryTensorBase<LocType, Base>>> tensor;
/*
  SymmetryTensor<LocType, Base, 0> tensor0;
  SymmetryTensor<LocType, Base, 1> tensor1;
  SymmetryTensor<LocType, Base, 2> tensor2;
  SymmetryTensor<LocType, Base, 3> tensor3;
  SymmetryTensor<LocType, Base, 4> tensor4;
*/
};

} // namespace ReverseAD

#endif // REVERSEAD_GENERIC_DERIV_H_
