uniform mat4	uProjMat;
//uniform mat4	uModelViewMat;

attribute vec4	aPosition;
attribute vec4	aColor;
attribute vec2	aTexCoord;
attribute float aNormal;

varying vec4	vColor;
varying vec2	vTexCoord;

void main(void) {
	vColor = aColor;
	vTexCoord = aTexCoord;
	gl_Position = uProjMat * /*uModelViewMat * */ aPosition;
}
