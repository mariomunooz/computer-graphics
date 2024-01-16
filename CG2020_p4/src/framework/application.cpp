#include "application.h"
#include "utils.h"
#include "includes.h"
#include "utils.h"

#include "image.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "material.h"
#include "light.h"

Camera* camera = NULL;
Mesh* mesh = NULL;
Shader* shader = NULL;

//might be useful...
Material* material = NULL;
Light* light = NULL;
Shader* phong_shader = NULL;
Shader* gouraud_shader = NULL;

Vector3 ambient_light(0.1,0.2,0.3); //here we can store the global ambient light of the scene

float angle = 0;

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	// initialize attributes
	// Warning: DO NOT CREATE STUFF HERE, USE THE INIT 
	// things create here cannot access opengl
	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(NULL);
}

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;
	
	//here we create a global camera and set a position and projection properties
	camera = new Camera();
	camera->lookAt(Vector3(0,20,20),Vector3(0,10,0),Vector3(0,1,0));
	camera->setPerspective(60,window_width / window_height,0.1,10000);

	//then we load a mesh
	mesh = new Mesh();
	if( !mesh->loadOBJ( "../res/meshes/lee.obj" ) )
		std::cout << "FILE Lee.obj NOT FOUND " << std::endl;

	//we load one or several shaders...
	shader = Shader::Get( "../res/shaders/simple.vs", "../res/shaders/simple.fs" );

	//load your Gouraud and Phong shaders here and stored them in some global variables
	
	phong_shader = Shader::Get("../res/shaders/phong/phong.vs", "../res/shaders/phong/phong.fs");
	gouraud_shader = Shader::Get("../res/shaders/gouraud/gouuraud.vs", "../res/shaders/gouraud/gouuraud.fs");


	//CODE HERE:
	//create a light (or several) and and some materials
	//...
	light = new Light();
	material = new Material();
}

//render one frame
void Application::render(void)
{
	//update the aspect of the camera acording to the window size
	camera->aspect = window_width / window_height;
	camera->updateProjectionMatrix();
	//Get the viewprojection matrix from our camera
	Matrix44 viewprojection = camera->getViewProjectionMatrix();

	//set the clear color of the colorbuffer as the ambient light so it matches
	glClearColor(ambient_light.x, ambient_light.y, ambient_light.z, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear framebuffer and depth buffer 
	glEnable( GL_DEPTH_TEST ); //enable depth testing for occlusions
	glDepthFunc(GL_LEQUAL); //Z will pass if the Z is LESS or EQUAL to the Z of the pixel

	//choose a shader and enable it
	gouraud_shader->enable();

	Matrix44 model_matrix;
	model_matrix.setIdentity();
	model_matrix.translate(0,0,0); //example of translation
	model_matrix.rotate(angle, Vector3(0, 1, 0));
	gouraud_shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
	gouraud_shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader

	//CODE HERE: pass all the info needed by the shader to do the computations
	//send the material and light uniforms to the shader
	//...

	//variables needed for the gouraud Shader
	gouraud_shader->setVector3("camera_position", camera->eye);
	gouraud_shader->setVector3("light_diffuse_color", light->diffuse_color);
	gouraud_shader->setVector3("light_position", light->position);
	gouraud_shader->setVector3("light_specular_color", light->specular_color);
	gouraud_shader->setVector3("material_ambient", material->ambient);
	gouraud_shader->setVector3("material_diffuse", material->diffuse);
	gouraud_shader->setUniform1("material_shininess", material->shininess);
	gouraud_shader->setVector3("material_specular", material->specular);
	gouraud_shader->setVector3("ambient_light", ambient_light);


	//variables needed for the phong Shader
	phong_shader->setVector3("camera_position", camera->eye);
	phong_shader->setVector3("light_diffuse_color", light->diffuse_color);
	phong_shader->setVector3("light_position", light->position);
	phong_shader->setVector3("light_specular_color", light->specular_color);
	phong_shader->setVector3("material_ambient", material->ambient);
	phong_shader->setVector3("material_diffuse", material->diffuse);
	phong_shader->setUniform1("material_shininess", material->shininess);
	phong_shader->setVector3("material_specular", material->specular);
	phong_shader->setVector3("ambient_light", ambient_light);


	//do the draw call into the GPU
	mesh->render(GL_TRIANGLES);

	//disable shader when we do not need it any more
	gouraud_shader->disable();

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

//called after render
void Application::update(double seconds_elapsed)
{
	if (keystate[SDL_SCANCODE_SPACE])
		angle += seconds_elapsed;

	if (keystate[SDL_SCANCODE_RIGHT])
		camera->eye = camera->eye + Vector3(1, 0, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_LEFT])
		camera->eye = camera->eye + Vector3(-1, 0, 0) * seconds_elapsed * 10.0;
	if (keystate[SDL_SCANCODE_UP])
		camera->eye = camera->eye + Vector3(0, 1, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_DOWN])
		camera->eye = camera->eye + Vector3(0, -1, 0) * seconds_elapsed * 10.0;
}

//keyboard press event 
void Application::onKeyPressed( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: exit(0); break; //ESC key, kill the app
		case SDLK_r: 
			Shader::ReloadAll();
			break; //ESC key, kill the app
	}
}

//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{
	}

}

void Application::onMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse unpressed
	{

	}
}

//when the app starts
void Application::start()
{
	std::cout << "launching loop..." << std::endl;
	launchLoop(this);
}
