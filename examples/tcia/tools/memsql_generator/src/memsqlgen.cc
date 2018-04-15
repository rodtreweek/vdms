#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */

#include "jarvis.h"
#include "util.h"
#include "neighbor.h"

#include "TCIAMemSqlGen.h"

int main(int argc, char const *argv[])
{
	if (argc < 2) {
        std::cout << "Please specify the db path" << std::endl;
        exit(0);
    }

    std::string db_path(argv[1]);

	TCIAMemSqlGen memsqlcreator(db_path);

    memsqlcreator.getGraphStats();

    try{
        memsqlcreator.generateSQLTables();
    }
    catch(Jarvis::Exception e) {
         print_exception(e);
    }

	return 0;
}
