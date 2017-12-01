#version 450 
layout(local_size_x = 1, local_size_y = 1) in;											//local group of shaders
layout(rgba8, binding = 0) uniform image2D img_input;									//input image
layout(rgba8, binding = 1) uniform image2D img_output;									//output image
void main() 
	{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);				
	vec4 l,u,r,d;//left up right down				
	vec4 col=imageLoad(img_input, pixel_coords);
	if(col.r<0.99)
		{
		//collecting neighbor pixel:
		l=u=r=d=vec4(0,0,0,1.0);
		if(pixel_coords.x>1)	l=imageLoad(img_input, pixel_coords + ivec2(-1,0));
		if(pixel_coords.y>1)	d=imageLoad(img_input, pixel_coords + ivec2(0,-1));
		if(pixel_coords.x<510)	r=imageLoad(img_input, pixel_coords + ivec2(1,0));
		if(pixel_coords.y<510)	u=imageLoad(img_input, pixel_coords + ivec2(0,1));
		col = (col + r + l + u + d)/5.0;												//my "own" heat distribution
		col =vec4(col.r,0,0,1);															//only red for now...
		}
	imageStore(img_output, pixel_coords, col);
	}