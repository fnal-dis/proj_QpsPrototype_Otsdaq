#include "QPSDefinitions.h"
#include <math.h>
#include <cstdint>

#include "otsdaq/Macros/CoutMacros.h"

typedef union _bytearray
{
	uint64_t word;
	uint8_t  bytes[8];
} _bytearray;

void qps_parse_from_raw(qps_sample* qps_data_ptr, uint64_t* word_ptr, float scale)
{
	_bytearray the_word = *(_bytearray*)word_ptr;

	uint32_t data        = the_word.word & 0xFFFFFF;                   // First 24 bits
	uint32_t data_sext   = data | (data & 0x800000 ? 0xFF000000 : 0);  // Sign extend
	int32_t  data_signed = static_cast<int32_t>(data_sext);  // Signed conversion

	// Convert to physical voltage

	qps_data_ptr->data = 2.0 * scale * ((float)(data_signed) / pow(2.0, 24.0));

	//qps_data_ptr->data = data_signed;
	qps_data_ptr->channel = (the_word.word >> 24) & 0x7;  // Next 3 bits
	//qps_data_ptr->channel   = channel_mapping[qps_data_ptr->channel];  // Apply mapping
	qps_data_ptr->timestamp_raw = (the_word.word >> 27) & 0x1FFFFFFFFF;  // Last 37 bits

	qps_data_ptr->timestamp = 0;  // This is updated outside this function
}
