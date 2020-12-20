#ifndef BACKEND_QUERY_H
#define BACKEND_QUERY_H


#include <boost/filesystem.hpp>
#include <iostream>
#include "../action_args.h"
#include "../action.h"

class Query : public Action {
public:
	static int run(const ActionArgs &action_args);
};


#endif //BACKEND_QUERY_H
