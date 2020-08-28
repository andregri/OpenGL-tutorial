#include <GL/glew.h>     // Find the drivers and load gl functions
#include <GLFW/glfw3.h>  // Very simple library: create a window, a gl context

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if(!(x))  __debugbreak()
#ifdef _DEBUG
	#define GLCall(x) GLClearError();\
		x;\
		ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
	#define GLCall(x) x
#endif

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "): "
			<< file << ": " << function << ": " << line << '\n';
		return false;
	}
	return true;
}

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

	// openGL works like a state machine.
	// Vertex buffer
	unsigned int Buffer;
	GLCall(glGenBuffers(1, &Buffer));	// Create a buffer and returns the buffer id
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, Buffer));	// Select a buffer
	GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), Positions, GL_STATIC_DRAW));	// Put data in the buffer

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));  // define the data format of the vertex

	// Index buffer
	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));	// Create a buffer and returns the buffer id
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));	// Select a buffer
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));	// Put data in the buffer

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	std::cout << "VERTEX\n" << source.VertexSource << '\n';
	std::cout << "FRAGMENT\n" << source.FragmentSource << '\n';

	unsigned int Shader = CreateShader(source.VertexSource, source.FragmentSource);
	GLCall(glUseProgram(Shader));

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		//GLCall(glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr));	// This will raise an error.
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));	// Draw the bound buffer: when the shader receives the vertex buffer, it has to know the layout of that buffer.

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