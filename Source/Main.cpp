//===========================================================================//
// Main
//	- Main file for testing custom implementations of ARToolKit C++ bindings.
//---------------------------------------------------------------------------//
// AUTHOR: Glen K. Straughn
// DATE: 06/16/2017
// COMPILER: Visual C++
//===========================================================================//

#include <gl/glew.h> // Include this first to prevent conflict between GLEW and GL.h

//======================================================================//
// STL
//======================================================================//
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <iomanip>
#include <cassert>

//======================================================================//
// ARToolkit
//======================================================================//
#include <AR/ar.h>
#include <AR/gsub_lite.h>
#include <AR/Video.h>

//======================================================================//
// GLUT
//======================================================================//
#include <gl/glut.h>

//======================================================================//
// GLM
//======================================================================//
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/detail/_swizzle.hpp>

//======================================================================//
// YAML CPP
//======================================================================//
#include <yaml-cpp/yaml.h>

//======================================================================//
// Custom Libraries & Tools
//======================================================================//
#include "Mesh.hpp"
#include "ARManager.hpp"
#include "Util.hpp"
#include "Object.hpp"
#include "Shaders.hpp"
#include "BackdropManager.hpp"
#include "LuminanceSampler.hpp"
#include "LightEstimator.hpp"
#include "AssetLoading.hpp"


//======================================================================//
// FUNCTION PROTOTYPES
//======================================================================//

void render();
void keyEvent(unsigned char key, int x, int y);
void mainLoop();
void glutVisibility(int visibility);

void updateMarkerObjects();
void cleanUp();
void renderObject(Object &obj);

void plasterCameraFrame(Image* p_cameraFrame);
void sampleSurfaces();

void outputMetaData();

ARPose bestOffsetPose();

// INITIALIZATION FUNCTIONS
bool initialize(const std::string &configFilePath);
bool initGraphics(int argc, char** argv);
bool initARManager(YAML::Node &config);
bool initARGL(YAML::Node &config);
bool initAssets(YAML::Node &config);
bool initShaders(YAML::Node &config);
bool initSampleData(YAML::Node &config);
bool initLightEstimator(YAML::Node &config);

//======================================================================//
// GLOBAL OBJECTS AND VARIABLES
//======================================================================//

ARManager g_arManager;
BackdropManager g_backdrop;
Mesh g_mesh;
Mesh g_debugMesh;
//std::vector<Object*> g_objects;

const int NUMBER_OF_OBJECTS = 12;
Object g_objects[NUMBER_OF_OBJECTS];

ARGL_CONTEXT_SETTINGS_REF gp_arGlSettings;
int g_windowID;

doubleMat4x4 g_perspectiveMatrix;
ARPose g_cameraToWorld;
glm::vec3 g_lightDirection; // Direction of primary light estimation.
GLfloat g_ambientIntensity;
GLfloat g_lightIntensity;
const float SHINY_FACTOR = 10.0;

Shader g_vertexShader;
Shader g_fragmentShader;
ShaderProgram g_shaderProgram;

LightEstimator g_lightEstimator;

std::vector<LuminanceSampler*> g_samplePoints;
float g_sampleAngleCutoff = 0.35f; // Default value = .35 ~= 70 deg.

const int FRAME_RATE = 60;
const float TIME_BETWEEN_RENDERS = 1.0f/FRAME_RATE;
int g_lastRenderTime = 0;
int g_lastSecondStart = 0;
int g_framesInLastSecond = 0;
int g_fps = 0;

std::ofstream g_lightOutFile("lightOutput.dat");

static struct
{
	bool showFrameRate;
	bool showLightVector;
	bool estimateLight;
	bool projectedSampling;
	bool renderObjects;
} g_debugOptions;

//======================================================================//


int main(int argc, char** argv)
{	
	g_debugOptions.showFrameRate = false;
	g_debugOptions.showLightVector = false;
	g_debugOptions.estimateLight = true;
	g_debugOptions.projectedSampling = false;
	g_debugOptions.renderObjects = true;

	if (argc >= 2)
	{
		g_lightDirection = -glm::vec3(0, 0, 1); // Negative because Phong model has L point from surface to light.
		g_ambientIntensity = 0.2f;
		g_lightIntensity = 1.0f;

		YAML::Node config;
		bool ret2go = true;

		if (initGraphics(argc, argv))
		{
			if (initialize(argv[1]))
			{
				// RUN
				glPolygonMode(GL_BACK, GL_LINE);
				g_arManager.start();

				glutMainLoop();
			}
		}
		else
		{
			std::cout << "GLEW failed to initialize." << std::endl;
		}
	}
	//*/

	// DONE
	return 0;
}


//----------------------------------------------------------------------//


void keyEvent(unsigned char key, int x, int y)
{
	int     debug;
	int     thresh;
	ARHandle* arHandle = g_arManager.getARHandlePtr();

	switch (key)
	{
	case 'E': // Toggle light *E*stimation
	case 'e':
		g_lightDirection = glm::vec3(0, 0, -1);
		g_debugOptions.estimateLight = !g_debugOptions.estimateLight;
		break;

	case 'F': // Toggle *F*rame-rate
	case 'f':
		g_debugOptions.showFrameRate = !g_debugOptions.showFrameRate;
		break;

	case 'L': // Output physical light vector
	{
		glm::vec4 physicalLight(g_lightDirection, 0);
		physicalLight = g_cameraToWorld*physicalLight;
		glm::vec3 output = glm::normalize(glm::vec3(physicalLight));

		std::cout << "<";
		for (int i = 0; i < 3; i++)
		{
			std::cout << setprecision(4) << output[i] << "  ";
			g_lightOutFile << setprecision(4) << output[i] << "\t";
		}
		std::cout << "\b\b" << ">" << std::endl;
		g_lightOutFile << std::endl;
		g_lightOutFile.flush();
	}
		break;

	case 'l': // Toggle *l*ight vector output
		g_debugOptions.showLightVector = !g_debugOptions.showLightVector;
		break;

	case 'P': // Toggle *P*rojected sampling
	case 'p':
		g_debugOptions.projectedSampling = !g_debugOptions.projectedSampling;
		std::cout << "Projected sampling ";
		g_debugOptions.projectedSampling ? std::cout << "on." << std::endl : std::cout << "off." << std::endl;
		break;

	case 'Q': // *Q*uerry which markers are detected
	case 'q':
		int curMarker;
		std::cout << "DETECTED MARKERS: ";
		for (int i = 0; i < g_samplePoints.size(); i++)
		{
			curMarker = g_samplePoints[i]->getMarkerID();
			if (g_arManager.getMarkerError(curMarker) != -1)
			{
				std::cout << curMarker << " ";
			}
		}
		std::cout << std::endl;
		break;

	case 'R':
	case 'r':
		g_debugOptions.renderObjects = !g_debugOptions.renderObjects;
		break;

	case 'T':
	case 't':

		break;

	case 'V': // Toggle *V*erbose marker detection
	case 'v':
		g_arManager.toggleVerbose();
		break;

	case '+': // Increase threshold
		thresh = g_arManager.getBaseThreshold() + 5;
		g_arManager.setBaseThreshold(thresh);
		ARLOG("thresh = %d\n", thresh);
		
		break;

	case '-': // Decrease threshold
		thresh = g_arManager.getBaseThreshold() - 5;
		g_arManager.setBaseThreshold(thresh);
		ARLOG("thresh = %d\n", thresh);
		
		break;

	case ' ': // Lock in camera inverse matrix
	{
		ARPose temp = zToYUp(g_arManager.getMarkerPose("world"));
		g_cameraToWorld = glm::inverse(temp);
		if (g_cameraToWorld == ARPose(ZERO_MATRIX_4X4))
		{
			std::cout << "g_cameraToWorld set to zero matrix." << std::endl;
		}
		else
		{
			std::cout << "Camera to World set." << std::endl;
		}
	}
		break;

	case '\n':
	case '\r': // Stupid windows...
		g_lightOutFile << endl;
		std::cout << endl;
		break;
	}
}


//----------------------------------------------------------------------//


void render()
{
	//glBindFramebuffer(GL_DRAW_BUFFER, 0);
	glDrawBuffer(GL_BACK);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	g_backdrop.update(g_arManager.getCameraFramePtr(), g_arManager.getARPixelFormat());
	
	if (g_debugOptions.renderObjects)
	{
		for (int i = 0; i < NUMBER_OF_OBJECTS; i++)
		{
			renderObject(g_objects[i]);
		}
	}

	glutSwapBuffers();
}


//----------------------------------------------------------------------//


void renderObject(Object &obj)
{
	glUseProgram(g_shaderProgram.getHandle());
	
	glm::mat4x4 perspectiveSingle = g_perspectiveMatrix;

	///*
	switch (obj.getUID())
	{//*
	case 0:
		g_shaderProgram.setUniform("u_diffColor", ORANGE);
		break;
	case 1:
		g_shaderProgram.setUniform("u_diffColor", RED);
		break;
	case 2:
		g_shaderProgram.setUniform("u_diffColor", BLUE);
		break;//*/
	default:
		g_shaderProgram.setUniform("u_diffColor", MAGENTA);
	}
	//*/


	//g_shaderProgram.setUniform("u_diffColor", RED);
	g_shaderProgram.setUniform("u_pose", obj.getTransform());
	g_shaderProgram.setUniform("u_perspective", perspectiveSingle);
	g_shaderProgram.setUniform("u_shinyFactor", SHINY_FACTOR);
	g_shaderProgram.setUniform("u_lightDirection", g_lightDirection);
	if (obj.getUID() == NUMBER_OF_OBJECTS - 1)
	{
		g_shaderProgram.setUniform("u_ambientFactor", .75f);
	}
	else
	{
		g_shaderProgram.setUniform("u_ambientFactor", g_ambientIntensity);
	}
	g_shaderProgram.setUniform("u_lightIntensity", g_lightIntensity);
	
	
	Mesh* mesh = obj.getMesh();

	glBindBuffer(GL_ARRAY_BUFFER, mesh->m_verticesBufferID);
	glVertexAttribPointer(g_shaderProgram.getAttributeLocation("a_position"), 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);
	glEnableVertexAttribArray(g_shaderProgram.getAttributeLocation("a_position"));
	
	glBindBuffer(GL_ARRAY_BUFFER, mesh->m_normalsBufferID);
	glVertexAttribPointer(g_shaderProgram.getAttributeLocation("a_normal"), 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);
	glEnableVertexAttribArray(g_shaderProgram.getAttributeLocation("a_normal"));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indicesBufferID);

	glDrawElements(mesh->m_glDrawMode, mesh->m_indices.size(), GL_UNSIGNED_INT, (void*)0);

	glDisableVertexAttribArray(mesh->m_normalsBufferID);
	glDisableVertexAttribArray(mesh->m_verticesBufferID);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glUseProgram(0);
}


//----------------------------------------------------------------------//


void mainLoop()
{
	int time = glutGet(GLUT_ELAPSED_TIME);
	float renderDelta = ((float)(time - g_lastRenderTime) / 1000);

	if (renderDelta > TIME_BETWEEN_RENDERS)
	{
		g_lastRenderTime = time;
		g_framesInLastSecond++;
		
		
		try
		{
			g_arManager.updateCameraFrame();
			g_arManager.updateMarkers();

			if (g_debugOptions.estimateLight)
			{
				sampleSurfaces();
				g_lightDirection = g_lightEstimator.getLightDirection();
				g_lightIntensity = g_lightEstimator.getHighestLuminance();
				g_ambientIntensity = g_lightEstimator.getAmbient();
			}
		}
		catch (Error::ARNullPointerException &ex)
		{
			//std::cout << ex.what() << std::endl;
		}
		//*/
		updateMarkerObjects();
		glutPostRedisplay();
	}
	else
	{
		//std::cout << "No render" << std::endl;
	}

	if (time - g_lastSecondStart >= 1000)
	{
		g_fps = g_framesInLastSecond;
		g_lastSecondStart = time;
		g_framesInLastSecond = 0;

		outputMetaData();
	}
}


//----------------------------------------------------------------------//


void glutVisibility(int visibility)
{
	if (visibility == GLUT_VISIBLE)
	{
		glutIdleFunc(&mainLoop);
	}
	else
	{
		glutIdleFunc(NULL);
	}
}


//----------------------------------------------------------------------//


void updateMarkerObjects()
{
	ARPose m, sum;
	int count = 0;
	int curMarkerID;
	Transform t, t2;
	/*
	for (int i = 0; i < g_samplePoints.size(); i++)
	{		
		curMarkerID = g_samplePoints[i]->getMarkerID();
		
		if (g_arManager.getMarkerError(curMarkerID) != -1)
		{
			sum += g_arManager.getOffsetMarkerPose(curMarkerID);
			count++;
		}
	}

	if (count > 0)
	{
		m = sum / count;
	}
	//*/

	t = bestOffsetPose();//m;
	t.setScale(glm::vec3(1.0));
	g_objects[0].setTransform(t);
	//*/
	///*
	t = g_arManager.getMarkerPose("world");
	g_objects[NUMBER_OF_OBJECTS - 1].setTransform(t);
	/*
	t2 = g_arManager.getMarkerPose(0);
	g_objects[1].setTransform(t2);
	
	t2 = t * glm::mat4x4(g_samplePoints[3]->getFaceOffset());
	g_objects[2].setTransform(t2);
	//*/
}


//----------------------------------------------------------------------//


void sampleSurfaces()
{
	Image& frame = *g_arManager.getCameraFramePtr();
	int curMarkerID;
	float curLuminance;
	ARPose m;
	ARPose dmPose = bestOffsetPose();
	float dotProd;

	for (int i = 0; i < g_samplePoints.size(); i++)
	{
		curMarkerID = g_samplePoints[i]->getMarkerID();

		if (g_arManager.getMarkerError(curMarkerID) != -1)
		{
			m = g_perspectiveMatrix*g_arManager.getMarkerPose(curMarkerID);
			curLuminance = g_samplePoints[i]->getAverageLuminance(m, frame, g_arManager.getARPixelFormat());

			g_lightEstimator.setMarkerLuminance(curMarkerID, curLuminance);
			g_lightEstimator.setMarkerNormal(curMarkerID, m[2]);
		}
		else if (g_debugOptions.projectedSampling)
		{
			m = g_perspectiveMatrix * dmPose * g_samplePoints[i]->getFaceOffset();
			dotProd = glm::dot(glm::normalize(m[2]), glm::tvec4<double>(FORWARD_VECTOR, 0));
			if (dotProd > g_sampleAngleCutoff)
			{
				curLuminance = g_samplePoints[i]->getAverageLuminance(m, frame, g_arManager.getARPixelFormat());
				g_lightEstimator.setMarkerLuminance(curMarkerID, curLuminance);
				g_lightEstimator.setMarkerNormal(curMarkerID, m[2]);
			}
			else
			{
				g_lightEstimator.setMarkerLuminance(curMarkerID, -1);
			}
		}
		else
		{
			g_lightEstimator.setMarkerLuminance(curMarkerID, -1);
		}
	}
}


//----------------------------------------------------------------------//


void outputMetaData()
{
	if (g_debugOptions.showFrameRate)
	{
		std::cout << "FPS: " << g_fps << std::endl;
	}
	if (g_debugOptions.showLightVector)
	{
		glm::vec3 normLight = glm::normalize(g_lightDirection);
		std::cout << "LIGHT DIRECTION: ["
			<< normLight[0] << ", "
			<< normLight[1] << ", "
			<< normLight[2] << "]" << std::endl;
	}
}


//----------------------------------------------------------------------//


ARPose bestOffsetPose()
{
	ARPose answer = ZERO_MATRIX_4X4;
	float bestError = -1;
	float currentError;
	UID currentMarker;
	for (int i = 0; i < g_samplePoints.size(); i++)
	{
		currentMarker = g_samplePoints[i]->getMarkerID();
		currentError = g_arManager.getMarkerError(currentMarker);
		if (bestError < currentError)
		{
			bestError = currentError;
			answer = g_arManager.getOffsetMarkerPose(currentMarker);
		}
	}

	return answer;
}


//----------------------------------------------------------------------//


void cleanUp()
{
	/*
	while (g_objects.size() > 0)
	{
		delete g_objects[g_objects.size() - 1];
		g_objects.pop_back();
	}
	*/
}


//----------------------------------------------------------------------//
// INITIALIZATION FUNCTIONS
//----------------------------------------------------------------------//


bool initGraphics(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	g_windowID = glutCreateWindow("AR Research");
	glutIdleFunc(&mainLoop);
	glutDisplayFunc(&render);
	glutVisibilityFunc(&glutVisibility);
	glutKeyboardFunc(&keyEvent);

	// glewInit
	GLenum error = glewInit();
	
	return (error == GLEW_OK);
}


//----------------------------------------------------------------------//


bool initialize(const std::string &configFilePath)
{
	YAML::Node config;
	try
	{
		config = YAML::LoadFile(configFilePath);
	}
	catch (YAML::BadFile &ex)
	{
		std::cout << ex.what() << std::endl;
		return false;
	}
	catch (YAML::ParserException &ex)
	{
		std::cout << ex.what() << std::endl;
		return false;
	}

	if ( !initARManager(config["AR Manager"]) )
	{
		return false;
	}

	if ( !initARGL(config["Backdrop Manager"]) )
	{
		return false;
	}

	if ( !initShaders(config["Shaders"]) )
	{
		return false;
	}

	if ( !initAssets(config["Assets"]) )
	{
		return false;
	}

	if ( !(initSampleData(config["Light Estimation"])) )
	{
		std::cout << "Couldn't initialize sample points." << std::endl;
		return false;
	}

	if (!initLightEstimator(config["Light Estimation"]))
	{
		std::cout << "Couldn't initialize light estimator." << std::endl;
		return false;
	}

	return true; // If all else fails to fail...
}


//----------------------------------------------------------------------//


bool initARManager(YAML::Node &config)
{
	if (!config)
	{
		return false;
	}

	// NFT MANAGER INITIALIZATIONS
	if (!config["AR Camera Config File"] || !g_arManager.initCamera(config["AR Camera Config File"].as<std::string>()))
	{
		std::cout << "ERROR: Camera NOT initialized." << std::endl;
		return false;
	}

	if (!g_arManager.initManager())
	{
		std::cout << "ERROR: Manager failed to initialize." << std::endl;
		return false;
	}

	if (!config["AR Config File"] || !g_arManager.loadMarkers(config["AR Config File"].as<std::string>()))
	{
		std::cout << "ERROR: Failed to load markers." << std::endl;
		return false;
	}

	if (config["Error Tolerance"])
	{
		g_arManager.setErrorTolerance(config["Error Tolerance"].as<float>());
	}

	if (config["Base Threshold"])
	{
		g_arManager.setBaseThreshold(config["Base Threshold"].as<int>());
	}

	if (config["Multipass"])
	{
		arSetLabelingThreshMode(g_arManager.getARHandlePtr(), AR_LABELING_THRESH_MODE_MANUAL);

		if (config["Multipass"]["Passes"])
		{
			g_arManager.setNumberOfPasses(config["Multipass"]["Passes"].as<int>());
		}
		if (config["Multipass"]["Pass Increment"])
		{
			g_arManager.setPassIncrement(config["Multipass"]["Pass Increment"].as<int>());
		}
	}

	return true;
}


//----------------------------------------------------------------------//


bool initARGL(YAML::Node &config)
{
	if (!config)
	{
		return false;
	}

	if (config["Vertex Shader"] && config["Fragment Shader"])
	{
		if (!g_backdrop.init(config["Vertex Shader"].as<std::string>().c_str(), config["Fragment Shader"].as<std::string>().c_str()))
		{
			std::cout << "ERROR: Failed to initialize backdrop manager." << std::endl;
			return false;
		}
	}
	else
	{
		return false;
	}

	// INITIALIZE ARTOOLKIT STUFF
	gp_arGlSettings = arglSetupForCurrentContext(&(g_arManager.getCameraParamLTPtr()->param), arVideoGetPixelFormat());

	if (gp_arGlSettings == nullptr)
	{
		return false;
	}

	// INITIALIZE PERSPECTIVE MATRIX
	ARdouble perspectiveMat[16];
	arglCameraFrustumRH(&g_arManager.getCameraParamLTPtr()->param, 0.001, 1000.0, perspectiveMat);
	g_perspectiveMatrix = glm::make_mat4x4(perspectiveMat);

	return true;
}


//----------------------------------------------------------------------//


bool initShaders(YAML::Node &config)
{
	if (!config)
	{
		return false;
	}

	if (!config["Vertex Shader"] || !g_vertexShader.compileShaderFromFile(config["Vertex Shader"].as<std::string>().c_str(), GL_VERTEX_SHADER))
	{
		std::cout << "Vertex shader failed to compile." << std::endl;
		return false;
	}

	if (!config["Fragment Shader"] || !g_fragmentShader.compileShaderFromFile(config["Fragment Shader"].as<std::string>().c_str(), GL_FRAGMENT_SHADER))
	{
		std::cout << "Fragment shader failed to compile." << std::endl;
		return false;
	}
	
	if (!g_shaderProgram.link({ &g_vertexShader, &g_fragmentShader }))
	{
		std::cout << "Failed to link shaders." << std::endl;
		return false;
	}

	return true;
}


//----------------------------------------------------------------------//


bool initAssets(YAML::Node &config)
{
	if (!config)
	{
		return false;
	}

	// INITIALIZE MESHES
	bool returnValue = false;
	if (config["Model"])
	{
		returnValue = loadOpenGexMesh(config["Model"].as<std::string>().c_str(), g_mesh);
	}

	if (config["Debug Model"])
	{
		returnValue = returnValue && loadOpenGexMesh(config["Debug Model"].as<std::string>().c_str(), g_debugMesh);
	}

	for (int i = 0; i < NUMBER_OF_OBJECTS; i++)
	{
		g_objects[i].setMesh(&g_mesh);
	}
	// TESTING
	g_objects[NUMBER_OF_OBJECTS - 1].setMesh(&g_debugMesh);

	return returnValue;
}


//----------------------------------------------------------------------//


bool initSampleData(YAML::Node &config)
{
	if (!config)
	{
		return false;
	}

	std::ifstream inFile;
	if (config["Sample Data Config File"])
	{
		inFile.open(config["Sample Data Config File"].as<std::string>());
	}
	else
	{
		return false;
	}

	std::string line, token, remainder;
	const std::string DELIMITER = "\t";
	std::string fileName;
	int markerID;
	LuminanceSampler* p_sampleData = NULL;

	if (!inFile.good())
	{
		return false;
	}


	int index = 0;
	while (inFile.good())
	{
		getline(inFile, line);
		token = tokenize(line, remainder, DELIMITER);
		
		if (index > 12)
		{
			break;
		}

		if (token == "OFFSET:")
		{
			if (p_sampleData != NULL)
			{
				p_sampleData->setFaceOffset(readMatrix(inFile));
			}
			else return false;
		}
		else if (!token.empty())
		{
			markerID = g_arManager.getMarkerPageNumber(token);

			if (markerID < 0)
			{
				return false;
			}
			else
			{
				line = remainder;

				fileName = getFirstRegionBetween(line, "\"", "\"", &remainder);
				if (fileName.empty())
				{
					fileName = tokenize(line, remainder, DELIMITER);
				}

				p_sampleData = new LuminanceSampler(markerID);
				p_sampleData->readSamplePointFile(fileName);
				g_samplePoints.push_back(p_sampleData);

				g_lightEstimator.setMarkerPageNumber(index, markerID);
				index++;
			}
		}
	}

	inFile.close();
	return true;
}


//----------------------------------------------------------------------//


bool initLightEstimator(YAML::Node &config)
{
	if (!(config["Adjacency Matrix File"] && g_lightEstimator.init(config["Adjacency Matrix File"].as<std::string>())))
	{
		return false;
	}

	if (config["Sample Angle Cutoff"])
	{
		g_sampleAngleCutoff = config["Sample Angle Cutoff"].as<float>();
		g_debugOptions.projectedSampling = true;
	}

	if (config["Shadow Threshold"])
	{
		g_lightEstimator.setShadowThreshold(config["Shadow Threshold"].as<float>());
	}

	return true;
}