#include "QPSHdf5Writer.h"
#include <math.h>
#include <chrono>
#include <format>
#include <string>

using namespace ots;

static const uint64_t raw_timestamp_max = (uint64_t)(pow(2.0, 37.0) - 1);

QPSHdf5Writer::QPSHdf5Writer(float param_Scale)
    : dataset_name("/my_ints")
    , theWriter_(new HDF5StreamWriter<qps_sample>())
    , param_Scale_(param_Scale)
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

		the_qps_sample.timestamp +=
		    unravel_absolute_time_delta(the_qps_sample.timestamp_raw);

		theWriter_->append(the_qps_sample);  // Flushes automatically
	}
}

void QPSHdf5Writer::writeAttributes(void)
{
	using namespace std::chrono;

	auto now = system_clock::now();
	auto s   = floor<seconds>(now);
	auto ms  = duration_cast<milliseconds>(now - s).count();

	std::string time_str = std::format("{:%Y-%m-%d %H:%M:%S}.{:03}", s, ms);

	theWriter_->attrwrite_string("time_datum", time_str);
	theWriter_->attrwrite_float("sampling_rate", 150e6);
}

uint64_t QPSHdf5Writer::unravel_absolute_time_delta(uint64_t raw_timestamp)
{
	/* Handles wrapping of raw timestamp and returns a strictly positive time delta */

	// Nominally we can just do this
	time_delta = raw_timestamp - QPSHdf5Writer::prev_timestamp;

	// If negative, we've wrapped! But the difference is not necessarily uniform
	if(time_delta < 0)
		time_delta = (raw_timestamp_max - QPSHdf5Writer::prev_timestamp) + raw_timestamp;

	QPSHdf5Writer::prev_timestamp = raw_timestamp;

	return time_delta;
}

void QPSHdf5Writer::close() { theWriter_->close(); }
