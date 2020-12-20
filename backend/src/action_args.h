#ifndef BACKEND_ACTION_ARGS_H
#define BACKEND_ACTION_ARGS_H


#include <string>

class ActionArgs {
public:
	std::string input_file;
	std::string database;
	bool append;
	bool overwrite;
	bool debug;
};


#endif //BACKEND_ACTION_ARGS_H
