#pragma once

#include <vector>

#include "common.h"
#include "assert.h"


namespace LOA::Util {
	struct ID {
		u32 index;
		u32 generation;
	};
	
	template<typename T>
	class PackedFreeList {
	public:

		T& operator[](ID id) {
			assert(has(id));
			return packed_array.at(index_array[id.index].index).data;
		}

		const T& operator[](ID id) const {
			assert(has(id));
			return packed_array.at(index_array[id.index].index).data;
		}

		//const T& operator[](ID id) const {
		//	assert(has(id));
		//	return packed_array[index_array[id.index].index];
		//}

		ID insert(const T &item) {
			return emplace(std::copy(item));
		}

		ID insert(T &&item) {
			u32 packed_index = packed_array.size();



			if (free_length == 0) {
				u32 generation = 0;
				u32 index = index_array.size();
				index_array.push_back({ packed_index, generation });

				packed_array.push_back({ std::move(item), index });
				return { index, generation };
			}
			else {

				ID& free_element = index_array[free_index];
				u32 next_index = free_element.index;

				free_element.index = packed_index;
				free_element.generation += 1;

				packed_array.push_back({ std::move(item), free_index });
				free_index = next_index;
				free_length -= 1;

				return { free_index, free_element.generation };
			}
		}

		void remove(ID id) {
			assert(has(id));

			u32 element_index = index_array[id.index].index;

			const auto back_element = packed_array.back();
			index_array[back_element.back_index].index = element_index;
			packed_array[element_index] = std::move(back_element);
			packed_array.pop_back();

			index_array[id.index].index = free_index;

			free_index = id.index;
			free_length += 1;
		}

		auto begin() {
			return packed_array.begin();
		}

		const auto begin() const {
			return packed_array.begin();
		}

		auto end() {
			return packed_array.end();
		}

		const auto end() const {
			return packed_array.end();
		}

		void clear() {
			index_array.clear();
			packed_array.clear();
			free_length = 0;
		}

		bool has(ID id) const {
			return index_array[id.index].generation == id.generation;
		}

		size_t size() const {
			return packed_array.size();
		}

	private:
		struct Data {
			T data;
			u32 back_index;
		};

		u32 free_index = 0;
		size_t free_length = 0;

		std::vector<ID> index_array;
		std::vector<Data> packed_array;
	};
}