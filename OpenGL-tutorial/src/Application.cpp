#include <GL/glew.h>     // Find the drivers and load gl functions
#include <GLFW/glfw3.h>  // Very simple library: create a window, a gl context

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	std::cout << "VERTEX\n" << source.VertexSource << '\n';
	std::cout << "FRAGMENT\n" << source.FragmentSource << '\n';

	unsigned int Shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(Shader);

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

	glDeleteProgram(Shader);

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