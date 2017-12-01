#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <cassert>
#include <fstream>
#include "GLSL.h"

int main() 
{
	//initialize Open GL
	glfwInit();

	//make a window
	GLFWwindow* window = glfwCreateWindow(512, 512, "Dummy", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	//initialize Open GL Loader function
	gladLoadGL();

	//load input image
	int width, height, channels;
	unsigned char* data = stbi_load("input.png", &width, &height, &channels, 4); //store the input data on the CPU memory and get the address
	
	//make a texture (buffer) on the GPU to store the input image
	GLuint inTexture;					//Context number - Open GL returns a number to establish a context when texture is made
	int tex_w = 512, tex_h = 512;		//size
	glGenTextures(1, &inTexture);		//Generate texture and store context number
	glActiveTexture(GL_TEXTURE0);		//since we have 2 textures in this program, we need to associate the input texture with "0" meaning first texture
	glBindTexture(GL_TEXTURE_2D, inTexture);	//highlight input texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	//texture sampler parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	//texture sampler parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		//texture sampler parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		//texture sampler parameter
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_w, tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);	//copy image data to texture
	glBindImageTexture(0, inTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);	//enable texture in shader

	//make a texture (buffer) on the GPU to store the input image
	GLuint inKTexture;					//Context number - Open GL returns a number to establish a context when texture is made
	glGenTextures(1, &inKTexture);		//Generate texture and store context number
	glActiveTexture(GL_TEXTURE0);		//since we have 2 textures in this program, we need to associate the input texture with "0" meaning first texture
	glBindTexture(GL_TEXTURE_2D, inKTexture);	//highlight input texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	//texture sampler parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	//texture sampler parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		//texture sampler parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		//texture sampler parameter
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_w, tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);	//copy image data to texture
	glBindImageTexture(0, inKTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);	//enable texture in shader

	//make a texture (buffer) on the GPU to store the output image
	GLuint outTexture;
	glGenTextures(1, &outTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, outTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_w, tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindImageTexture(1, outTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
	stbi_image_free(data);

	//load the compute shader
	std::string ShaderString = readFileAsString("../resources/compute.glsl");
	const char *shader = ShaderString.c_str();	
	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShader, 1, &shader, nullptr);

	GLint rc;
	CHECKED_GL_CALL(glCompileShader(computeShader));
	CHECKED_GL_CALL(glGetShaderiv(computeShader, GL_COMPILE_STATUS, &rc));
	if (!rc)	//error compiling the shader file
		{
		GLSL::printShaderInfoLog(computeShader);
		std::cout << "Error compiling fragment shader " <<  std::endl;
		return false;
		}

	GLuint computeProgram = glCreateProgram();
	glAttachShader(computeProgram, computeShader);
	glLinkProgram(computeProgram);
	glUseProgram(computeProgram);
	int flap = 1;
	//compute shader loop, 200 iterations
	for (int i = 0; i < 200; i++)
		{
		glDispatchCompute((GLuint)tex_w, (GLuint)tex_h, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glBindImageTexture(!flap, outTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
		glBindImageTexture(flap, inTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
		glBindImageTexture(2, inKTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
		flap = !flap;
		}

	//getting the result in a PNG image
	std::vector<unsigned char> buffer(width * height * 4);
	glBindTexture(GL_TEXTURE_2D, inTexture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
	stbi_write_png("output.png", width, height, 4, buffer.data(), 0);

	glfwTerminate();

	return 0;
}
