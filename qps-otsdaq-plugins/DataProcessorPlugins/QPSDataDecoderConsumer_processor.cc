#include "otsdaq/Macros/CoutMacros.h"
#include "otsdaq/Macros/ProcessorPluginMacros.h"
#include "qps-otsdaq-plugins/DataProcessorPlugins/QPSDataDecoderConsumer.h"

using namespace ots;

QPSDataDecoderConsumer::QPSDataDecoderConsumer(
    std::string              supervisorApplicationUID,
    std::string              bufferUID,
    std::string              processorUID,
    const ConfigurationTree& theXDAQContextConfigTree,
    const std::string&       configurationPath)
    : WorkLoop(processorUID)
    , DQMHistosConsumerBase(
          supervisorApplicationUID, bufferUID, processorUID, HighConsumerPriority)
    , Configurable(theXDAQContextConfigTree, configurationPath)
    , bitsSample_(theXDAQContextConfigTree.getNode(configurationPath)
                      .getNode("SampleBits")
                      .getValue<uint32_t>())
    , bitsChannel_(theXDAQContextConfigTree.getNode(configurationPath)
                       .getNode("ChannelBits")
                       .getValue<uint32_t>())
    , bitsTimestamp_(theXDAQContextConfigTree.getNode(configurationPath)
                         .getNode("TimestampBits")
                         .getValue<uint32_t>())
    , doSaveRootFile_(theXDAQContextConfigTree.getNode(configurationPath)
                          .getNode("DoSave")
                          .getValue<bool>())
    , rootFilePath_(theXDAQContextConfigTree.getNode(configurationPath)
                        .getNode("RootFilePath")
                        .getValue<std::string>())
    , rootFilePrefix_(theXDAQContextConfigTree.getNode(configurationPath)
                          .getNode("RootFilePrefix")
                          .getValue<std::string>())
    , dqmHistosMixin_(new QPSProtoDQMHistos())
    , hdf5WriterMixin_(new QPSHdf5Writer())
{
	__COUT__ << "Initializing QPSDataDecoderConsumer" << __E__;
	__COUT__ << bitsSample_ << __E__;
	__COUT__ << bitsTimestamp_ << __E__;
	__COUT__ << bitsChannel_ << __E__;
}

QPSDataDecoderConsumer::~QPSDataDecoderConsumer(void) { ; }

void QPSDataDecoderConsumer::startProcessingData(std::string runNumber)
{
	// Open file
	DQMHistosBase::openFile(rootFilePath_ + "/" + rootFilePrefix_ + "_Run" + runNumber +
	                        ".root");
	dqmHistosMixin_->book(DQMHistosBase::theFile_);
	hdf5WriterMixin_->open(rootFilePath_ + "/" + rootFilePrefix_ + "_Run" + runNumber +
	                       ".h5");
	DataConsumer::startProcessingData(runNumber);
}

void QPSDataDecoderConsumer::stopProcessingData(void)
{
	DataConsumer::stopProcessingData();
	if(doSaveRootFile_)
	{
		save();
	}
	closeFile();
	hdf5WriterMixin_->close();
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

	// decode(read_dataP_, read_headerP_);
	//dqmHistosMixin_->fill(*read_dataP_, *read_headerP_);
	dqmHistosMixin_->fillTree(*read_dataP_, *read_headerP_);
	hdf5WriterMixin_->fill(*read_dataP_, *read_headerP_);

	DataConsumer::setReadSubBuffer<std::string, std::map<std::string, std::string>>();
}

bool QPSDataDecoderConsumer::workLoopThread(toolbox::task::WorkLoop*)
{
	readDecodeWrite();
	return WorkLoop::continueWorkLoop_;
}

DEFINE_OTS_PROCESSOR(QPSDataDecoderConsumer)
