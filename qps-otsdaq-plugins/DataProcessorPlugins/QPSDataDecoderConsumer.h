#ifndef _OTS_QPSDATADECODERCONSUMER_H_
#define _OTS_QPSDATADECODERCONSUMER_H_

#include "otsdaq/Configurable/Configurable.h"
#include "otsdaq/DataManager/DataConsumer.h"
#include "otsdaq/DataManager/DataProducer.h"

#include <string>

namespace ots
{
class QPSDataDecoderBridge : public DataConsumer, public DataProducer, public Configurable
{
  public:
	QPSDataDecoderBridge(std::string              supervisorApplicationUID,
	                     std::string              inputBufferUID,
	                     std::string              outputBufferUID,
	                     std::string              processorUID,
	                     const ConfigurationTree& theXDAQContextConfigTree,
	                     const std::string&       configurationPath);
	virtual ~QPSDataDecoderBridge(void);

	void startProcessingData(std::string runNumber) override;
	void stopProcessingData(void) override;
	void readDecodeWrite(void);
	bool workLoopThread(toolbox::task::WorkLoop* workLoop) override;

	void fastRead();

  private:
	std::string*                        read_dataP_;
	std::map<std::string, std::string>* read_headerP_;

	std::string*                        write_dataP_;
	std::map<std::string, std::string>* write_headerP_;

	void decode(std::string*                        read_dataP_,
	            std::map<std::string, std::string>* read_headerP_,
	            std::string*                        write_dataP_,
	            std::map<std::string, std::string>* write_headerP_);
};
}  // namespace ots

#endif  // _OTS_QPSDATADECODERCONSUMER_H_
