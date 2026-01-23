cet_make_library(LIBRARY_NAME QPSDefinitions
	SOURCE QPSDefinitions.cc
    LIBRARIES PRIVATE
    otsdaq::Macros
)

install_headers()
install_source()
