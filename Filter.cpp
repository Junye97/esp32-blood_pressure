#include "Filter.h"
#include <Arduino.h>

#include <cstdlib>
#include <cmath>
#include <cctype>
#include <algorithm>

#define TEST_RW

template<typename T>
void array_from_list(T*& array, int& size, const std::list<double>& list) {
	size = list.size();
	array = new T[size];
	std::copy(list.begin(), list.end(), array);
}

// template<typename T>
// class Filter {
// public:
//  virtual void rfilter(T* input, T* output, unsigned channel_count, double* const a, int na, double* const b, int nb, int* const c, int nc);
// };

//template<typename T>
//class FilterImpl : public Filter {
//public:
void Filter::rfilter(filter_io* input, filter_io* output, unsigned channel_count, filter_dt* const a, int na, filter_dt* const b, int nb, int* const c, int nc) {
		// filter_dt buffer[channel_count*sizeof(filter_dt)];
		filter_dt input_samples[nb][nc];
		filter_dt output_samples[na][nc];
    memset(input_samples, 0, sizeof(filter_dt)*nb*nc);
    memset(output_samples, 0, sizeof(filter_dt)*na*nc);
    
		int input_index = 0, output_index = 0;

		union {
			uint8_t byte_buffer[sizeof(filter_dt)];
			filter_dt actual_input;
		} buffer;

		// fseeko(input, 0, SEEK_END);
		const off_t sample_size = sizeof(filter_dt) * channel_count;
		// const off_t byte_count = ftello(input); //0;
		// if (byte_count < 0) {Serial.printf("could not calculate file length\n"); exit;}
		off_t sample_count = input->size() / sample_size; // 0; // number of input samples
    size_t output_write_size = channel_count*sizeof(filter_dt);

    filter_dt value = 0.0;
		while (sample_count-- > 0) {
			// if (!input.seek(input, -sample_size, SEEK_CUR)) {Serial.printf("could not move in input file\n"); exit;}
			if (input->read(buffer.byte_buffer, sizeof(filter_dt)) == -1) {Serial.printf("could not read from input file\n"); exit;}
			// if (!input.seek(input, -sample_size, SEEK_CUR)) {Serial.printf("could not move in input file\n"); exit;}
//			memcpy(&buffer.actual_input, &buffer.byte_buffer, sizeof(filter_dt));
#ifndef TEST_RW
			for (int i=0; i<nc; ++i) {
				input_samples[input_index][i] = *((filter_dt*) (&buffer.byte_buffer));
				value = 0.0;
				for (int r=0; r<nb; ++r) {
					int prev_index = input_index - r;
					if (prev_index < 0) prev_index += nb; // zero-padding boundary extension
					value += b[r] * input_samples[prev_index][i];
				}
				for (int r=1; r<na; ++r) {
					int prev_index = output_index - r;
					if (prev_index < 0) prev_index += na;
					value -= a[r] * output_samples[prev_index][i];
				}
				output_samples[output_index][i] = value / a[0];
			}

			// Hardcode the output to single channel
//			memcpy(&buffer.byte_buffer, &output_samples[output_index][0], sizeof(filter_dt));
#endif
			if (output->write((uint8_t*)(output_samples[output_index]), output_write_size) != output_write_size) {Serial.printf("could not write to output file\n"); exit;}
      if (sample_count % 500 == 0){
        Serial.printf("%d samples remaining, current input = %f, output = %f, value = %f\n", sample_count, input_samples[input_index][0], output_samples[output_index][0], value);
      }
			++input_index %= nb;
			++output_index %= na;
		}
	}
//};

// static uint_8* dt_to_byte(filter_dt input) {
// 	union {
// 		uint8_t byte_val[sizeof(input)];
// 		filter_dt input;
// 	} data_union;

// 	return byte_val;
// }
