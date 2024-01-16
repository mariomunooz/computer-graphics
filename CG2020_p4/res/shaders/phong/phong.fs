//this var comes from the vertex shader
//they are baricentric interpolated by pixel according to the distance to every vertex
varying vec3 v_wPos;
varying vec3 v_wNormal;

//here create uniforms for all the data we need here

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
uniform vec3 reflected_ambient_light;
uniform vec3 reflected_diffuse_light;
uniform vec3 reflected_specular_light;

// vectors

uniform vec3 light_normalized_vector;
uniform vec3 view_normalized_vector;
uniform vec3 reflected_normalized_vector;

// dot products

uniform float dot_product_L_N;
uniform float dot_product_R_V_shining;



uniform vec3 color;

void main()
{
	//here we set up the normal as a color to see them as a debug
	//vec3 color = v_wNormal;

	//here write the computations for PHONG.
	
	// computing vectors
	vec3 light_normalized_vector =  normalize(light_position - v_wPos);
	vec3 view_normalized_vector = normalize( camera_position - v_wPos);
	vec3 reflected_normalized_vector = normalize(reflect(-1*light_normalized_vector, v_wNormal));
	

	// AMBIENT LIGHT
	vec3 reflected_ambient_light = ambient_light * material_ambient;

	//DIFFUSE LIGHT

	float dot_product_L_N = clamp(dot(light_normalized_vector, v_wNormal), 0, 1);
	vec3 reflected_diffuse_light = material_diffuse * light_diffuse_color * dot_product_L_N;
	
	// SPECULAR LIGHT
	float dot_product_R_V_shining = pow(clamp(dot(reflected_normalized_vector, view_normalized_vector), 0, 1), material_shininess);
	vec3 reflected_specular_light = material_specular * light_specular_color * dot_product_R_V_shining;

	// TOTAL LIGHT

	vec3 color = reflected_ambient_light + reflected_diffuse_light + reflected_specular_light;

	//set the ouput color por the pixel
	gl_FragColor = vec4( color, 1.0 ) * 1.0;
}
