#include "otsdaq-demo/DemoRootUtilities/QPSProtoDQMHistos.h"

#include <stdint.h>
#include <iostream>
#include <sstream>
#include <string>

#include <TCanvas.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1I.h>
#include <TMultiGraph.h>

Int_t PLOT_MAX_LIMIT = 10000;

using namespace ots;

std::map<uint8_t, uint8_t> channel_mapping;

//==============================================================================
QPSProtoDQMHistos::QPSProtoDQMHistos(void)
{
	decimation_rate    = 10;
	previous_timestamp = 0;
	current_timestamp  = 0;
	timestamp_offset   = 0;

	// Fix SMA port to ADC channel mapping
	channel_mapping[2] = 0;
	channel_mapping[3] = 1;
	channel_mapping[0] = 2;
	channel_mapping[1] = 3;
	channel_mapping[5] = 4;
	channel_mapping[4] = 5;
	channel_mapping[7] = 6;
	channel_mapping[6] = 7;
}

//==============================================================================
QPSProtoDQMHistos::~QPSProtoDQMHistos(void) {}

Int_t _colors[8] = {kBlue, kRed, kGreen, kOrange, kViolet, kPink, kAzure, kMagenta};
//==============================================================================
void QPSProtoDQMHistos::book(TFile* rootFile)
{
	std::cout << "Booking start!" << std::endl;
	std::cout << "This is QPS Prototype" << std::endl;
	TDirectory* currentDir = rootFile->mkdir("General", "General");
	currentDir->cd();

	std::stringstream ss;
	sequenceNumbers_ = new TH1I("SequenceNumber", "Sequence Number", 256, 0, 255);
	cAdcPlots_       = new TCanvas("ADC Data", "ADC Data Plots", 800, 600);
	currentDir->Add(cAdcPlots_);
	//cAdcPlots_->cd();

	multiGraph = new TMultiGraph();

	for(int i = 0; i < 8; i++)
	{
		ss.str(std::string());
		ss << "AdcChannel" << i;
		dataNumbers_[i] = new TGraph(PLOT_MAX_LIMIT);
		//for(int j = 0; j < PLOT_MAX_LIMIT; j++)
		//{
		//	dataNumbers_[i]->SetPoint(j, 0.0, 0.0);
		//}
		multiGraph->Add(dataNumbers_[i]);
		dataNumbers_[i]->SetTitle(ss.str().c_str());
		dataNumbers_[i]->SetLineWidth(3);
		dataNumbers_[i]->SetLineColor(_colors[i]);
		current_index[i]       = 0;
		decimation_counter_[i] = 0;
	}

	multiGraph->SetTitle("ADC Data;Time;Sample value");
	multiGraph->GetYaxis()->SetLimits(-5, 5);
	multiGraph->Draw("ap 13d");
	cAdcPlots_->Modified();
	cAdcPlots_->Update();
	//multiGraph->GetHistogram()->GetXaxis()->SetRangeUser(0.,2.5);

	/*
    dataNumbers_[0] = new TH1I("ADC Data Channel 1", "Data", 21, 0, 0x1000000);
    dataNumbers_[0]->Fill(0);
    dataNumbers_[1] = new TH1I("ADC Data Channel 2", "Data", 21, 0, 0x1000000);
    dataNumbers_[1]->Fill(0);
    dataNumbers_[2] = new TH1I("ADC Data Channel 3", "Data", 21, 0, 0x1000000);
    dataNumbers_[2]->Fill(0);
    dataNumbers_[3] = new TH1I("ADC Data Channel 4", "Data", 21, 0, 0x1000000);
    dataNumbers_[3]->Fill(0);
    dataNumbers_[4] = new TH1I("ADC Data Channel 5", "Data", 21, 0, 0x1000000);
    dataNumbers_[4]->Fill(0);
    dataNumbers_[5] = new TH1I("ADC Data Channel 6", "Data", 21, 0, 0x1000000);
    dataNumbers_[5]->Fill(0);
    dataNumbers_[6] = new TH1I("ADC Data Channel 7", "Data", 21, 0, 0x1000000);
    dataNumbers_[6]->Fill(0);
    dataNumbers_[7] = new TH1I("ADC Data Channel 8", "Data", 21, 0, 0x1000000);
    dataNumbers_[7]->Fill(0);
    */
}

//
// |------------|----------|-----------|
// | TIMESTAMP  |  CHANNEL |  DATA     |
// |  63 : 27   |  26 : 24 |  23:0     |
// |------------|----------|-----------|

typedef union _bytearray
{
	uint64_t word;
	uint8_t  bytes[8];
} _bytearray;

typedef struct qps_data
{
	uint64_t timestamp;
	uint16_t channel;
	Double_t data;
} qps_data;

qps_data the_qps_data;

void qps_parse(qps_data* qps_data_ptr, uint64_t* word_ptr)
{
	_bytearray the_word = *(_bytearray*)word_ptr;

	/*
    the_word.bytes[0] = (*(_bytearray *)(word_ptr)).bytes[7];
    the_word.bytes[1] = (*(_bytearray *)(word_ptr)).bytes[6];
    the_word.bytes[2] = (*(_bytearray *)(word_ptr)).bytes[5];
    the_word.bytes[3] = (*(_bytearray *)(word_ptr)).bytes[4];
    the_word.bytes[4] = (*(_bytearray *)(word_ptr)).bytes[3];
    the_word.bytes[5] = (*(_bytearray *)(word_ptr)).bytes[2];
    the_word.bytes[6] = (*(_bytearray *)(word_ptr)).bytes[1];
    the_word.bytes[7] = (*(_bytearray *)(word_ptr)).bytes[0];
    */

	//std::cout << "------" << std::endl;
	//std::cout << "qps-parse orig: 0x" << std::hex << *word_ptr << std::dec << std::endl;
	//std::cout << "qps-parse revd: 0x" << std::hex << the_word.word << std::dec << std::endl;

	uint32_t data        = the_word.word & 0xFFFFFF;                   // First 24 bits
	uint32_t data_sext   = data | (data & 0x800000 ? 0xFF000000 : 0);  // Sign extend
	int32_t  data_signed = static_cast<int32_t>(data_sext);  // Signed conversion

	//std::cout << "qps-parse data       : 0x" << std::hex << data << std::dec << std::endl;
	//std::cout << "qps-parse data_sext  : 0x" << std::hex << data_sext << std::dec << std::endl;
	//std::cout << "qps-parse data_signed:   " << data_signed << std::endl;

	qps_data_ptr->data      = 5 * ((-(Double_t)(data_signed)) / pow(2.0, 24.0) +
                              0.5);                    // Convert to physical voltage
	qps_data_ptr->channel   = (the_word.word >> 24) & 0x7;  // Next 3 bits
	qps_data_ptr->channel   = channel_mapping[qps_data_ptr->channel];  // Apply mapping
	qps_data_ptr->timestamp = (the_word.word >> 27) & 0x1FFFFFFFFF;    // Last 37 bits

	//std::cout << "qps-parse timestamp: 0x" << std::hex << qps_data_ptr->timestamp << std::dec << std::endl;
	//std::cout << "qps-parse channel: 0x" << std::hex << qps_data_ptr->channel << std::dec << std::endl;
	//std::cout << "qps-parse data: 0x" << std::hex << qps_data_ptr->data << std::dec << std::endl;
}

//==============================================================================
void QPSProtoDQMHistos::fill(std::string& buffer,
                             std::map<std::string, std::string> /*header*/)
{
	//std::stringstream  ss;
	//uint64_t dataQW = *((uint64_t*)&((buffer)[2]));
	/*{  // print
        ss << "dataP Read: 0x ";
        for(unsigned int i = 0; i < (buffer).size(); ++i)
            ss << std::hex << (int)(((buffer)[i] >> 4) & 0xF)
               << (int)(((buffer)[i]) & 0xF) << " " << std::dec;
        ss << std::endl;
        std::cout << "\n" << ss.str();

        std::cout << "sequence = " << (int)*((uint8_t*)&((buffer)[1])) << std::endl;

        std::cout << "dataQW = 0x" << std::hex << (dataQW) << " " << std::dec << dataQW
                  << std::endl;
    }*/

	//uint8_t header = *(uint8_t*)&((buffer)[0]);
	uint8_t sequenceNumber = *(uint8_t*)&((buffer)[1]);

	uint64_t* buf_addr      = (uint64_t*)&(buffer[2]);
	uint64_t  buf_size_long = (buffer.size()) / sizeof(uint64_t);

	uint8_t chan;

	for(uint64_t* buf_ptr = buf_addr; buf_ptr < buf_addr + buf_size_long; buf_ptr++)
	{
		// Process raw data from buffer
		qps_parse(&the_qps_data, buf_ptr);

		chan = the_qps_data.channel;

		// Calculate timestamp offset for timestamp unwrapping
		current_timestamp = (float)(the_qps_data.timestamp) * 6.666e-9;
		if(current_timestamp < previous_timestamp)
		{
			timestamp_offset += previous_timestamp;
		}
		previous_timestamp = current_timestamp;

		//std::cout << "chan: " << chan << ", idx: " << current_index[chan]
		//          << ", dec_cnt: " << decimation_counter_[chan] << std::endl;

		// Update plot data
		// Note that we add timestamp_offset to the timestamp to prevent wrap-arounds due to limited precision of the firmware timestamp
		if(decimation_counter_[chan] == 0)
		{
			dataNumbers_[chan]->SetPoint(current_index[chan]++,
			                             current_timestamp + timestamp_offset,
			                             (float)the_qps_data.data);
			current_index[chan] %= (PLOT_MAX_LIMIT);
		}

		decimation_counter_[chan] = (decimation_counter_[chan] + 1) % decimation_rate;
	}

	cAdcPlots_->Modified();
	cAdcPlots_->Update();

	sequenceNumbers_->Fill(sequenceNumber);
}

//==============================================================================
void QPSProtoDQMHistos::load(std::string /*fileName*/)
{
	/*LORE 2016 MUST BE FIXED THIS MONDAY
    DQMHistosBase::openFile (fileName);
    numberOfTriggers_ = (TH1I*)theFile_->Get("General/NumberOfTriggers");

    std::string directory = "Planes";
    std::stringstream name;
    for(unsigned int p=0; p<4; p++)
    {
        name.str("");
        name << directory << "/Plane_" << p << "_Occupancy";
        //FIXME Must organize better all histograms!!!!!
        //planeOccupancies_.push_back((TH1I*)theFile_->Get(name.str().c_str()));
    }
    //canvas_ = (TCanvas*) theFile_->Get("MainDirectory/MainCanvas");
    //histo1D_ = (TH1F*) theFile_->Get("MainDirectory/Histo1D");
    //histo2D_ = (TH2F*) theFile_->Get("MainDirectory/Histo2D");
    //profile_ = (TProfile*) theFile_->Get("MainDirectory/Profile");
    closeFile();
    */
}
