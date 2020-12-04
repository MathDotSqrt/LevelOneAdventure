#pragma once

#include <string>
#include <vector>

#include "common.h"

namespace LOA::Graphics {
	class TEX {
	public:
		TEX(TEX&&);
		~TEX();
		TEX(const TEX&) = delete;
		
		TEX& operator=(const TEX&) = delete;
		TEX& operator=(TEX&& other);

		void bind();
		void bindActiveTexture(int unit);
		void unbind();

		GLuint getTexID() const;
		int getWidth() const;
		int getHeight() const;

		class Builder {
		private:
			friend class TEX;
			int width, height;

			GLuint texID;
			GLenum textureTarget;

			GLenum storage;
			GLenum components;
			GLenum dataType;

			GLenum wrapS;
			GLenum wrapT;

			float bgColor[4];

			GLenum filter;
			GLenum mipmap;
			bool useMipMap;

		public:
			Builder();

			Builder& a();
			Builder& r();
			Builder& rgb();
			Builder& rgba();
			Builder& depth24();
			Builder& unsignedByteType();
			Builder& floatType();

			Builder& repeat();
			Builder& mirrorRepeat();
			Builder& clampToEdge();
			Builder& clampToBorder();
			Builder& borderColor(float r, float g, float b, float a);
			Builder& nearest();
			Builder& linear();
			Builder& mipmapNearest();
			Builder& mipmapLinear();

			TEX buildTexture(std::string filename);
			TEX buildTexture3D(const std::vector<float> &buffer);
		};

	private:
		GLuint texID;
		GLenum textureTarget;
		int width, height;

		TEX(Builder& builder);

		void dispose();
	};
}