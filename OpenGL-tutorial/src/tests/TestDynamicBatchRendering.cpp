#include "TestDynamicBatchRendering.h"

#include "Renderer.h"

#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <array>

namespace test {

	TestDynamicBatchRendering::TestDynamicBatchRendering()
		: m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
		m_QuadPosition(0.0f, 0.0f)
	{
		unsigned int indices[] = {  // You must use an unsigned type
			0, 1, 2, 2, 3, 0,  // indices for one quad
			4, 5, 6, 6, 7, 4
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		m_VAO = std::make_unique<VertexArray>();

		m_VertexBuffer = std::make_unique<VertexBuffer>(nullptr, sizeof(Vertex) * 1000);  // Create a buffer that can contain 1000 vertices
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(4);
		layout.Push<float>(2);
		layout.Push<float>(1);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 12);

		m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
		m_Shader->Bind();
		int samplers[2] = { 0, 1 };
		m_Shader->SetUniform1iv("u_Textures", 2, samplers);

		m_Texture1 = std::make_unique<Texture>("res/textures/logo.png");
		m_Texture2 = std::make_unique<Texture>("res/textures/fire.png");
	}


	TestDynamicBatchRendering::~TestDynamicBatchRendering()
	{
	}

	void TestDynamicBatchRendering::OnUpdate(float deltaTime)
	{
	}

	void TestDynamicBatchRendering::OnRender()
	{
		auto q0 = CreateQuad(m_QuadPosition[0], m_QuadPosition[1], 0.0f);
		auto q1 = CreateQuad( 400.0f, 200.0f, 1.0f);

		Vertex vertices[8];
		memcpy(vertices, q0.data(), q0.size() * sizeof(Vertex));
		memcpy(vertices + q0.size(), q1.data(), q1.size() * sizeof(Vertex));

		// Set the dynamic vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer->GetID());
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);  // every frame dynamically populate the vertex buffer

		GLCall(glClearColor(1.0f, 1.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer renderer;

		m_Texture1->Bind(1);
		m_Texture2->Bind(0);

		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
			glm::mat4 mvp = m_Proj * m_View * model;  // the order is important!
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}
	}

	void TestDynamicBatchRendering::OnImGuiRender()
	{
		ImGui::SliderFloat2("Quad0 position", &m_QuadPosition.x, 0.0f, 960.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	std::array<Vertex, 4> CreateQuad(float x, float y, float textureID)  // x and y are the bottom left corner of the quad
	{
		float size = 200.0f;

		Vertex v0 = {
			{x, y},
			{0.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f},
			textureID };

		Vertex v1 = {
			{x + size, y},
			{0.0f, 0.0f, 0.0f, 0.0f},
			{1.0f, 0.0f},
			textureID };

		Vertex v2 = {
			{x + size, y + size},
			{0.0f, 0.0f, 0.0f, 0.0f},
			{1.0f, 1.0f},
			textureID };

		Vertex v3 = {
			{x,  y + size},
			{0.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f},
			textureID };

		return { v0, v1, v2, v3 };
	}

}