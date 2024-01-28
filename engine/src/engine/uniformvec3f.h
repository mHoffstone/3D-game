#ifndef UNIFORMVEC3F_H
#define UNIFORMVEC3F_H

#include "util.h"
#include "uniform.h"

class UniformVec3f : public vec3f, public Uniform{
	public:
		UniformVec3f();
		virtual ~UniformVec3f();

		UniformVec3f& operator=(const vec3f& v);

		void updateUniform();

	protected:

	private:
};

#endif // UNIFORMVEC3F_H
