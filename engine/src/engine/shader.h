#ifndef SHADER_H
#define SHADER_H

#include "objectlife.h"

#include <string>

class Shader : public ObjectLife{
	public:
		Shader();
		Shader(unsigned int type, const std::string& path);
		virtual ~Shader();

		unsigned int getType() const;
		unsigned int getShader() const;

		void deleteShader();
		void destroy() override;

	protected:
		unsigned int type;
		unsigned int shader = 0;

	private:
};

#endif // SHADER_H
