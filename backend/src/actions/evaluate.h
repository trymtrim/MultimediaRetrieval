#ifndef BACKEND_EVALUATE_H
#define BACKEND_EVALUATE_H


#include <boost/format.hpp>
#include <iostream>
#include "../action_args.h"
#include "../action.h"
#include "../config.h"

class Evaluate : public Action {
public:
	static int run(const ActionArgs &action_args);
};

#endif //BACKEND_EVALUATE_H
