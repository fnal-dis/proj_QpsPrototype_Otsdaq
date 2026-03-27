#ifndef QPSDEFINITIONS_H_
#define QPSDEFINITIONS_H_

#include <stdint.h>

//uint16_t channel_mapping[8] = {0, 1, 2, 3, 7, 6, 5, 4};

typedef struct qps_sample
{
	uint64_t timestamp;
	uint16_t channel;
	float    data;
} qps_sample;

void qps_parse_from_raw(qps_sample* qps_data_ptr, uint64_t* word_ptr, float scale);

#endif  // QPSDEFINITIONS_H_
