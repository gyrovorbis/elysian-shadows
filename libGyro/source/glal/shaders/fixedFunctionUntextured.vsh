uniform mat4	uProjMat;
//uniform mat4	uModelViewMat;

attribute vec4	aPosition;
attribute vec4	aColor;
attribute float aNormal;

varying vec4	vColor;

void main(void) {
	vColor = aColor;
	gl_Position = uProjMat * /* uModelViewMat * */ aPosition;
}
