varying vec4	vColor;
varying vec2	vTexCoord;

uniform sampler2D uTexture;

void main(void) {

	gl_FragColor = texture2D(uTexture, vTexCoord) * vColor;
}
