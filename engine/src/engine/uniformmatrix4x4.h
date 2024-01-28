#ifndef UNIFORMMATRIX4X4_H
#define UNIFORMMATRIX4X4_H

#include "matrix4x4.h"
#include "uniform.h"
#include <string>

class UniformMatrix4x4 : public Matrix4x4, public Uniform{
	public:
		UniformMatrix4x4();
		virtual ~UniformMatrix4x4();

		UniformMatrix4x4& operator=(const Matrix4x4& m);

		void updateUniform();

	private:
};

#endif // UNIFORMMATRIX4X4_H
