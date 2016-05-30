//
// Copyright (C) 2011-16 DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#pragma once

#include <dynd/callables/base_dispatch_callable.hpp>

namespace dynd {
namespace nd {

  class imag_dispatch_callable : public base_dispatch_callable {
    dispatcher<1, callable> m_dispatcher;

  public:
    imag_dispatch_callable(const ndt::type &tp, const dispatcher<1, callable> &dispatcher)
        : base_dispatch_callable(tp), m_dispatcher(dispatcher) {}

    const callable &specialize(const ndt::type &dst_tp, intptr_t nsrc, const ndt::type *src_tp) {
      return m_dispatcher(dst_tp, nsrc, src_tp);
    }
  };

} // namespace dynd::nd
} // namespace dynd
