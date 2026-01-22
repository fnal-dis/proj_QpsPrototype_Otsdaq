#ifndef _OTS_QPSDATADECODERCONSUMER_H_
#define _OTS_QPSDATADECODERCONSUMER_H_

#include "otsdaq/Configurable/Configurable.h"
#include "otsdaq/DataManager/DQMHistosConsumerBase.h"

#include "qps-otsdaq-plugins/QPSHdf5Utilities/QPSHdf5Writer.h"
#include "qps-otsdaq-plugins/QPSRootUtilities/QPSProtoDQMHistos.h"

#include <string>

namespace ots
{
class QPSDataDecoderConsumer : public DQMHistosConsumerBase, public Configurable
{
  public:
	QPSDataDecoderConsumer(std::string              supervisorApplicationUID,
	                       std::string              inputBufferUID,
	                       std::string              processorUID,
	                       const ConfigurationTree& theXDAQContextConfigTree,
	                       const std::string&       configurationPath);
	virtual ~QPSDataDecoderConsumer(void);

	void startProcessingData(std::string runNumber) override;
	void stopProcessingData(void) override;
	void readDecodeWrite(void);
	bool workLoopThread(toolbox::task::WorkLoop* workLoop) override;

	void fastRead();

  private:
	uint32_t    bitsSample_;
	uint32_t    bitsChannel_;
	uint32_t    bitsTimestamp_;
	bool        doSaveRootFile_;
	std::string rootFilePath_;
	std::string rootFilePrefix_;

	QPSProtoDQMHistos* dqmHistosMixin_;
	QPSHdf5Writer*     hdf5WriterMixin_;

	std::string*                        read_dataP_;
	std::map<std::string, std::string>* read_headerP_;

	void decode(std::string*                        read_dataP_,
	            std::map<std::string, std::string>* read_headerP_);
};
}  // namespace ots

#endif  // _OTS_QPSDATADECODERCONSUMER_H_
