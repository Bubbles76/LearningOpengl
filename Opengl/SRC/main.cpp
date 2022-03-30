#include<iostream>
#include<gl/glew.h>
#include<GLFW/glfw3.h>

//documentation OpenGL
//https://docs.gl/

static void HandleInput(GLFWwindow* window) 
{

	// Poll for and process events
	glfwPollEvents();

	// Get Key input 
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
// Vertex Shader Data
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"vec2 Resolution = vec2(640, 480);\n"
"void main()\n"
"{\n"
"   vec2 st = gl_FragCoord.xy/Resolution;\n"
"   FragColor = vec4(st.x, st.y, 0.8f, 1.0f);\n"
"}\n\0";
int main()
{
	//initialize and configure GLFW
	if (!glfwInit())
	{
		std::cout << "[GLFW]: Failed to initalize"<<std::endl;
		return 1;
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_ANY_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Creating the GLFW Window
	GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL", NULL, NULL);
	if (!window)
	{
		std::cout << "[GLFW]: Failed to Create window" << std::endl;
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window,framebufferResizeCallback);


	if (glewInit())
	{
		std::cout << "[GLEW]: Failed to initialize" << std::endl;
		return 1;
	}

	// vertex shader
	unsigned int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShaderID);


	// check for shader compile errors
	int success;
	char infoLog[512];

	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShaderID, 512, NULL, infoLog);
		std::cout << "[OpenGL] : Failed to compile vertex shader..\n" << infoLog << std::endl;
	}

	//fragment shader
	unsigned int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShaderID);


	// check for shader compile errors
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderID, 512, NULL, infoLog);
		std::cout << "[OpenGL]: Failed to compile fragment shader.. \n" << infoLog << std::endl;
	}

	//link shader

	unsigned int shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShaderID);
	glAttachShader(shaderProgramID, fragmentShaderID);
	glLinkProgram(shaderProgramID);

	// check for linking errors
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
		std::cout << "[OpenGL]: Failed to link shader.. \n" << infoLog << std::endl;
	}

	// Not longer needed since they have been compiled into our shader
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);


	// Normalized Device Coordinates Triangle Vertex Data 
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, // Bottom Left
		-0.5f,  0.5f, 0.0f,  // top left 
		 0.5f, -0.5f, 0.0f, // Bottom Right
		 0.5f, 0.5f, 0.0f  // top right
		
	};

	unsigned int indices[] = {
		0,1,2,
		1,2,3

	};

	unsigned int VBO, VAO, EBO;// IDs for..
	glGenVertexArrays(1, &VAO);// Vertex Array Object (VBO)
	glGenBuffers(1, &VBO);// Vertex Buffer Object (VBO)
	glGenBuffers(1, &EBO);// Element Buffer Object (VBO)

	 // Bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	// Set VBO Vertex Buffer Data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Set EBO Buffer data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Set AttribPointer data stride 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);


	while (!glfwWindowShouldClose(window))
	{

		//handles input 
		HandleInput(window);

		// Clear
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw
		glUseProgram(shaderProgramID);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		// Swap to backbuffer for window
		glfwSwapBuffers(window);

	}

	//Destroy GPU objects
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgramID);

	//Destory window
	glfwTerminate();
	return 0;

}