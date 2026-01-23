cet_make_library(LIBRARY_NAME QPSRootUtilities
SOURCE QPSProtoDQMHistos.cc
	      LIBRARIES PRIVATE
		  ROOT::Gui
			ROOT::Tree
			qps-otsdaq-plugins::QPSDefinitions
)

install_headers()
install_source()
