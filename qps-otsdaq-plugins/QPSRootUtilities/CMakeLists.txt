cet_make_library(LIBRARY_NAME QPSRootUtilities
SOURCE QPSProtoDQMHistos.cc
	      LIBRARIES PRIVATE
		  ROOT::Gui
)

install_headers()
install_source()
