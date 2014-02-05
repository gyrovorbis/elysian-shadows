attribute vec4 aPosition;

uniform mat4 uProjViewMat;

void main(void)
{
	gl_Position = uProjViewMat * aPosition;
	//gl_Position.xyz /= gl_Position.w;
}
