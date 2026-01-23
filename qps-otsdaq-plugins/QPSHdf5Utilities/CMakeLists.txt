cet_make_library(LIBRARY_NAME QPSHdf5Utilities
SOURCE QPSHdf5Writer.cc HDF5StreamWriter.hpp
	      LIBRARIES PRIVATE
				HDF5::HDF5
				hdf5::hdf5_hl
				hdf5::hdf5_hl_cpp
				qps-otsdaq-plugins::QPSDefinitions
				otsdaq::Macros
)

install_headers()
install_source()
