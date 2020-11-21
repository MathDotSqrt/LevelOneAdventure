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
		
		T get(ID id) {
			assert(has(id));
			return packed_array[index_array[id.index].index];
		}

		template<typename T>
		ID insert(const T &item) {
			

			size_t packed_index = packed_array.size();



			if (free_length == 0) {
				u32 generation = 0;
				size_t index = index_array.size();
				index_array.push_back({packed_index, generation});

				packed_array.push_back({item, index});
				return {index, generation};
			}
			else {

				ID free_element = index_array[free_index];
				u32 next_index = free_element.index;

				free_element.index = packed_index;
				free_element.generation += 1;
				
				packed_array.push_back({ item, free_index });
				free_index = next_index;
				free_length -= 1;

				return { free_index, free_element.generation };
			}
		}

		void remove(ID id) {
			assert(has(id));

			u32 element_index = index_array[id.index];

			const auto back_element = packed_array.back();
			index_array[back_element.back_index].index = element_index;
			packed_array[element_index] = std::move(back_element);
			packed_array.pop_back();

			index_array[id.index].index = free_index;

			free_index = id.index;
			free_length += 1;
		}

		bool has(ID id) const {
			return index_array[id.index].inner_id == id.inner_id;
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