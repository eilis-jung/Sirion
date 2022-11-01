#pragma once

#include <core/core.h>

namespace Sirion {
	struct PhysicsConfig {

	};
	struct SimulationConfig {

	};
	struct FluidSolverConfig {

	};
	struct ExporterConfig {

	};
	class Application {
	private:
		std::shared_ptr<PhysicsConfig> m_physicsConfig;
		std::shared_ptr<SimulationConfig> m_simulationConfig;
		std::shared_ptr<FluidSolverConfig> m_fluidSolverConfig;
		int m_logTimeStep;
		std::string m_configFilename;

		std::shared_ptr<ExporterConfig> m_exporterConfig;
		std::shared_ptr<Renderer> m_renderer;
		std::shared_ptr<PhysicsSystem> m_physicsSystem;
		std::shared_ptr<ParticleSystem> m_particleSystem;
	public:
		Application(int argc, char** argv, TiXmlElement* pChimeraConfig) {
			m_pMainNode = pChimeraConfig;
			m_configFilename = pChimeraConfig->GetDocument()->Value();
			m_pRenderer = NULL;
			m_pPhysicsCore = NULL;
			m_pMainSimCfg = NULL;
			m_pFlowSolverParams = NULL;
			m_logTimeStep = -1;
			m_pPhysicsParams = new PhysicsCore<Vector2>::params_t();


			//m_pDataLogger = NULL;
		}

		/************************************************************************/
		/* Functionalities                                                      */
		/************************************************************************/
		virtual void draw() = 0;
		virtual void update() = 0;


		/************************************************************************/
		/* Callbacks                                                            */
		/************************************************************************/
		virtual void mouseCallback(int button, int state, int x, int y) {
			m_pRenderer->mouseCallback(button, state, x, y);
		}

		virtual void motionCallback(int x, int y) {
			m_pRenderer->motionCallback(x, y);
		}

		virtual void keyboardCallback(unsigned char key, int x, int y) {
			m_pRenderer->keyboardCallback(key, x, y);
		}

		virtual void keyboardUpCallback(unsigned char key, int x, int y) {

		}

		virtual void specialKeyboardCallback(int key, int x, int y) {
			m_pRenderer->keyboardCallback(key, x, y);
		}

		virtual void specialKeyboardUpCallback(int key, int x, int y) {
			m_pRenderer->keyboardCallback(key, x, y);
		}

		virtual void reshapeCallback(int width, int height) {
			m_pRenderer->reshapeCallback(width, height);
		}

		virtual void exitCallback() {

		}

		/************************************************************************/
		/* Access functions                                                     */
		/************************************************************************/
		GLRenderer2D* getRenderer() const {
			return m_pRenderer;
		}

		PhysicsCore<Vector2>* getPhysicsCore() const {
			return m_pPhysicsCore;
		}

	protected:

		/************************************************************************/
		/* Functionalities                                                      */
		/************************************************************************/
		void initializeGL(int argc, char** argv) {
			glutInit(&argc, argv);
			int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
			int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
			glutInitWindowSize(screenWidth, screenHeight);
			glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
			glutCreateWindow("Fluids Renderer");
			GLenum err = glewInit();
			if (GLEW_OK != err) {
				Logger::get() << "GLEW initialization error! " << endl;
				exit(1);
			}

			/**GLUT and GLEW initialization */
			const char* GLVersion = (const char*)glGetString(GL_VERSION);
			Logger::get() << "OpenGL version: " << GLVersion << endl;
		}

		/************************************************************************/
		/* General loaders                                                      */
		/************************************************************************/
		plataform_t loadPlataform(TiXmlElement* pNode);
		void loadSimulationParams();
		void loadConvectionMethodParams(TiXmlElement* pConvectionNode);
		void loadParticleBasedParams(TiXmlElement* pParticleBasedNode);
		void loadPoissonSolverParams(TiXmlElement* pSolverParamsNode);
		void loadProjectionMethodParams(TiXmlElement* pProjectionNode);
		void loadFarFieldParams(TiXmlElement* pFarFieldNode);
		void loadIntegrationMethodParams(TiXmlElement* pFarFieldNode);
		void loadSolidWallConditions(TiXmlElement* pSolidWallNode);

		void loadParticleSystem(TiXmlElement* pParticleSystemNode, QuadGrid* pQuadGrid = NULL);
		void loadDensityField(TiXmlElement* pDensityNode, QuadGrid* pGrid);
		vector<FlowSolver<Vector2, Array2D>::rotationalVelocity_t> loadRotationalVelocityField(TiXmlElement* pRotationalVelocityNode);

		/** Class based loaders */
		QuadGrid* loadGrid(TiXmlElement* pGridNode);

		template <class valueType>
		Interpolant<valueType, Array2D, Vector2>* loadInterpolant(TiXmlElement* pInterpolantType, const Array2D<valueType>& values, Scalar gridDx) {
			TiXmlElement* pTempNode = NULL;
			if (pTempNode = pInterpolantType->FirstChildElement("Type")) {
				string interpolantType = pTempNode->GetText();
				transform(interpolantType.begin(), interpolantType.end(), interpolantType.begin(), tolower);
				if (interpolantType == "bilinearstaggered") {
					return new BilinearStaggeredInterpolant2D<valueType>(values, gridDx);
				}
				else if (interpolantType == "cubicstreamfunction") {
					return new CubicStreamfunctionInterpolant2D<valueType>(values, gridDx);
				}
				else if (interpolantType == "subdivisionstreamfunction") {
					return new SubdivisionStreamfunctionInterpolant2D<valueType>(values, gridDx, gridDx / 4);
				}
				else if (interpolantType == "bilinearstreamfunction") {
					return new BilinearStreamfunctionInterpolant2D<valueType>(values, gridDx);
				}
			}
			return nullptr;
		}

		/************************************************************************/
		/* Specific loaders                                                     */
		/************************************************************************/
		void loadMultigridParams(TiXmlElement* pMultigridNode);
		void loadLoggingParams(TiXmlElement* pLoggingNode);



	};
}