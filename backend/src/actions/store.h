#ifndef BACKEND_STORE_H
#define BACKEND_STORE_H


#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include "../action_args.h"
#include "../action.h"

class Store : public Action {
public:
    static int run(const ActionArgs &action_args);
};


#endif //BACKEND_STORE_H
