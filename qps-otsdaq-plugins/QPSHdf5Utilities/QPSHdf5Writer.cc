#include "QPSHdf5Writer.h"

using namespace ots;

QPSHdf5Writer::QPSHdf5Writer(float param_Scale)
    : dataset_name("/my_ints"), theWriter_(new HDF5StreamWriter<qps_sample>()), param_Scale_(param_Scale)
{
}

void QPSHdf5Writer::open(const std::string& file)
{
	__COUT__ << "Opening file from QPSHdf5Writer:" << __E__;
	__COUT__ << file << __E__;
	theWriter_->open(file, "the_data", 1024 * 1024);
	__COUT__ << "Opened file successfully" << __E__;
}

void QPSHdf5Writer::fill(std::string& buffer,
                         std::map<std::string, std::string> /*header*/)
{
	//uint8_t sequenceNumber = *(uint8_t*)&((buffer)[1]);

	//__COUT__ << "Filling from QPSHdf5Writer" << __E__;
	uint64_t* buf_addr      = (uint64_t*)&(buffer[2]);
	uint64_t  buf_size_long = (buffer.size()) / sizeof(uint64_t);

	//uint8_t chan;
	for(uint64_t* buf_ptr = buf_addr; buf_ptr < buf_addr + buf_size_long; buf_ptr++)
	{
		qps_parse_from_raw(&the_qps_sample, buf_ptr, param_Scale_);
		theWriter_->append(the_qps_sample);  // Flushes automatically
	}
}

void QPSHdf5Writer::close() { theWriter_->close(); }
