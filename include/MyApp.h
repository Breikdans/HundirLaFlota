#include <Ogre.h>
#include "MyFrameListener.h"

class MyApp
{
	private:
		Ogre::SceneManager* _sceneManager;
		Ogre::Root* _root;
		MyFrameListener* _framelistener;

	public:
		MyApp();
		~MyApp();
		int start();
		void loadResources();
		void createScene();
};
