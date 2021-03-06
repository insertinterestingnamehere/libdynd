//
// Copyright (C) 2011-16 DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#pragma once

#include <typeinfo>

#include <dynd/array.hpp>
#include <dynd/kernels/kernel_prefix.hpp>

namespace dynd {
namespace nd {

  /**
   * This is a helper macro for this header file. It's the memory kernel requests
   * (kernel_request_host is the only one without CUDA enabled) to appropriate
   * function qualifiers in the variadic arguments, which tell e.g. the CUDA
   * compiler to build the functions for the GPU device.
   *
   * The classes it generates are the base classes to use for defining ckernels
   * with a single and strided kernel function.
   */
  template <typename...>
  struct base_kernel;

  template <typename PrefixType, typename SelfType>
  struct base_kernel<PrefixType, SelfType> : PrefixType {
    /**
     * Returns the child kernel immediately following this one.
     */
    kernel_prefix *get_child(intptr_t offset) { return kernel_prefix::get_child(kernel_builder::aligned_size(offset)); }

    /**
     * Returns the child kernel immediately following this one.
     */
    // This needs to be a template to avoid resolving this->size while
    // SelfType is still an incomplete type, as would the case when SelfType
    // is built using a CRTP idiom subclassing from base_kernel.
    template <typename T = SelfType>
    kernel_prefix *get_child() {
      return kernel_prefix::get_child(reinterpret_cast<T *>(this)->size());
    }

    template <size_t I>
    std::enable_if_t<I == 0, kernel_prefix *> get_child() {
      return get_child();
    }

    template <size_t I>
    std::enable_if_t<(I > 0), kernel_prefix *> get_child() {
      const size_t *offsets = this->get_offsets();
      return kernel_prefix::get_child(offsets[I - 1]);
    }

    // This needs to be a template to avoid resolving sizeof(SelfType) while
    // SelfType is still an incomplete type, as would the case when SelfType
    // is built using a CRTP idiom subclassing from base_kernel.
    template <typename T = SelfType>
    constexpr size_t size() const {
      return sizeof(T);
    }

    /** Initializes just the kernel_prefix function member. */
    template <typename... ArgTypes>
    static void init(SelfType *self, kernel_request_t kernreq, ArgTypes &&... args) {
      new (self) SelfType(std::forward<ArgTypes>(args)...);

      self->destructor = SelfType::destruct;
      switch (kernreq) {
      case kernel_request_call:
        self->function = reinterpret_cast<void *>(SelfType::call_wrapper);
        break;
      case kernel_request_single:
        self->function = reinterpret_cast<void *>(SelfType::single_wrapper);
        break;
      default:
        DYND_HOST_THROW(std::invalid_argument,
                        "expr ckernel init: unrecognized ckernel request " + std::to_string(kernreq));
      }
    }

    /**
     * The ckernel destructor function, which is placed in
     * the kernel_prefix destructor.
     */
    static void destruct(kernel_prefix *self) { reinterpret_cast<SelfType *>(self)->~SelfType(); }

    void call(array *DYND_UNUSED(dst), const array *DYND_UNUSED(src)) {
      std::stringstream ss;
      ss << "void call(array *dst, const array *src) is not implemented in " << typeid(SelfType).name();
      throw std::runtime_error(ss.str());
    }

    static void call_wrapper(kernel_prefix *self, array *dst, const array *src) {
      reinterpret_cast<SelfType *>(self)->call(dst, src);
    }

    void single(char *DYND_UNUSED(dst), char *const *DYND_UNUSED(src)) {
      std::stringstream ss;
      ss << "void single(char *dst, char *const *src) is not implemented in " << typeid(SelfType).name();
      throw std::runtime_error(ss.str());
    }

    static void DYND_EMIT_LLVM(single_wrapper)(kernel_prefix *self, char *dst, char *const *src) {
      reinterpret_cast<SelfType *>(self)->single(dst, src);
    }

    static const volatile char *DYND_USED(ir);
  };

// Ignore a false positive warning from gcc 8 about unnecessary parentheses.
DYND_IGNORE_UNNECESSARY_PARENTHESES
  template <typename PrefixType, typename SelfType>
  const volatile char *DYND_USED((base_kernel<PrefixType, SelfType>::ir)) = NULL;
DYND_END_IGNORE_UNNECESSARY_PARENTHESES

  template <typename SelfType>
  struct base_kernel<SelfType> : base_kernel<kernel_prefix, SelfType> {};

} // namespace dynd::nd
} // namespace dynd
