#ifndef QPSHDF5WRITER_H_
#define QPSHDF5WRITER_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "HDF5StreamWriter.hpp"
#include "otsdaq/Macros/CoutMacros.h"
#include "qps-otsdaq-plugins/QPSDefinitions/QPSDefinitions.h"

template<>
struct HDF5TypeTraits<qps_sample>
{
	static H5::CompType type()
	{
		// clang-format off
		H5::CompType t(sizeof(qps_sample));
		t.insertMember("timestamp",
                   HOFFSET(qps_sample, timestamp),
                   H5::PredType::NATIVE_UINT64);
		t.insertMember("timestamp_raw",
                   HOFFSET(qps_sample, timestamp_raw),
                   H5::PredType::NATIVE_UINT64);
		t.insertMember("channel",
                   HOFFSET(qps_sample, channel),
                   H5::PredType::NATIVE_UINT16);
		t.insertMember("data",
                   HOFFSET(qps_sample, data),
                   H5::PredType::NATIVE_FLOAT);
		// clang-format on
		return t;
	}
};

namespace ots
{

class QPSHdf5Writer
{
  public:
	QPSHdf5Writer(float param_Scale);
	void open(const std::string& file);
	void close();
	void fill(std::string& buffer, std::map<std::string, std::string> header);
	void writeAttributes(void);

  private:
	uint64_t unravel_absolute_time_delta(uint64_t raw_timestamp);

  protected:
	std::string      dataset_name;
	std::vector<int> data;
	hsize_t          dims[1];

	HDF5StreamWriter<qps_sample>* theWriter_;

	bool isFirstPacket_;

	qps_sample the_qps_sample;
	float      param_Scale_;

	uint64_t prev_timestamp = 0;
	int64_t  time_delta     = 0;

	uint64_t absolute_timestamp = 0;
	//std::vector<qps_sample> dataBuffer_;
};
}  // namespace ots

#endif  // QPSHDF5WRITER_H_
