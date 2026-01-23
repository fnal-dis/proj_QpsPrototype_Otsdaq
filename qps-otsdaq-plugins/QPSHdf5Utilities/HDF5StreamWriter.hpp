#ifndef HDF5STREAMWRITER_HPP_
#define HDF5STREAMWRITER_HPP_

#include "H5Cpp.h"
#include "otsdaq/Macros/CoutMacros.h"

using namespace H5;

template<typename T>
struct HDF5TypeTraits;

template<typename T>
class HDF5StreamWriter
{
  public:
	HDF5StreamWriter() = default;
	~HDF5StreamWriter();

	void open(const std::string& file, const std::string& dataset, hsize_t chunk_rows);
	void append(const T& record);
	void appendBatch(const T* records, hsize_t count);
	void close();

  private:
	void flush();
	void compressOnClose();

	void ensureOpen();

	hsize_t chunk_rows_;
	hsize_t total_rows_ = 0;

	std::vector<T> buffer_;

	bool is_open_{false};

	std::string file_name_;
	std::string dataset_name_;
	H5::H5File  file_;
	H5::DataSet dataset_;
};

template<typename T>
void HDF5StreamWriter<T>::open(const std::string& file,
                               const std::string& dataset,
                               hsize_t            chunk_rows)
{
	dataset_name_ = dataset;

	chunk_rows_ = chunk_rows;
	buffer_.reserve(chunk_rows_);

	file_ = H5File(file.c_str(), H5F_ACC_TRUNC);

	hsize_t dims[1]    = {0};
	hsize_t maxdims[1] = {H5S_UNLIMITED};
	hsize_t chunk[1]   = {chunk_rows_};

	H5::DataSpace space(1, dims, maxdims);

	H5::DSetCreatPropList plist;
	plist.setChunk(1, chunk);
	plist.setShuffle();
	plist.setDeflate(6);

	H5::CompType dtype = HDF5TypeTraits<T>::type();

	dataset_ = file_.createDataSet(dataset_name_, dtype, space, plist);

	total_rows_ = 0;
}

template<typename T>
HDF5StreamWriter<T>::~HDF5StreamWriter()
{
	flush();
	compressOnClose();
}

template<typename T>
void HDF5StreamWriter<T>::append(const T& record)
{
	buffer_.push_back(record);
	if(buffer_.size() >= chunk_rows_)
	{
		flush();
	}
}

template<typename T>
void HDF5StreamWriter<T>::appendBatch(const T* records, hsize_t count)
{
	buffer_.insert(buffer_.end(), records, records + count);
	while(buffer_.size() >= chunk_rows_)
	{
		flush();
	}
}

template<typename T>
void HDF5StreamWriter<T>::flush()
{
	//__COUT__ << "Flushing buffer from HDF5Writer" << __E__;

	if(buffer_.empty())
		return;

	hsize_t rows       = buffer_.size();
	hsize_t new_dim[1] = {total_rows_ + rows};

	dataset_.extend(new_dim);

	H5::DataSpace filespace = dataset_.getSpace();
	hsize_t       start[1]  = {total_rows_};
	hsize_t       count[1]  = {rows};
	filespace.selectHyperslab(H5S_SELECT_SET, count, start);

	H5::DataSpace memspace(1, count);

	dataset_.write(buffer_.data(), dataset_.getDataType(), memspace, filespace);

	total_rows_ += rows;
	buffer_.clear();
}

template<typename T>
void HDF5StreamWriter<T>::close()
{
	dataset_.close();
	file_.close();
}

template<typename T>
void HDF5StreamWriter<T>::compressOnClose()
{
	if(total_rows_ == 0)
		return;

	std::vector<T> data(total_rows_);
	dataset_.read(data.data(), HDF5TypeTraits<T>::type());

	std::string tmp = dataset_name_ + "_compressed";

	hsize_t dims[1]  = {total_rows_};
	hsize_t chunk[1] = {chunk_rows_};

	H5::DataSpace space(1, dims);

	H5::DSetCreatPropList plist;
	plist.setChunk(1, chunk);
	plist.setShuffle();
	plist.setDeflate(6);

	H5::DataSet compressed =
	    file_.createDataSet(tmp, HDF5TypeTraits<T>::type(), space, plist);

	compressed.write(data.data(), HDF5TypeTraits<T>::type());

	dataset_.close();
	file_.unlink(dataset_name_);
	file_.move(tmp, dataset_name_);

	dataset_ = file_.openDataSet(dataset_name_);
}

#endif
