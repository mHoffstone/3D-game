#ifndef PROGRAM_H
#define PROGRAM_H

#include "shader.h"

#include "objectlife.h"

#include <memory>

class Program : public ObjectLife{
	public:
		Program();
		virtual ~Program();

		void createProgram();
		unsigned int getProgram() const;

		bool setShaders(std::initializer_list<Shader*> shaders);

		void useProgram() const;

		void deleteProgram();
		void destroy() override;

	protected:
		unsigned int program = 0;

	private:
};

#endif // PROGRAM_H
