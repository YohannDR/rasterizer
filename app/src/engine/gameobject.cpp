#include "engine/gameobject.h"
#include "renderer/renderer.h"

#include <iostream>
#include "TinyObj/tiny_obj_loader.h"

GameObject::GameObject()
	: Position(0.0f), Rotation(0.0f), Scaling(1.0f), ModelMaterial(1.f, 1.f, 1.f, 1.f),
	  Hidden(false), Outlined(false), TextureId(-1)
{
}

GameObject::GameObject(const Vector3& position, const Vector3& rotation, const Vector3& scaling)
	: Position(position), Rotation(rotation), Scaling(scaling), ModelMaterial(1.f, 1.f, 1.f, 1.f),
	  Hidden(false), Outlined(false), TextureId(-1)
{
}

GameObject::GameObject(const Vector3& position, const Vector3& rotation, const Vector3& scaling,
		const char* const modelName, const Material& material, const size_t textureId)
	: Position(position), Rotation(rotation), Scaling(scaling), ModelMaterial(material),
	  Hidden(false), Outlined(false), TextureId(textureId)
{
	LoadModel(modelName);
}

void GameObject::LoadModel(const char* const name)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning;
	std::string error;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, name))
	{
		std::cerr << "TinyObj error : " << error << std::endl;
		return;
	}

	if (!warning.empty())
		std::cout << "TinyObj warning : " << warning << std::endl;

	for (const tinyobj::shape_t& shape : shapes)
	{
		for (const tinyobj::index_t& index : shape.mesh.indices)
		{
			const Vector3 position = Vector3(
				attrib.vertices[index.vertex_index * 3 + 0],
				attrib.vertices[index.vertex_index * 3 + 1],
				attrib.vertices[index.vertex_index * 3 + 2]
			);

			const Vector2 uv = Vector2(
				attrib.texcoords[index.texcoord_index * 2 + 0],
				1.0f - attrib.texcoords[index.texcoord_index * 2 + 1]
			);

			const Vector3 normal = Vector3(
				attrib.normals[index.normal_index * 3 + 0],
				attrib.normals[index.normal_index * 3 + 1],
				attrib.normals[index.normal_index * 3 + 2]
			);

			const Vector4 color = Vector4(1.0f);

			const Vertex vertex = Vertex(position, color, normal, uv);
			m_Vertices.push_back(vertex);
		}
	}
}

void GameObject::CalculateModelMatrix(Matrix4x4& model) const
{
	Matrix4x4::TRS(Position, Rotation, Scaling, model);
}

void GameObject::Render(Renderer& renderer) const
{
	if (Hidden)
		return;

	renderer.BindTexture(TextureId);
	renderer.CurrentMaterial = ModelMaterial;

	// Rotation.x = renderer.GetTime();
	CalculateModelMatrix(renderer.m_Model);

	renderer.ProcessVertices(m_Vertices);
}

void GameObject::RenderOutlined(Renderer& renderer) const
{
	if (Hidden)
		return;

	renderer.BindTexture(TextureId);
	renderer.CurrentMaterial = ModelMaterial;
	CalculateModelMatrix(renderer.m_Model);

	// Draw and write to stencil buffer
	renderer.SetStencilState(true, StencilOp::WRITE);
	renderer.ProcessVertices(m_Vertices);

	renderer.BindTexture(-1);
	renderer.SetStencilState(StencilOp::DISCARD);

	Matrix4x4::TRS(Position, Rotation, Scaling * 1.05f, renderer.m_Model);
	renderer.ProcessVertices(m_Vertices);

	renderer.SetStencilState(false);
}
