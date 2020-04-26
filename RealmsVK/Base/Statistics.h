#pragma once

#include <cmath>
#include <array>
#include <algorithm>

namespace rlms {
	class Statistics {
		static constexpr size_t size = 100;
		std::array<double, size> population;
		size_t head;
		double sum;
		double squared_sum;
	public:
		Statistics () : population ({0}), sum (0), squared_sum (0), head (0) {}

		void add (double&& data) {
			population[head++] = data;
			sum = 0;
			for (size_t i = 0; i < population.size(); i++) {
				sum += population[i];
				squared_sum += population[i] * population[i];
			}

			head = head % size;
		}

		void reset () {
			population = std::array<double, size> ({ 0 });
			head = 0;
			sum = 0;
			squared_sum = 0;
		}

		double count () {
			return size;
		}

		double mean () {
			return sum / size;
		}

		double max () {
			return *(std::max_element (population.begin(), population.end()));
		}

		double min () {
			return *(std::min_element (population.begin (), population.end ()));
		}

		double variance () {
			return (squared_sum / size) - (mean ()* mean());
		}

		double sD () {
			return std::sqrt (variance());
		}
	};
}