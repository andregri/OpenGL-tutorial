#include <GL/glew.h>     // Find the drivers and load gl functions
#include <GLFW/glfw3.h>  // Very simple library: create a window, a gl context

#include <iostream>

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error!\n";
	}

	std::cout << glGetString(GL_VERSION) << '\n';

	float Positions[6] = {
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f,
	};


	// openGL works like a state machine.
	unsigned int Buffer;
	glGenBuffers(1, &Buffer);	// Create a buffer and returns the buffer id
	glBindBuffer(GL_ARRAY_BUFFER, Buffer);	// Select a buffer
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), Positions, GL_STATIC_DRAW);	// Put data in the buffer

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);	// Draw the bound buffer
		
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}