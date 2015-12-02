#include <Ogre.h>
#include "Grid.h"
#include "MyFrameListener.h"

class MyApp
{
	private:
		Ogre::SceneManager* _sceneManager;		// gestor de escena
		Ogre::Root* _root;						// nodo root de Ogre
		MyFrameListener* _framelistener;		// gestion listener frames
		Ogre::OverlayManager* _overlayManager;	// gestor de Overlays
		Grid CPUGrid;
		Grid PlayerGrid;

	public:
		MyApp();
		~MyApp();
		int start();
		void loadResources();
		void createScene();
		void createOverlay();
		void ActualizaTablero(Ogre::Node* node, usint16 F, usint16 C);

};
