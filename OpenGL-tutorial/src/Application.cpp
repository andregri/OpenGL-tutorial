#include <GL/glew.h>     // Find the drivers and load gl functions
#include <GLFW/glfw3.h>  // Very simple library: create a window, a gl context

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "tests/Test.h"
#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"
#include "tests/TestBatchRenderingQuads.h"
#include "tests/TestBatchRenderingColors.h"
#include "tests/TestBatchRenderingTexture2D.h"

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // I want core profile instead of compatibility profile.

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);  // for synchronization

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error!\n";
	}

	std::cout << glGetString(GL_VERSION) << '\n';

	{
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		Renderer renderer;

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		// GL 3.2 + GLSL 150
		const char* glsl_version = "#version 150";
		ImGui_ImplOpenGL3_Init(glsl_version);

		test::Test* currentTest = nullptr;
		test::TestMenu* testMenu = new test::TestMenu(currentTest);
		currentTest = testMenu;

		testMenu->RegisterTest<test::TestClearColor>("Clear Color");
		testMenu->RegisterTest<test::TestTexture2D>("2D Texture");
		testMenu->RegisterTest<test::TestBatchRenderingQuads>("Batch Rendering of quads");
		testMenu->RegisterTest<test::TestBatchRenderingColors>("Batch Rendering Colors");
		testMenu->RegisterTest<test::TestBatchRenderingTexture2D>("Batch Rendering 2D Texture");

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			renderer.Clear();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			if (currentTest)
			{
				currentTest->OnUpdate(0.0f);
				currentTest->OnRender();
				ImGui::Begin("Test");
				if (currentTest != testMenu && ImGui::Button("<-"))
				{
					delete currentTest;
					currentTest = testMenu;
				}
				currentTest->OnImGuiRender();
				ImGui::End();
			}

			// Rendering
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}

		delete currentTest;
		if (currentTest != testMenu)
		{
			delete testMenu;
		}
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();
	return 0;
}

/*
06 - A shader is a program that runs in the GPU. We can write the shader as a string and pass it to the GPU
that compile and runs it. You have to tell the GPU what to do with the provided data (shape, color, ...).
There are 2 main shader types: vertex shaders and fragment shaders.
Rendering pipeline: from data to result on the screen.
When we call the Draw call first the vertex shader is called and then the fragment shader is called and we see
the result on the screen. The vertex shader is called for every vertex (in a triangle is called 3 times). Its
main purpose is to draw the vertex in a specific position. The fragment shader / pixel shader run once for each
pixel that has to be rasterized on the screen (the rasterization process fills the pixel in the triangle, put the
color). So vertex shader is called 3 times, the fragment shader can be called thousands, millions of times.
The color depends on the lighting, the texture, etc... the fragment shader determines the color of the pixels.
Shaders can be very complex and can be generated at run time for complex applications.
*/