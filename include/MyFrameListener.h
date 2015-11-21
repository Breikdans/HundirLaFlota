#include <Ogre.h>
#include <OgreOverlaySystem.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OIS/OIS.h>

#define PLAYER_CELLS 	1 << 0		// = 1. 32 bits de mascara.
#define CPU_CELLS 		1 << 1		// = 2. 32 bits de mascara.

typedef unsigned int uint32;
/**
 * FrameListener se basa en el patrón Observador
 *
 * Antes de representar un frame, Ogre itera sobre todos
 * los FrameListener añadidos, ejecutando el método frameStarted de cada uno de ellos.
 */

class MyFrameListener : public Ogre::FrameListener
{
	private:
		OIS::InputManager* _inputManager;
		OIS::Keyboard* _keyboard;
		OIS::Mouse* _mouse;
		Ogre::RenderWindow* _window;
		Ogre::Camera* _camera;
		Ogre::SceneManager* _sceneManager;
		Ogre::RaySceneQuery *_raySceneQuery;
	public:
		MyFrameListener(Ogre::RenderWindow* win, Ogre::SceneManager *sm, Ogre::Camera *cam);
		~MyFrameListener();
		bool frameStarted(const Ogre::FrameEvent& evt);
		Ogre::Ray setRayQuery(int posx, int posy, uint32 mask);
};
