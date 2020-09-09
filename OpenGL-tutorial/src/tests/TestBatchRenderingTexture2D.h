#pragma once
#include "Test.h"

#include "Texture.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

#include <memory>

namespace test {

	class TestBatchRenderingTexture2D : public Test
	{
	public:
		TestBatchRenderingTexture2D();
		~TestBatchRenderingTexture2D();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture1;
		std::unique_ptr<Texture> m_Texture2;

		glm::mat4 m_Proj;
		glm::mat4 m_View;
	};

}