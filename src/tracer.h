#ifndef TRACER_H
#define TRACER_H

#ifdef __cplusplus
extern "C" {
#endif

struct stack_element_t{
	void* fn;
	void* site;
};

struct stack_t{
	struct stack_element_t* elements;
	int ptr;
};

extern struct stack_t stack;

#ifdef __cplusplus
}
#endif

void printStack();

#endif
