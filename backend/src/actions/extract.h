#ifndef BACKEND_EXTRACT_H
#define BACKEND_EXTRACT_H


#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include "../action_args.h"
#include "../action.h"

class Extract : public Action {
public:
	static int run(const ActionArgs &action_args);
};


#endif //BACKEND_EXTRACT_H
