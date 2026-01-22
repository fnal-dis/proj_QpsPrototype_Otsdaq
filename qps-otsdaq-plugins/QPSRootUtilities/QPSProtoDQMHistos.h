#ifndef _ots_QPSProtoDQMHistos_h_
#define _ots_QPSProtoDQMHistos_h_

#include <stdint.h>
#include <map>
#include <queue>
#include <string>

class TH1I;
class TGraph;
class TMultiGraph;
class TFile;
class TCanvas;
class TTree;

namespace ots
{
class QPSProtoDQMHistos
{
  public:
	QPSProtoDQMHistos(void);
	virtual ~QPSProtoDQMHistos(void);
	void book(TFile* rootFile);
	void fill(std::string& buffer, std::map<std::string, std::string> header);
	void load(std::string fileName);
	void fillTree(std::string& buffer, std::map<std::string, std::string> header);

  protected:
	TH1I*    sequenceNumbers_;
	TCanvas* cAdcPlots_;

	TCanvas* cAdcCanvasPerChannel_[8];

	TTree*       theTree;
	TGraph*      dataNumbers_[8];
	TMultiGraph* multiGraph;
	uint64_t     previous_timestamp;
	uint64_t     current_timestamp;
	uint64_t     timestamp_offset;
	double       timestamp_seconds;
	int          current_index[8];
	int          decimation_counter_[8];
	int          decimation_rate;
};
}  // namespace ots

#endif
