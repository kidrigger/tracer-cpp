
#ifndef _SHAPE_H
#define _SHAPE_H

#include "hitable.h"
#include "material/material.h"

class shape : public hitable {
protected:
	material *m;

public:
	shape(material *m) :
			m(m) {}
	virtual const material *mat() const { return m; }
	virtual ~shape() { delete m; }
};

#endif /* _SHAPE_H */
