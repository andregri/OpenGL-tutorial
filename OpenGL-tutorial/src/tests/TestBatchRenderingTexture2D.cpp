#include "TestBatchRenderingTexture2D.h"

#include "Renderer.h"

#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test {

	TestBatchRenderingTexture2D::TestBatchRenderingTexture2D()
		: m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)))
	{
		float Positions[] = {  // Each line is a vertex-position (a vertex is more than a position, it can contains more than positions)
			//  x        y     r     g     b    alpha texX  texY  texIndex
			-100.0f, -100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			 100.0f, -100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			 100.0f,  100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			-100.0f,  100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
									  			
			 200.0f, -100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			 400.0f, -100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 400.0f,  100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			 200.0f,  100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
		};

		unsigned int indices[] = {  // You must use an unsigned type
			0, 1, 2, 2, 3, 0,  // indices for one quad
			4, 5, 6, 6, 7, 4
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		m_VAO = std::make_unique<VertexArray>();

		m_VertexBuffer = std::make_unique<VertexBuffer>(Positions, 9 * 8 * sizeof(float));
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


	TestBatchRenderingTexture2D::~TestBatchRenderingTexture2D()
	{
	}

	void TestBatchRenderingTexture2D::OnUpdate(float deltaTime)
	{
	}

	void TestBatchRenderingTexture2D::OnRender()
	{
		GLCall(glClearColor(1.0f, 1.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer renderer;

		m_Texture1->Bind(0);
		m_Texture2->Bind(1);

		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));
			glm::mat4 mvp = m_Proj * m_View * model;  // the order is important!
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}
	}

	void TestBatchRenderingTexture2D::OnImGuiRender()
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

}