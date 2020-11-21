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
		

		template<typename T>
		ID insert(const T &item) {
			

			size_t packed_index = packed_array.size();
			packed_array.push_back(item);

			if (free_length == 0) {
				index_array.push_back(IndexElement{ ID{packed_index, 0},  packed_index });
			}
			else {
				ID free_element = index_array[free_index];
				free_index = free_element.index;
				free_length -= 1;

				free_element.generation += 1;
				free_element.index = packed_index;
			}
		}

		void remove(ID id) {
			assert(has(id));

			size_t packed_index = id.index;
			index_array

		}

		bool has(ID id) const {
			return index_array[id.index].inner_id == id.inner_id;
		}

		size_t size() const {
			return packed_array.size();
		}

	private:
		struct IndexElement {
			size_t index;
			u32 generation;
		};

		u32 free_index = 0;
		size_t free_length = 0;

		std::vector<IndexElement> index_array;
		std::vector<T> packed_array;
	};
}