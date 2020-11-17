#pragma once
#include <glm/glm.hpp>

#include "common.h"
#include "Graphics/preamble.h"

namespace LOA::Graphics {

	namespace Implementation {
		template<typename T>
		struct AttribType {
			typedef T ScalarType;
		};
		
		template<u8 SIZE>
		struct AttribSize {
			constexpr static i32 NumComponents = SIZE;
		};


		template<typename T> struct AttribInfo {};
		template<> struct AttribInfo<f64> : AttribType<f64>, AttribSize<1> {};
		template<> struct AttribInfo<f32> : AttribType<f32>, AttribSize<1> {};
		template<> struct AttribInfo<i64> : AttribType<i64>, AttribSize<1> {};
		template<> struct AttribInfo<u64> : AttribType<u64>, AttribSize<1> {};
		template<> struct AttribInfo<i32> : AttribType<i32>, AttribSize<1> {};
		template<> struct AttribInfo<u32> : AttribType<u32>, AttribSize<1> {};
		template<> struct AttribInfo<i16> : AttribType<i16>, AttribSize<1> {};
		template<> struct AttribInfo<u16> : AttribType<u16>, AttribSize<1> {};
		template<> struct AttribInfo<i8> : AttribType<i8>, AttribSize<1> {};
		template<> struct AttribInfo<u8> : AttribType<u8>, AttribSize<1> {};

		template<typename T, glm::qualifier Q> struct AttribInfo<glm::vec<1, T, Q>> : AttribType<T>, AttribSize<1> {};
		template<typename T, glm::qualifier Q> struct AttribInfo<glm::vec<2, T, Q>> : AttribType<T>, AttribSize<2> {};
		template<typename T, glm::qualifier Q> struct AttribInfo<glm::vec<3, T, Q>> : AttribType<T>, AttribSize<3> {};
		template<typename T, glm::qualifier Q> struct AttribInfo<glm::vec<4, T, Q>> : AttribType<T>, AttribSize<4> {};
	}

	template<u32 LOC, typename T>
	class Attrib {
	public:
		typename T Type;
		typename typedef Implementation::AttribInfo<T>::ScalarType ScalarType;
		constexpr static u8 NumComponents = Implementation::AttribInfo<T>::NumComponents;
		constexpr static u32 Location = LOC;

		constexpr static size_t size() {
			return sizeof(T);
		}
	};

	typedef Attrib<POSITION_ATTRIB, glm::vec3> PositionAttrib;
	typedef Attrib<TEXCOORD_ATTRIB, glm::vec2> TexcoordAttrib;
}