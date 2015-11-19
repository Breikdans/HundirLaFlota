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
		Ogre::Camera* _camera;
		Ogre::SceneNode* _node;
		Ogre::OverlayManager* _overlayManager;

	public:
		MyFrameListener(Ogre::RenderWindow* win,
						Ogre::Camera* cam,
						Ogre::SceneNode* node);
		~MyFrameListener();
		bool frameStarted(const Ogre::FrameEvent& evt);
		bool frameEnded(const Ogre::FrameEvent& evt);
};
