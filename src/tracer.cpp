#include "tracer.h"

#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>
#include <cxxabi.h>
#include <iostream>

struct stack_t stack;

static int initialized = 0;
static const int size = 256;

extern "C"{
	void __cyg_profile_func_enter(void *this_fn, void *call_site) __attribute__((no_instrument_function));
	void __cyg_profile_func_exit(void *this_fn, void *call_site) __attribute__((no_instrument_function));

	void __cyg_profile_func_enter(void *this_fn, void *call_site){
		if(initialized == 0){
			stack.elements = (struct stack_element_t*)malloc(sizeof(struct stack_element_t) * size);
			stack.ptr = -1;
			initialized = 1;
		}

		stack.ptr++;
		if(stack.ptr >= size || stack.ptr < 0){
			//printf("%d\n", stack.ptr);
		}
		if(stack.ptr < 0 || stack.ptr >= size) stack.ptr = 0;
		stack.elements[stack.ptr].fn = this_fn;
		stack.elements[stack.ptr].site = call_site;
	}
	void __cyg_profile_func_exit(void *this_fn, void *call_site){
		int origSize = stack.ptr;
		while(stack.ptr > 0){
			if(this_fn == stack.elements[stack.ptr].fn){
				stack.ptr--;
				return;
			}
			stack.ptr--;
		}
		stack.ptr = origSize;
	}
}

void printStack(){
	for(int j = 0; j <= stack.ptr; j++){
		Dl_info info;
		if(!dladdr(stack.elements[j].fn, &info)){
			info.dli_fname = "?";
			info.dli_sname = "?";
		}
		if(!info.dli_fname) {
			info.dli_fname = "?";
		}
		if(!info.dli_sname) {
			info.dli_sname = "?";
		}

		size_t length;
		int status;
		char* name = abi::__cxa_demangle(info.dli_sname, nullptr, &length, &status);

		std::cout << info.dli_fname << ": ";
		if(status == 0){
			std::cout << name;
			std::free(name);
		}
		else std::cout << info.dli_sname;
		std::cout << std::endl;
	}
}
