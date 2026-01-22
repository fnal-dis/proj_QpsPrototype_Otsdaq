
include(otsdaq::dataProcessor)
cet_build_plugin(QPSDataDecoderConsumer otsdaq::dataProcessor LIBRARIES REG
	qps-otsdaq-plugins::QPSRootUtilities
	qps-otsdaq-plugins::QPSHdf5Utilities
  otsdaq::RootUtilities
  )
install_headers()
install_source()
