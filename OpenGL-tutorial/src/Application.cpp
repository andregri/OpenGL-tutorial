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

	float Positions[6] = {  // Each line is a vertex-position (a vertex is more than a position, it can contains more than positions)
		-0.5f, -0.5f,  // x and y positions of the vertex
		 0.0f,  0.5f,
		 0.5f, -0.5f,
	};

	// openGL works like a state machine.
	unsigned int Buffer;
	glGenBuffers(1, &Buffer);	// Create a buffer and returns the buffer id
	glBindBuffer(GL_ARRAY_BUFFER, Buffer);	// Select a buffer
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), Positions, GL_STATIC_DRAW);	// Put data in the buffer
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);	// Draw the bound buffer: when the shader receives the vertex buffer, it has to know the layout of that buffer.
		
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

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