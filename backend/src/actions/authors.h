#ifndef BACKEND_AUTHORS_H
#define BACKEND_AUTHORS_H


#include <iostream>
#include "../action_args.h"
#include "../action.h"

class Authors : public Action {
public:
	static int run(const ActionArgs &action_args);
};


#endif //BACKEND_AUTHORS_H
