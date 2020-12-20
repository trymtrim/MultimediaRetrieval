#include "actions/authors.h"
#include "actions/evaluate.h"
#include "actions/extract.h"
#include "actions/normalize.h"
#include "actions/query.h"
#include "actions/store.h"
#include "actions/version.h"
#include "database_mr.h"
#include <boost/program_options.hpp>
#include <time.h>

static bool check_append_overwrite(const ActionArgs &aargs) {
	if (aargs.append && aargs.overwrite) {
		std::cout << "Append and overwrite can not both be passed at the same time." << std::endl;
		return false;
	}

	return true;
}

static bool check_database(const ActionArgs &aargs) {
	if (aargs.database.empty()) {
		std::cout << "Required argument 'database' has not been provided." << std::endl;
		return false;
	}

	boost::filesystem::path db_abs_path = boost::filesystem::absolute(aargs.database);

	if (boost::filesystem::exists(db_abs_path)) {
		if (!boost::filesystem::is_regular_file(db_abs_path)) {
			std::cout << "Database has to be a file, not a directory." << std::endl;
			return false;
		}
	}

	if (!boost::filesystem::exists(db_abs_path)) {
		Database::create(db_abs_path.c_str());
	}

	return true;
}

static bool exactly_one_command(const boost::program_options::variables_map &vm) {
	int count = 0;
	const std::string commands[] = {"help", "version", "authors", "normalize", "extract", "store", "query", "evaluate"};
	for (const std::string &command : commands) {
		if (vm.count(command)) {
			count++;
		}
	}

	return count == 1;
}

int main(int argc, char **argv) {
	/*
	 * General comments:
	 * - Using the append flag whilst there is no existing database does not throw an error,
	 *   but acts as if no append flag has been provided.
	 * - Using the overwrite flag whilst there is nothing to overwrite does not throw an error,
	 *   but acts as if no overwrite flag has been provided.
	 * - Directories and databases are created as needed.
	 */

	/*
	 * Exit codes:
	 * 0: Everything is OK.
	 * 1: Command-specific. (Input not found, or of incorrect type (file/directory), or queried input does not exist in database)
	 * 2: Command-specific. (Cache directory/database file exists but may not be overwritten or changed, or is of incorrect type (file/directory))
	 * 3: Command-specific. (No query result found)
	 * 4: <Reserved.>
	 * 5: <Reserved.>
	 * 6: No or multiple commands provided.
	 * 7: Check append or overwrite failed.
	 * 8: Check database failed.
	 * 9: Unknown error occurred.
	 */

	// Random seed
	std::srand(time(nullptr));

	int exit_code = 9;

	ActionArgs aargs = ActionArgs();

	try {
		boost::program_options::options_description desc{"Multimedia Retrieval Backend " + Version::version()};
		desc.add_options()
				("help", "Displays this help text.")
				("version", "Displays the backend's version.")
				("authors", "Displays the backend's authors.")
				("normalize",
				 "Normalizes all files in originals directory."
				 "\nUsage:"
				 "\n./backend --normalize --database ./my_database.db [--append] [--overwrite] [--debug]")
				("extract",
				 "Extracts features from all files in originals directory."
				 "\nUsage:"
				 "\n./backend --extract --database ./my_database.db [--append] [--overwrite] [--debug]")
				("store", boost::program_options::value<std::string>(&aargs.input_file),
				 "Normalize and extract in one command."
				 "\nUsage:"
				 "\n./backend --store ./my_input_file.off --database ./my_database.db [--debug]")
				("query", boost::program_options::value<std::string>(&aargs.input_file),
				 "Query (normalize, extract and compare) an input file on a database."
				 "Prints location and name of result, if any. Prints 'No match found.' otherwise."
				 "\nUsage:"
				 "\n./backend --query ./my_input_file.off --database ./my_database.db [--debug]")
				("evaluate",
				 "Evaluates the quality of the database."
				 "\nUsage:"
				 "\n./backend --evaluate --database ./my_database.db [--debug]")
				("database", boost::program_options::value<std::string>(&aargs.database), "A database file.")
				("append", "Allows for appending to (and thus changing) the database")
				("overwrite", "Allows overwriting the cache directory/database file.")
				("debug", "Allows printing of debug info.");

		boost::program_options::variables_map vm;
		boost::program_options::store(parse_command_line(argc, argv, desc), vm);
		notify(vm);

		aargs.append = vm.count("append");
		aargs.overwrite = vm.count("overwrite");
		aargs.debug = vm.count("debug");

		if (argc == 1 || vm.count("help")) {
			std::cout << desc << '\n';
			exit_code = 0;
		} else if (!exactly_one_command(vm)) {
			std::cout << "No or multiple commands provided." << std::endl;
			std::cout << "Please provide exactly one of the following:" << std::endl;
			std::cout << "--help, --version, --authors, --normalize, --extract, --store, --query --evaluate" << std::endl;
			exit_code = 6;
		} else {
			if (vm.count("version")) {
				exit_code = Version::run(aargs);
			} else if (vm.count("authors")) {
				exit_code = Authors::run(aargs);
			} else if (!check_append_overwrite(aargs)) {
				exit_code = 7;
			} else if (!check_database(aargs)) {
				exit_code = 8;
			} else {
				Database::open(boost::filesystem::absolute(aargs.database).c_str());

				if (vm.count("normalize")) {
					exit_code = Normalize::run(aargs);
				} else if (vm.count("extract")) {
					exit_code = Extract::run(aargs);
				} else if (vm.count("store")) {
					exit_code = Store::run(aargs);
				} else if (vm.count("query")) {
					exit_code = Query::run(aargs);
				} else if (vm.count("evaluate")) {
					exit_code = Evaluate::run(aargs);
				}

				Database::close();
			}
		}
	} catch (const boost::program_options::error &ex) {
		std::cerr << ex.what() << '\n';
		exit_code = 9;
	}

	return exit_code;
}