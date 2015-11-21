#include <Ogre.h>
#include <OgreOverlaySystem.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OIS/OIS.h>

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
	public:
		MyFrameListener(Ogre::RenderWindow* win);
		~MyFrameListener();
		bool frameStarted(const Ogre::FrameEvent& evt);
};
