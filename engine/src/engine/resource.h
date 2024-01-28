#ifndef RESOURCE_H
#define RESOURCE_H

struct obj_file{
	const char* data;
	size_t size;
};

#define LOAD_RESOURCE(var, name) {                                                                              \
	extern const char _binary_ ## name ## _start[];                                                             \
	extern const char _binary_ ## name ## _end[];                                                               \
	var = obj_file{_binary_ ## name ## _start, (size_t)(_binary_ ## name ## _end - _binary_ ## name ## _start)};\
}

#define LOAD_RESOURCE_STRING(var, name) {\
	obj_file f;                          \
	LOAD_RESOURCE(f, name);              \
	var = std::string(f.data, f.size);   \
}

#endif
