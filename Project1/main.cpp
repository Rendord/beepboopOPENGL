#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "objloader.h"
#include <vector>

#include "glsl.h"
#include "texture.h"

using namespace std;

struct LightSource
{
	glm::vec3 position;
};

struct Material
{
	glm::vec3 ambient_color;
	glm::vec3 diffuse_color;
	glm::vec3 specular;
	float power;
};
//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------

const int WIDTH = 800, HEIGHT = 600;

const char* fragshader_name = "fragmentshader.frag";
const char* vertexshader_name = "vertexshader.vert";

unsigned const int DELTA_TIME = 0;

const int objectamount = 3;

int previoustimepassed;
int deltatime;

//keybuffer for using simultaneous keypresses
bool keybuffer[127];


//--------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------

// ID's
GLuint program_id;
GLuint vao[objectamount];
GLuint texture_id[objectamount];
//GLuint uv_id;

LightSource light;
Material material[objectamount];

// Uniform ID's
GLuint uniform_mv;
GLuint uniform_proj;
GLuint uniform_light_pos;
GLuint uniform_material_ambient;
GLuint uniform_material_diffuse;
GLuint uniform_specular;
GLuint uniform_power;
//glm::vec3 light_position, ambient_color, diffuse_color, specular;
//float power;

// Matrices
glm::mat4 model[objectamount], view, projection;
glm::mat4 mv[objectamount];
//glm::mat4 mvp;

//lecture 8
vector<glm::vec3> normals[objectamount];
vector<glm::vec3> vertices[objectamount];
vector<glm::vec2> uvs[objectamount];

//camera
glm::vec3 cameraPos = glm::vec3(-1.0, 2.0, 12.0);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//camera speed
const float multiplier = 0.08f;

bool res;

//--------------------------------------------------------------------------------
// Mesh variables
//--------------------------------------------------------------------------------

//------------------------------------------------------------
//
//           7----------6
//          /|         /|
//         / |        / |
//        /  4-------/--5               y
//       /  /       /  /                |
//      3----------2  /                 ----x
//      | /        | /                 /
//      |/         |/                  z
//      0----------1
//------------------------------------------------------------

//const GLfloat vertices[] = {
//    // front
//    -1.0, -1.0, 1.0,
//    1.0, -1.0, 1.0,
//    1.0, 1.0, 1.0,
//    -1.0, 1.0, 1.0,
//    // back
//    -1.0, -1.0, -1.0,
//    1.0, -1.0, -1.0,
//    1.0, 1.0, -1.0,
//    -1.0, 1.0, -1.0,
//};
//
//const GLfloat colors[] = {
//    // front colors
//    1.0, 1.0, 0.0,
//    0.0, 1.0, 0.0,
//    0.0, 0.0, 1.0,
//    1.0, 1.0, 1.0,
//    // back colors
//    0.0, 1.0, 1.0,
//    1.0, 0.0, 1.0,
//    1.0, 0.0, 0.0,
//    1.0, 1.0, 0.0,
//};
//
//GLushort cube_elements[] = {
//    0,1,1,2,2,3,3,0,  // front
//    0,4,1,5,3,7,2,6,  // front to back
//    4,5,5,6,6,7,7,4   // back
//};


//--------------------------------------------------------------------------------
// Keyboard handling
//--------------------------------------------------------------------------------

void keyboardDownHandler(unsigned char key, int a, int b)
{
	float speed = multiplier * deltatime;
    if (key == 27)
        glutExit();
	if(key == 119) //w
		cameraPos += speed * cameraFront;
	if(key == 97) //a
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
	if(key == 115) //s
		cameraPos -= speed * cameraFront;
	if(key == 100) //d
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
		//up
		//down
		//pitch
		//roll 
		///yaw
}


void keyboardUpHandler(unsigned char key, int a, int b)
{
	float speed = multiplier * deltatime;
	if (key == 119) //w
		keybuffer[119] = true;
	if (key == 97) //a
		keybuffer[97] = true;
	if (key == 115) //s
		keybuffer[115] = true;
	if (key == 100) //d
		keybuffer[100] = true;
	//up
	//down
	//pitch
	//roll 
	///yaw
}



//--------------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------------

void Render()
{
	//get DeltaTime
	int timepassed = glutGet(GLUT_ELAPSED_TIME);
	deltatime = timepassed - previoustimepassed; 
	previoustimepassed = timepassed;

	//clearbuffer 
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Attach to program_id
    glUseProgram(program_id);

	//rotate die shit naar rechts 
	model[0] = glm::rotate(model[0], 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
	//rotate die shit naar links
	model[1] = glm::rotate(model[1], 0.01f, glm::vec3(0.0f, -1.0f, 0.0f));

	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	for (int i = 0; i < objectamount; i++) {

		// Do transformation
		
		mv[i] = view * model[i];

	

		// Send mvp
		glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(mv[i]));

		// bind texture
		glBindTexture(GL_TEXTURE_2D, texture_id[i]);


		// Send vao
		/*glBindVertexArray(vao);
		glDrawElements(GL_LINES, sizeof(cube_elements) / sizeof(GLushort),
			GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);*/
		glUniform3fv(uniform_light_pos, 1, glm::value_ptr(light.position));
		glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(material[i].ambient_color));
		glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(material[i].diffuse_color));
		glUniform3fv(uniform_specular, 1, glm::value_ptr(material[i].specular));
		glUniform1f(uniform_power, material[i].power);

		glBindVertexArray(vao[i]);
		glDrawArrays(GL_TRIANGLES, 0, vertices[i].size());
		glBindVertexArray(0);

	}

    glutSwapBuffers();
}


//------------------------------------------------------------
// void Render(int n)
// Render method that is called by the timer function
//------------------------------------------------------------

void Render(int n)
{
    Render();
    glutTimerFunc(DELTA_TIME, Render, 0);
}


//------------------------------------------------------------
// void InitGlutGlew(int argc, char **argv)
// Initializes Glut and Glew
//------------------------------------------------------------

void InitGlutGlew(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Hello OpenGL");
    glutDisplayFunc(Render);
    glutKeyboardFunc(keyboardDownHandler);
	glutKeyboardUpFunc(keyboardUpHandler);
    glutTimerFunc(DELTA_TIME, Render, 0);

    glewInit();
}


//------------------------------------------------------------
// void InitShaders()
// Initializes the fragmentshader and vertexshader
//------------------------------------------------------------

void InitShaders()
{
    char* vertexshader = glsl::readFile(vertexshader_name);
    GLuint vsh_id = glsl::makeVertexShader(vertexshader);

    char* fragshader = glsl::readFile(fragshader_name);
    GLuint fsh_id = glsl::makeFragmentShader(fragshader);

    program_id = glsl::makeShaderProgram(vsh_id, fsh_id);
}


void InitLoadObjects(int teller) {
	
	const char* objects[objectamount] = { "teapot.obj", "sphere.obj","torus.obj" };
	for (int i = 0; i < teller; i++) {
		bool res = loadOBJ(objects[i], vertices[i], uvs[i], normals[i]);
		texture_id[i] = loadBMP("Textures/uvtemplate.bmp");
	} 

}

//------------------------------------------------------------
// void InitMatrices()
//------------------------------------------------------------

void InitMatrices(int teller)
{
	model[0] = glm::translate(glm::mat4(), glm::vec3(1.5, 0.0, 0.0));
	model[1] = glm::translate(glm::mat4(), glm::vec3(-1.5,0.0,0.0));
	model[2] = glm::translate(glm::mat4(), glm::vec3(-1.5, 0.0, 4.5));

	//glm::vec3 VectorPosition = glm::vec3(0.0f, 0.0f, 3.0f);
	//glm::vec3 vectorFront = glm::vec3(0.0f, 0.0f, -1.0f);
	//glm::vec3 VectorUp = glm::vec3(0.0f, 1.0f, 0.0f);




	for (int i = 0; i < teller; i++) {		
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(
			glm::radians(45.0f),
			1.0f * WIDTH / HEIGHT, 0.1f,
			120.0f);

		mv[i] = view * model[i];

	}
}


//------------------------------------------------------------
// void InitBuffers()
// Allocates and fills buffers
//------------------------------------------------------------

void InitLight(int teller) {

	light.position = glm::vec3(4.0, 4.0, 4.0);

	for (int i = 0; i < teller; i++) {
		material[i].ambient_color = glm::vec3(0.0, 0.0, 0.0);
		material[i].diffuse_color = glm::vec3(0.0, 0.0, 0.0);
		material[i].specular = glm::vec3(1.0);
		material[i].power = 1024;
	}

	//light_position = glm::vec3(4.0,4.0,4.0);
	//ambient_color = glm::vec3(0.2,0.2,0.2);
	//diffuse_color = glm::vec3(0.5,0.5,0.5);
	//specular = glm::vec3(0.7, 0.7, 0.7);
	//power = 1024;

}

void InitBuffers(int teller)
{
    GLuint position_id, color_id;
	GLuint vbo_vertices;
	GLuint vbo_normals;
	GLuint vbo_uvs;



	//vbo_colors;
    //GLuint ibo_elements;

	for (int i = 0; i < teller; i++) {

		// vbo for uvs
		glGenBuffers(1, &vbo_uvs);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
		glBufferData(GL_ARRAY_BUFFER, uvs[i].size() * sizeof(glm::vec2),
			&uvs[i][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		// vbo for normals
		glGenBuffers(1, &vbo_normals);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
		glBufferData(GL_ARRAY_BUFFER,
			normals[i].size() * sizeof(glm::vec3),
			&normals[i][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		// vbo for vertices
		//glGenBuffers(1, &vbo_vertices);
		//glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		glGenBuffers(1, &vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER,
			vertices[i].size() * sizeof(glm::vec3), &vertices[i][0],
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		// vbo for colors
		//glGenBuffers(1, &vbo_colors);
		//glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		//// vbo for elements
		//glGenBuffers(1, &ibo_elements);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements),
		//    cube_elements, GL_STATIC_DRAW);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Get vertex attributes
		GLuint normal_id = glGetAttribLocation(program_id, "normal");
		position_id = glGetAttribLocation(program_id, "position");
		color_id = glGetAttribLocation(program_id, "color");
		GLuint uv_id = glGetAttribLocation(program_id, "uv");

		// Allocate memory for vao
		glGenVertexArrays(1, &vao[i]);

		// Bind to vao
		glBindVertexArray(vao[i]);

		// Bind vertices to vao
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(position_id);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Bind normals to vao
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
		glVertexAttribPointer(normal_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(normal_id);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Bind uvs to to vao
		glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
		glVertexAttribPointer(uv_id, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(uv_id);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		// Bind colors to vao
		//glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
		//glVertexAttribPointer(color_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(color_id);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Bind elements to vao
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);

		// Stop bind to vao
		glBindVertexArray(0);
	}

	glUseProgram(program_id);
    // Make uniform vars
    //uniform_mvp = glGetUniformLocation(program_id, "mvp");
	uniform_mv = glGetUniformLocation(program_id, "mv");
	uniform_proj = glGetUniformLocation(program_id, "projection");
	uniform_light_pos = glGetUniformLocation(program_id, "light_pos");
	uniform_material_ambient = glGetUniformLocation(program_id,
		"mat_ambient");
	uniform_material_diffuse = glGetUniformLocation(program_id,
		"mat_diffuse");
	uniform_specular = glGetUniformLocation(program_id, "mat_specular");
	uniform_power = glGetUniformLocation(program_id, "mat_power");

	// Fill uniform vars
	glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));

}


int main(int argc, char** argv)
{
    InitGlutGlew(argc, argv);
    InitShaders();
    InitMatrices(objectamount);
	InitLoadObjects(objectamount);
	InitLight(objectamount);
    InitBuffers(objectamount);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Hide console window
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    // Main loop
    glutMainLoop();

    return 0;
}
