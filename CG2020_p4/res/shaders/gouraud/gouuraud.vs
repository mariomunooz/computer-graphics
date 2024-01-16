//global variables from the CPU
uniform mat4 model;
uniform mat4 viewprojection;

//vars to pass to the pixel shader
varying vec3 v_wPos;
varying vec3 v_wNormal;

//here create uniforms for all the data we need here
uniform vec3 camera_position; // Point
uniform vec3 light_diffuse_color; // RGB
uniform vec3 light_position; // Point
uniform vec3 light_specular_color; // RGB
uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular; 
uniform float material_shininess; // constant
uniform vec3 ambient_light;

//outputs
varying vec3 reflected_ambient_light;
varying vec3 reflected_diffuse_light;
varying vec3 reflected_specular_light;



// vectors

uniform vec3 light_normalized_vector;
uniform vec3 view_normalized_vector;
uniform vec3 reflected_normalized_vector;

// dot products

uniform float dot_product_L_N;
uniform float dot_product_R_V_shining;


varying vec3 color;






void main()
{	
	//convert local coordinate to world coordinates
	vec3 wPos = (model * vec4( gl_Vertex.xyz, 1.0)).xyz;
	//convert local normal to world coordinates
	vec3 wNormal = (model * vec4( gl_Normal.xyz, 0.0)).xyz;

	//pass them to the pixel shader interpolated
	v_wPos = wPos;
	v_wNormal = wNormal;

	//in GOURAUD compute the color here and pass it to the pixel shader
	//...


	// computing vectors
	vec3 light_normalized_vector =  normalize(light_position - v_wPos);
	vec3 view_normalized_vector = normalize( camera_position - v_wPos);
	vec3 reflected_normalized_vector = normalize(reflect(-1.0*light_normalized_vector, v_wNormal));
	

	// AMBIENT LIGHT
	reflected_ambient_light = ambient_light * material_ambient;

	//DIFFUSE LIGHT

	float dot_product_L_N = clamp(dot(light_normalized_vector, v_wNormal), 0, 1);
	reflected_diffuse_light = material_diffuse * light_diffuse_color * dot_product_L_N;
	
	// SPECULAR LIGHT
	float dot_product_R_V_shining = pow(clamp(dot(reflected_normalized_vector, view_normalized_vector), 0, 1), material_shininess);
	reflected_specular_light = material_specular * light_specular_color * dot_product_R_V_shining;

	// TOTAL LIGHT

	color = reflected_ambient_light + reflected_diffuse_light + reflected_specular_light;

	//project the vertex by the model view projection 
	gl_Position = viewprojection * vec4(wPos,1.0); //output of the vertex shader


}