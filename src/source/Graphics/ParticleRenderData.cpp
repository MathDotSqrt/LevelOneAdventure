#include "Graphics/ParticleRenderData.h"

#include "Graphics/Attrib.h"
#include "Graphics/GeometryBuilder.h"

using namespace LOA::Graphics;

ParticleRenderData::ParticleRenderData(int max_size) : 
	max_size(max_size), 
	quad(VBO::BufferType::ARRAY_BUFFER),
	instance_data(VBO::BufferType::ARRAY_BUFFER),
	ebo(VBO::BufferType::ELEMENT_ARRAY_BUFFER) {

	auto geometry = gen_quad(1);

	vao.bind();

	quad.bind();
	vao.addVertexAttribPtr<PositionAttrib, TexcoordAttrib>();
	quad.bufferData(geometry.getVerticies());
	quad.unbind();

	instance_data.bind();
	vao.addVertexAttribPtr<PositionSizeAttrib, ParticleColorAttrib>();
	instance_data.bufferData(sizeof(RenderData) * max_size, nullptr, VBO::BufferHint::STREAM_DRAW);
	instance_data.unbind();

	ebo.bind();
	ebo.bufferData(geometry.getIndices());
	ebo.unbind();

}

void ParticleRenderData::streamData(const std::vector<RenderData> &data) {

}