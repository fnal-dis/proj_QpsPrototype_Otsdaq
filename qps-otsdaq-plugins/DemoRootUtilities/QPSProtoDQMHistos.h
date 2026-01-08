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

  protected:
	TH1I*        sequenceNumbers_;
	TCanvas*     cAdcPlots_;
	TGraph*      dataNumbers_[8];
	TMultiGraph* multiGraph;
	float        previous_timestamp;
	float        current_timestamp;
	float        timestamp_offset;
	int          current_index[8];
	int          decimation_counter_[8];
	int          decimation_rate;
};
}  // namespace ots

#endif
