//
// Copyright (C) 2011-15 DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#pragma once

#include <dynd/callables/default_instantiable_callable.hpp>
#include <dynd/kernels/serialize_kernel.hpp>
#include <dynd/types/callable_type.hpp>

namespace dynd {
namespace nd {

  template <type_id_t Arg0ID>
  class serialize_callable : public base_callable {
  public:
    serialize_callable() : base_callable(ndt::callable_type::make(ndt::type("bytes"), {ndt::type(Arg0ID)})) {}

    const ndt::type &resolve(call_graph &cg, const ndt::type &dst_tp, size_t DYND_UNUSED(nsrc),
                             const ndt::type *DYND_UNUSED(src_tp), size_t DYND_UNUSED(nkwd),
                             const array *DYND_UNUSED(kwds),
                             const std::map<std::string, ndt::type> &DYND_UNUSED(tp_vars)) {
      cg.emplace_back(this);
      return dst_tp;
    }

    void instantiate(char *DYND_UNUSED(data), kernel_builder *ckb, const ndt::type &DYND_UNUSED(dst_tp),
                     const char *DYND_UNUSED(dst_arrmeta), intptr_t DYND_UNUSED(nsrc), const ndt::type *src_tp,
                     const char *const *DYND_UNUSED(src_arrmeta), kernel_request_t kernreq, intptr_t DYND_UNUSED(nkwd),
                     const array *DYND_UNUSED(kwds), const std::map<std::string, ndt::type> &DYND_UNUSED(tp_vars)) {
      ckb->emplace_back<serialize_kernel<Arg0ID>>(kernreq, src_tp[0].get_data_size());
    }
  };

} // namespace dynd::nd
} // namespace dynd
