#include <cassert>

#define DYND_ABI_BUILTINS_CPP

#include "dynd/abi/builtins.h"
#include "dynd/abi/noexcept.h"

extern "C" {

// TODO: We could build a factory type constructor to generate all the
// builtin fixed-width types from their size and alignment requirements.
// That'd require that we have types that represent integers or some notion
// of dependent typing, which isn't currently present.

static void never_release(dynd_resource*) dynd_noexcept {
  assert(false); // a static resource should never be released.
}

// TODO: This is currently a part of a type's ABI. This probably should be moved
// to be a part of an interface specific to primitive types.
static dynd_size_t primitive_type_alignment(dynd_type_header *header) dynd_noexcept {
  return ((dynd_type_primitive_typemeta*)(dynd_type_metadata(header))) -> alignment;
}

static dynd_type_range empty_range(dynd_type_header*) dynd_noexcept {
  return {nullptr, nullptr};
}

struct dynd_primitive_vtable {
  dynd_type_vtable vtable;
  dynd_primitive_vtable() dynd_noexcept {
    vtable.header.refcount.resource.release = never_release;
    dynd_atomic_store(&vtable.header.refcount.refcount, dynd_size_t(1u), dynd_memory_order_relaxed);
    vtable.header.header.allocated.base_ptr = this;
    vtable.header.header.allocated.size = sizeof(dynd_primitive_vtable);
    vtable.header.header.build_interface = nullptr;
    vtable.entries.alignment = primitive_type_alignment;
    vtable.entries.parameters = empty_range;
    vtable.entries.superclasses = empty_range;
  }
  ~dynd_primitive_vtable() dynd_noexcept {
    assert(dynd_atomic_load(&header.refcount.refcount, dynd_memory_order_relaxed) == 1);
  }
};

static dynd_primitive_vtable primitive_vtable{};

struct dynd_builtin_primitive_type : dynd_type_primitive {
  dynd_builtin_primitive_type(std::size_t size, std::size_t alignment) dynd_noexcept {
    typemeta.size = size;
    typemeta.alignment = alignment;
    dynd_atomic_store(&prefix.refcount.refcount, dynd_size_t(1u), dynd_memory_order_relaxed);
    prefix.refcount.resource.release = &never_release;
    // These will likely be never used, but initialize them correctly anyway.
    prefix.header.allocated.base_ptr = this;
    prefix.header.allocated.size = sizeof(dynd_builtin_primitive_type);
    prefix.header.vtable = reinterpret_cast<dynd_type_vtable*>(&primitive_vtable);
    // TODO: Should these be generated by some kind of type constructor?
    // Currently it's not really feasible since we don't have dependent typing
    // and we'd need to pass things like alignment and size as arguments.
    prefix.header.constructor = nullptr;
  }
  ~dynd_builtin_primitive_type() dynd_noexcept {
    assert(dynd_atomic_load(&prefix.refcount.refcount, dynd_memory_order_relaxed) == 1u);
  }
};

DYND_ABI_EXPORT dynd_builtin_primitive_type dynd_types_float16{2u, 2u};
DYND_ABI_EXPORT dynd_builtin_primitive_type dynd_types_float32{4u, 4u};
DYND_ABI_EXPORT dynd_builtin_primitive_type dynd_types_float64{8u, 8u};

DYND_ABI_EXPORT dynd_builtin_primitive_type dynd_types_uint8{1u, 1u};
DYND_ABI_EXPORT dynd_builtin_primitive_type dynd_types_uint16{2u, 2u};
DYND_ABI_EXPORT dynd_builtin_primitive_type dynd_types_uint32{4u, 4u};
DYND_ABI_EXPORT dynd_builtin_primitive_type dynd_types_uint64{8u, 8u};
DYND_ABI_EXPORT dynd_builtin_primitive_type dynd_types_int8{1u, 1u};
DYND_ABI_EXPORT dynd_builtin_primitive_type dynd_types_int16{2u, 2u};
DYND_ABI_EXPORT dynd_builtin_primitive_type dynd_types_int32{4u, 4u};
DYND_ABI_EXPORT dynd_builtin_primitive_type dynd_types_int64{8u, 8u};

DYND_ABI_EXPORT dynd_builtin_primitive_type dynd_types_size_t{sizeof(dynd_size_t), alignof(dynd_size_t)};

}
