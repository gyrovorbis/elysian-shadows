
#if 0

#include "tkMapSystem.h"
#include <gyro_video_api.h>
#include <glal/gyro_video_glal.h>
#include "elysian_engine.hpp"

void TkMapSystem::initializeGL(void) {
    makeCurrent();
        qDebug() << "VERSION " << (char*)glGetString(GL_VERSION) <<"SCIZOR" << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    gyVidOglInit(size().width(), size().height(), size().width(), size().height());
    //getCurArea()->populateVbo();
    qDebug() << "C++ CONTEXT - " << context();
    //updateGL();


}

void TkMapSystem::resizeGL(int w, int h) {
    //makeCurrent();
    //gyVidOglInit(w, h, w, h);
    updateGL();
    qDebug() << isValid();
}

void TkMapSystem::paintGL(void) {
        qDebug() << "VERSION " << (char*)glGetString(GL_VERSION);
   // makeCurrent();
    qDebug() << isValid();
//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
engine->update();
    engine->render();
}

void TkMapSystem::paintEvent(QPaintEvent* event) {

}

#else


#include "tkMapSystem.h"

#include <QMouseEvent>

#include <math.h>
#include <locale.h>

TkMapSystem::TkMapSystem(QWidget *parent) :
	QGLWidget(parent),
	angularSpeed(0)
{
}

TkMapSystem::~TkMapSystem()
{
	deleteTexture(texture);
}

//! [0]
void TkMapSystem::mousePressEvent(QMouseEvent *e)
{
	// Save mouse press position
	mousePressPosition = QVector2D(e->localPos());
}

void TkMapSystem::mouseReleaseEvent(QMouseEvent *e)
{
	// Mouse release position - mouse press position
	QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

	// Rotation axis is perpendicular to the mouse position difference
	// vector
	QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

	// Accelerate angular speed relative to the length of the mouse sweep
	qreal acc = diff.length() / 100.0;

	// Calculate new rotation axis as weighted sum
	rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

	// Increase angular speed
	angularSpeed += acc;
}
//! [0]

//! [1]
void TkMapSystem::timerEvent(QTimerEvent *)
{
	// Decrease angular speed (friction)
	angularSpeed *= 0.99;

	// Stop rotation when speed goes below threshold
	if (angularSpeed < 0.01) {
		angularSpeed = 0.0;
	} else {
		// Update rotation
		rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

		// Update scene
		updateGL();
	}
}
//! [1]

void TkMapSystem::initializeGL()
{
	initializeGLFunctions();
	qglClearColor(Qt::black);
	initShaders();
	initTextures();

//! [2]
	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// Enable back face culling
	glEnable(GL_CULL_FACE);
//! [2]

	geometries.init();

	// Use QBasicTimer because its faster than QTimer
	timer.start(12, this);
}

//! [3]
void TkMapSystem::initShaders()
{
	// Override system locale until shaders are compiled
	setlocale(LC_NUMERIC, "C");

	// Compile vertex shader
	if (!program.addShaderFromSourceFile(QGLShader::Vertex, ":/projectwizard/vshader.glsl")) {
		qDebug() << "REESTED VERTEX SHADER";
		close();
	}

	// Compile fragment shader
	if (!program.addShaderFromSourceFile(QGLShader::Fragment, ":/projectwizard/fshader.glsl")) {
				qDebug() << "REESTED FRAGMENT SHADER";
		close();

	}
	// Link shader pipeline
	if (!program.link())
		close();

	// Bind shader pipeline for use
	if (!program.bind())
		close();

	// Restore system locale
	setlocale(LC_ALL, "");
}
//! [3]

//! [4]
void TkMapSystem::initTextures()
{
	// Load cube.png image
	glEnable(GL_TEXTURE_2D);
	texture = bindTexture(QImage(":/projectwizard/cube.png"));
	if(!texture) qDebug() << "REESTED TEXTURE";

	// Set nearest filtering mode for texture minification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Set bilinear filtering mode for texture magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Wrap texture coordinates by repeating
	// f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
//! [4]

//! [5]
void TkMapSystem::resizeGL(int w, int h)
{
	// Set OpenGL viewport to cover whole widget
	glViewport(0, 0, w, h);

	// Calculate aspect ratio
	qreal aspect = qreal(w) / qreal(h ? h : 1);

	// Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
	const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

	// Reset projection
	projection.setToIdentity();

	// Set perspective projection
	projection.perspective(fov, aspect, zNear, zFar);
}
//! [5]

void TkMapSystem::paintGL()
{
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//! [6]
	// Calculate model view transformation
	QMatrix4x4 matrix;
	matrix.translate(0.0, 0.0, -5.0);
	matrix.rotate(rotation);

	// Set modelview-projection matrix
	program.setUniformValue("mvp_matrix", projection * matrix);
//! [6]

	// Use texture unit 0 which contains cube.png
	program.setUniformValue("texture", 0);

	//glBindTexture(GL_TEXTURE_2D, texture);
	// Draw cube geometry
	geometries.drawCubeGeometry(&program);
}


#endif
