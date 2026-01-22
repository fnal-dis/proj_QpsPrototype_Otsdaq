cet_make_library(LIBRARY_NAME QPSHdf5Utilities
SOURCE QPSHdf5Writer.cc
	      LIBRARIES PRIVATE
				HDF5::HDF5
				hdf5::hdf5_hl
				hdf5::hdf5_hl_cpp
)

install_headers()
install_source()
