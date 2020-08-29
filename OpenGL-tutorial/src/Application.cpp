#include <GL/glew.h>     // Find the drivers and load gl functions
#include <GLFW/glfw3.h>  // Very simple library: create a window, a gl context

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				// set mode to vertex
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				// set mode to fragment
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return {ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char *)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<" shader\n";
		std::cout << message << '\n';
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& VertexShader, const std::string& FragmentShader)
{
	unsigned int Program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, VertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, FragmentShader);

	glAttachShader(Program, vs);
	glAttachShader(Program, fs);
	glLinkProgram(Program);
	glValidateProgram(Program);

	// We got a program so we can delete our shader source code.
	// The shader source is necessary for debugging.
	glDeleteShader(vs);
	glDeleteShader(fs);

	return Program;
}

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
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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
		float Positions[] = {  // Each line is a vertex-position (a vertex is more than a position, it can contains more than positions)
			-0.5f, -0.5f,  // x and y positions of the vertex
			 0.5f, -0.5f,
			 0.5f,  0.5f,
			-0.5f,  0.5f,
		};

		unsigned int indices[] = {  // You must use an unsigned type
			0, 1, 2,  // indices for the right triangle
			2, 3, 0,  // indices for the left triangle
		};

		// Create a vertex array object
		unsigned int vao;
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		// openGL works like a state machine.
		// Vertex buffer
		VertexBuffer vb(Positions, 4 * 2 * sizeof(float));

		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

		// Index buffer
		IndexBuffer ib(indices, 6);

		ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
		std::cout << "VERTEX\n" << source.VertexSource << '\n';
		std::cout << "FRAGMENT\n" << source.FragmentSource << '\n';

		unsigned int Shader = CreateShader(source.VertexSource, source.FragmentSource);
		GLCall(glUseProgram(Shader));

		GLCall(int location = glGetUniformLocation(Shader, "u_Color"));
		ASSERT(location != -1);
		GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));

		// Unbound
		GLCall(glBindVertexArray(0));
		GLCall(glUseProgram(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));


		float r = 0.0f;
		float increment = 0.05f;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			GLCall(glClear(GL_COLOR_BUFFER_BIT));

			GLCall(glUseProgram(Shader));
			GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

			GLCall(glBindVertexArray(vao));
			ib.Bind();

			//GLCall(glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr));	// This will raise an error.
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));	// Draw the bound buffer: when the shader receives the vertex buffer, it has to know the layout of that buffer.

			if (r > 1.0f)
			{
				increment = -0.05f;
			}
			else if (r < 0.0f)
			{
				increment = 0.05f;
			}
			r += increment;

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}

		GLCall(glDeleteProgram(Shader));
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