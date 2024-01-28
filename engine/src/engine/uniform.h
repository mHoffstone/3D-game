#ifndef UNIFORM_H
#define UNIFORM_H

#include <string>

#include "program.h"

class Uniform{
	public:
		Uniform();
		virtual ~Uniform();

		void setLocation(const Program& program, const std::string& name);
		int getLocation();

	protected:
		int location = -1;

	private:
};

#endif // UNIFORM_H
