
#ifndef _SHAPE_H
#define _SHAPE_H

#include "material/prelude.h"
#include "core/prelude.h"

class shape : public hitable {
protected:
	material *m;

public:
	shape(material *m) noexcept : m(m) {}

	inline const material *mat() const { return m; }

	virtual ~shape() { delete m; }
};

#endif /* _SHAPE_H */
