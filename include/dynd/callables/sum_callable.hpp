//
// Copyright (C) 2011-16 DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#pragma once

#include <dynd/callables/default_instantiable_callable.hpp>
#include <dynd/kernels/sum_kernel.hpp>

namespace dynd {
namespace nd {

  template <type_id_t Arg0ID>
  class sum_callable : public default_instantiable_callable<sum_kernel<Arg0ID>> {
  public:
    sum_callable()
        : default_instantiable_callable<sum_kernel<Arg0ID>>(ndt::make_type<ndt::callable_type>(
              ndt::make_type<typename nd::sum_kernel<Arg0ID>::dst_type>(), {ndt::type(Arg0ID)})) {}
  };

} // namespace dynd::nd
} // namespace dynd
