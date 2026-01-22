#include "QPSHdf5Writer.h"

using namespace ots;

QPSHdf5Writer::QPSHdf5Writer() : dataset_name("/my_ints")
{
	data    = {10, 20, 30, 40, 50};
	dims[0] = 1;
	dims[1] = {static_cast<hsize_t>(data.size())};
}

void QPSHdf5Writer::book()
{
	hid_t file_id = H5Fcreate("test_file.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	if(file_id < 0)
	{
		std::cerr << "Could not create file" << std::endl;
		return;
	}

	herr_t status = H5LTmake_dataset(
	    file_id, dataset_name.c_str(), 1, dims, H5T_NATIVE_INT, data.data());

	if(status < 0)
	{
		std::cerr << " Failed to write dataset" << std::endl;
	}

	H5Fclose(file_id);
}
