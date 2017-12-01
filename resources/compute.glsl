#version 450 
layout(local_size_x = 1, local_size_y = 1) in;											//local group of shaders
layout(rgba8, binding = 0) uniform image2D img_input;									//input image
layout(rgba8, binding = 1) uniform image2D img_output;									//output image
layout(rgba8, binding = 2) uniform image2D img_inputK;									//output image
void main() 
	{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);				
	vec4 l,u,r,d;//left up right down				
	float k;//left up right down				
	vec4 col=imageLoad(img_output, pixel_coords);
	vec4 colK=imageLoad(img_inputK, pixel_coords);
	//if(col.r < 0.01 && col.g < 0.01 && col.b < 0.01)
	if(colK.rgb != vec3(0.0, 0.0, 1.0)) {
		//collecting neighbor pixel:
		l=u=r=d=vec4(0,0,0,1.0);
		if(pixel_coords.x>1)	l=imageLoad(img_output, pixel_coords + ivec2(-1,0));
		if(pixel_coords.y>1)	d=imageLoad(img_output, pixel_coords + ivec2(0,-1));
		if(pixel_coords.x<510)	r=imageLoad(img_output, pixel_coords + ivec2(1,0));
		if(pixel_coords.y<510)	u=imageLoad(img_output, pixel_coords + ivec2(0,1));
		
		if (colK.rgb == vec3(0.0)) {
			k = 0.2f;
		}
		else if (colK.rgb == vec3(1.0, 0.0, 0.0)) {
			k = 0.15f;
		}
		else if (colK.rgb == vec3(0.0, 1.0, 0.0)) {
			k = 0.05f;
		}
		
		col.a = col.a + k * (l.a + d.a + r.a + u.a - 4*col.a);
												
		col.xyz = vec3(1,0,0);	
		//col = vec4(0.2, 0.3, 0.4, 1.0);										
		}
	else {
		col = vec4(1.0, 0.0, 0.0, 1.0);
	}
	imageStore(img_output, pixel_coords, col);
	}
