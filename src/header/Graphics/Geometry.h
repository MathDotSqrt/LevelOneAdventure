#pragma once

#include <vector>
#include "common.h"
#include <tuple>
#include "Graphics/Attrib.h"

namespace LOA::Graphics {
	template<typename ...T>
	class Geometry {
	private:
		template<typename ...V>
		struct Vertex {};

		template<typename V, typename ...REST>
		struct alignas(4) Vertex<V, REST...> {
			Vertex() {};

			Vertex(
				typename const V::Type &first,
				typename const REST::Type &...rest) 
				: first(first), rest(rest...) {}

			typename V::Type first;
			Vertex<REST...> rest;
		};

		template<typename V>
		struct alignas(4) Vertex<V> {
			Vertex() {}
			Vertex(typename const V::Type& last) : last(last) {}

			typename V::Type last;
		};

		template <typename T, typename Tuple>
		struct has_type;

		template <typename T, typename... Us>
		struct has_type<T, std::tuple<Us...>> : std::disjunction<std::is_same<T, Us>...> {};

	public:
		typedef Vertex<T...> GeometryVertex;
		using PositionType = typename std::tuple_element<0, std::tuple<T...> >::type::Type;
		

		inline void pushVertex(typename const T::Type &...vertexData) {
			verticies.push_back(GeometryVertex(vertexData...));
			
			PositionType pos = std::get<0>(std::make_tuple(vertexData...));
			max = glm::max(pos, max);
			min = glm::min(pos, min);
		}

		inline void pushVertex(const GeometryVertex& vertex) {
			verticies.push_back(vertex);

			PositionType pos = vertex.first;
			max = glm::max(pos, max);
			min = glm::min(pos, min);
		}

		inline void pushTriangle(u32 v0, u32 v1, u32 v2) {
			indices.push_back(v0);
			indices.push_back(v1);
			indices.push_back(v2);
		}

		inline void resizeVerticies(size_t newSize) {
			this->verticies.reserve(newSize);
		}

		inline void resizeIndices(size_t newSize) {
			this->indices.reserve(newSize);
		}

		inline void clear() {
			verticies.clear();
			indices.clear();
		}

		inline const std::vector<GeometryVertex>& getVerticies() const {
			return verticies;
		}

		inline const std::vector<unsigned int>& getIndices() const {
			return indices;
		}

		inline void setIndices(const std::vector<u32>& newIndices) {
			indices = newIndices;
		}

		inline size_t getVertexCount() const {
			return verticies.size();
		}

		inline size_t getIndexCount() const {
			return indices.size();
		}

		inline constexpr size_t sizeofVertex() const {
			return sizeof(GeometryVertex);
		}

		PositionType getMin() const {
			return min;
		}

		PositionType getMax() const {
			return max;
		}

	private:
		std::vector<GeometryVertex> verticies;
		std::vector<u32> indices;
		PositionType min = PositionType(std::numeric_limits<PositionType::value_type>::max());
		PositionType max = PositionType(std::numeric_limits<PositionType::value_type>::lowest());
	};
}