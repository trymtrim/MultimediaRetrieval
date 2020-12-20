#include "version.h"

std::string Version::version() {
    return boost::str(boost::format("%s.%s") % VERSION_MAJOR % VERSION_MINOR);
}

int Version::run(const ActionArgs &action_args) {
    std::cout << version() << std::endl;
    return 0;
}
