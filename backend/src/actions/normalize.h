#ifndef BACKEND_NORMALIZE_H
#define BACKEND_NORMALIZE_H


#include <boost/filesystem.hpp>
#include <iostream>
#include "../action_args.h"
#include "../action.h"

class Normalize : public Action {
public:
	static int run(const ActionArgs &action_args);
};


#endif //BACKEND_NORMALIZE_H
