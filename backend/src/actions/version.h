#ifndef BACKEND_VERSION_H
#define BACKEND_VERSION_H


#include <boost/format.hpp>
#include <iostream>
#include "../action_args.h"
#include "../action.h"
#include "../config.h"

class Version : public Action {
public:
	static std::string version();

	static int run(const ActionArgs &action_args);
};

#endif //BACKEND_VERSION_H
