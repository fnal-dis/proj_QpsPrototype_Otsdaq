#include "otsdaq/DataManager/DataConsumer.h"
#include "otsdaq/DataManager/DataProducer.h"
#include "qps-otsdaq-plugins/DataProcessorPlugins/QPSDataDecoderConsumer.h"

using namespace ots;

QPSDataDecoderBridge::QPSDataDecoderBridge(
    std::string              supervisorApplicationUID,
    std::string              inputBufferUID,
    std::string              outputBufferUID,
    std::string              processorUID,
    const ConfigurationTree& theXDAQContextConfigTree,
    const std::string&       configurationPath)
    : WorkLoop(processorUID)
    , DataConsumer(
          supervisorApplicationUID, inputBufferUID, processorUID, LowConsumerPriority)
    , DataProducer(supervisorApplicationUID, outputBufferUID, processorUID)
    , Configurable(theXDAQContextConfigTree, configurationPath)
{
	;
}

void QPSDataDecoderBridge::startProcessingData(std::string runNumber)
{
	// Open file
	DataConsumer::startProcessingData(runNumber);
	DataProducer::startProcessingData(runNumber);
}

void QPSDataDecoderBridge::stopProcessingData(void)
{
	DataConsumer::stopProcessingData();
	DataProducer::stopProcessingData();
}

void QPSDataDecoderBridge::decode(std::string*                        read_dataP_,
                                  std::map<std::string, std::string>* read_headerP_,
                                  std::string*                        write_dataP_,
                                  std::map<std::string, std::string>* write_headerP_)
{
	// TODO: Read data from read_ buffer, decode, and write to write_ buffer

	uint64_t timestamp;
	uint16_t channel;
	int32_t  sample;
}

void QPSDataDecoderBridge::readDecodeWrite(void)
{
	if(DataConsumer::read(read_dataP_, read_headerP_) < 0)
	{
		usleep(1000);
	}

	decode(read_dataP_, read_headerP_, write_dataP_, write_headerP_);

	while(DataProducer::write(write_dataP_, write_headerP_) < 0)
	{
		usleep(10000);
	}
}

bool QPSDataDecoderBridge::workLoopThread(toolbox::task::WorkLoop*)
{
	readDecodeWrite();
	return WorkLoop::continueWorkLoop_;
}
