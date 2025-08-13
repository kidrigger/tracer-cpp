
#ifndef _SHAPE_H
#define _SHAPE_H

#include "material/material.h"
#include "material/prelude.h"
#include "core/prelude.h"

#include <memory>

class shape : public hitable {
  std::shared_ptr<material> m;

public:
  shape(std::shared_ptr<material> m) noexcept : m(std::move(m)) {}

  inline const material *mat() const { return m.get(); }
};

#endif /* _SHAPE_H */

