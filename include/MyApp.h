#include <Ogre.h>
#include "Grid.h"
#include "MyFrameListener.h"

class MyApp
{
	private:
		Ogre::SceneManager* _sceneManager;	// gestor de escena
		Ogre::Root* _root;					// nodo root de Ogre
		MyFrameListener* _framelistener;	// gestion listener frames
		Grid CPUGrid;
		Grid PlayerGrid;

	public:
		MyApp();
		~MyApp();
		int start();
		void loadResources();
		void createScene();
		void ActualizaTablero(usint16 F, usint16 C);
};
