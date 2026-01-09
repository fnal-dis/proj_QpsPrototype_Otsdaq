#include "otsdaq/Macros/CoutMacros.h"
#include "otsdaq/Macros/ProcessorPluginMacros.h"
#include "qps-otsdaq-plugins/DataProcessorPlugins/QPSDataDecoderConsumer.h"

using namespace ots;

QPSDataDecoderConsumer::QPSDataDecoderConsumer(
    std::string              supervisorApplicationUID,
    std::string              inputBufferUID,
    std::string              processorUID,
    const ConfigurationTree& theXDAQContextConfigTree,
    const std::string&       configurationPath)
    : WorkLoop(processorUID)
    , DataConsumer(
          supervisorApplicationUID, inputBufferUID, processorUID, LowConsumerPriority)
    , Configurable(theXDAQContextConfigTree, configurationPath)
{
	;
}

QPSDataDecoderConsumer::~QPSDataDecoderConsumer(void) { ; }

void QPSDataDecoderConsumer::startProcessingData(std::string runNumber)
{
	// Open file
	DataConsumer::startProcessingData(runNumber);
}

void QPSDataDecoderConsumer::stopProcessingData(void)
{
	DataConsumer::stopProcessingData();
}

void QPSDataDecoderConsumer::decode(std::string* read_dataP_,
                                    std::map<std::string, std::string>* /*read_headerP_*/)
{
	// TODO: Read data from read_ buffer, decode, and write to write_ buffer

	uint64_t packet_raw = *((uint64_t*)&((read_dataP_)[2]));

	uint64_t timestamp = (packet_raw >> 27) & 0x7fffff;
	uint16_t channel   = (packet_raw >> 24) & 0x7;

	uint32_t sample_raw  = packet_raw & 0xffffff;
	uint32_t sample_sext = (sample_raw | (sample_raw & 0x8000000 ? 0xff000000 : 0));
	int32_t  sample      = static_cast<int32_t>(sample_sext);

	__COUT__ << timestamp << __E__;
	__COUT__ << channel << __E__;
	__COUT__ << sample << __E__;
}

void QPSDataDecoderConsumer::readDecodeWrite(void)
{
	if(DataConsumer::read(read_dataP_, read_headerP_) < 0)
	{
		usleep(1000);
		return;
	}

	decode(read_dataP_, read_headerP_);
}

bool QPSDataDecoderConsumer::workLoopThread(toolbox::task::WorkLoop*)
{
	readDecodeWrite();
	return WorkLoop::continueWorkLoop_;
}

DEFINE_OTS_PROCESSOR(QPSDataDecoderConsumer)
