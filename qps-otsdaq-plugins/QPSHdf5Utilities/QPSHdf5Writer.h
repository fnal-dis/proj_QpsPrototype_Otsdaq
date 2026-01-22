#ifndef QPSHDF5WRITER_H_
#define QPSHDF5WRITER_H_

#include <iostream>
#include <string>
#include <vector>

#include "hdf5.h"
#include "hdf5_hl.h"

namespace ots
{
class QPSHdf5Writer
{
  public:
	QPSHdf5Writer(void);
	void book();
	void fill();

  protected:
	std::string      dataset_name;
	std::vector<int> data;
	hsize_t          dims[2];
};
}  // namespace ots

#endif  // QPSHDF5WRITER_H_
