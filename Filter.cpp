#include "Filter.h"
#include <Arduino.h>

#include <cstdlib>
#include <cmath>
#include <cctype>
#include <algorithm>

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
void Filter::rfilter(filter_dt* input, filter_dt* output, unsigned channel_count, double* const a, int na, double* const b, int nb, int* const c, int nc) {
		filter_dt buffer[channel_count];
		filter_dt input_samples[nb][nc];
		filter_dt output_samples[na][nc];
		int input_index = 0, output_index = 0;

//		fseeko(input, 0, SEEK_END);
		const off_t sample_size = sizeof(filter_dt) * channel_count;
		const off_t byte_count = 0; //ftello(input);
		if (byte_count < 0) {Serial.printf("could not calculate file length"); exit;}
		off_t sample_count = 0; // ftello(input) / sample_size;
		while (sample_count-- > 0) {
//			if (fseeko(input, -sample_size, SEEK_CUR) < 0) {Serial.printf("could not move in input file"); exit;}
//			if (fread(buffer, sizeof(filter_dt), channel_count, input) != channel_count) {Serial.printf("could not read from input file"); exit;}
//			if (fseeko(input, -sample_size, SEEK_CUR) < 0) {Serial.printf("could not move in input file"); exit;}

			for (int i=0; i<nc; ++i) {
				input_samples[input_index][i] = buffer[c[i]];
				double value = 0.0;
				for (int r=0; r<nb; ++r) {
					int prev_index = input_index - r;
					if (prev_index < 0) prev_index += nb;
					value += b[r] * input_samples[prev_index][i];
				}
				for (int r=1; r<na; ++r) {
					int prev_index = output_index - r;
					if (prev_index < 0) prev_index += na;
					value -= a[r] * output_samples[prev_index][i];
				}
				output_samples[output_index][i] = value / a[0];
			}
//			if (fwrite(output_samples[output_index], sizeof(filter_dt), nc, output) != static_cast<size_t>(nc)) {Serial.printf("could not write to output file"); exit;}
			++input_index %= nb;
			++output_index %= na;
		}
	}
//};
