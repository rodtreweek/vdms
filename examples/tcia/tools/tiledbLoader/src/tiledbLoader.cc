#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "VCL.h"
#include "tiledb.h"

void old_generator()
{
	std::string root = "/home/luisremi/athena/apps/hls/images_hls/pngs/";
	std::ifstream input
		("/home/luisremi/athena/apps/hls/images_hls/filenames.txt");
	std::string tdb_dir = "/tmp/tdb_dir/hls/";

	// TileDB_CTX* tiledb_ctx;
	// tiledb_ctx_init(&tiledb_ctx, NULL);

	// // Create a workspace
	// tiledb_workspace_create(tiledb_ctx, "tdb_dir");

	// // Create a group in the worskpace
	// tiledb_group_create(tiledb_ctx, tdb_dir.c_str());

	// // Finalize context
	// tiledb_ctx_finalize(tiledb_ctx);

	std::cout << "Creating TDB database" << std::endl;

	std::string img_name;
	while (std::getline(input, img_name))
	{
		try{
			VCL::Image img(root + img_name);

			std::cout << root + img_name << std::endl;

			size_t pos = img_name.rfind('.');
			img_name = img_name.substr(0, pos);
			// img.store(VCL::TDB, tdb_dir + img_name + "tdb");
		}
		catch (VCL::Exception e) {
	        printf("[Exception] %s at %s:%d\n", e.name, e.file, e.line);
		    if (e.errno_val != 0)
		        printf("%s: %s\n", e.msg.c_str(), strerror(e.errno_val));
		    else if (!e.msg.empty())
		        printf("%s\n", e.msg.c_str());
	    }
	}

	input.close();


	// input.open("/home/luisremi/hls_images/filenames.txt");
	// std::cout << "Reading from TDB and storing at jpeg/" << std::endl;
	// while (std::getline(input, img_name))
	// {
	// 	try{
	// 		size_t pos = img_name.rfind('.');
	// 		img_name = img_name.substr(0, pos);
	// 		VCL::Image img(tdb_dir + img_name + "tdb");
	// 		std::cout << tdb_dir + img_name + "tdb" << std::endl;

	// 		img.store(VCL::JPG, "jpgs/" + img_name);
	// 	}
	// 	catch (VCL::Exception e) {
	//         printf("[Exception] %s at %s:%d\n", e.name, e.file, e.line);
	// 	    if (e.errno_val != 0)
	// 	        printf("%s: %s\n", e.msg.c_str(), strerror(e.errno_val));
	// 	    else if (!e.msg.empty())
	// 	        printf("%s\n", e.msg.c_str());
	//     }
	// }

}

int main(int argc, char const *argv[])
{
	std::ifstream input ("/mnt/ssd_400/240-files.txt");
	std::string img_name;
	std::string root = "/mnt/ssd_400/scan-35529/";

	std::string tdb_dir = "/mnt/ssd_400/tdb_database_240/hls/";
	// std::string png_dir = "/mnt/ssd_400/scan-35529-large/";

	while (std::getline(input, img_name))
	{
		try{
			VCL::Image img(root + img_name);

			std::cout << root + img_name << std::endl;

			img.store(tdb_dir + img_name, VCL::TDB);
			// img.store(png_dir + img_name, VCL::PNG);
		}
		catch (VCL::Exception e) {
	        printf("[Exception] %s at %s:%d\n", e.name, e.file, e.line);
		    if (e.errno_val != 0)
		        printf("%s: %s\n", e.msg.c_str(), strerror(e.errno_val));
		    else if (!e.msg.empty())
		        printf("%s\n", e.msg.c_str());
	    }
	}

	return 0;

}
