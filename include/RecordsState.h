#ifndef RecordsState_H
#define RecordsState_H

#include <Ogre.h>
#include <OIS/OIS.h>
#include <CEGUI.h>

#include "GameState.h"

class RecordsState : public Ogre::Singleton<RecordsState>, public GameState
{
	public:
		RecordsState() {}

		void enter ();
		void exit ();
		void pause ();
		void resume ();

		void keyPressed (const OIS::KeyEvent &e);
		void keyReleased (const OIS::KeyEvent &e);
//		void isKeyDown(OIS::KeyCode key) const;

		void mouseMoved (const OIS::MouseEvent &e);
		void mousePressed (const OIS::MouseEvent &e, OIS::MouseButtonID id);
		void mouseReleased (const OIS::MouseEvent &e, OIS::MouseButtonID id);

		bool frameStarted (const Ogre::FrameEvent& evt);
		bool frameEnded (const Ogre::FrameEvent& evt);

		// Heredados de Ogre::Singleton.
		static RecordsState& getSingleton ();
		static RecordsState* getSingletonPtr ();

	protected:
		Ogre::Root* _root;
		Ogre::SceneManager* _sceneMgr;
		Ogre::Viewport* _viewport;
		Ogre::Camera* _camera;

		bool _exitGame;

		CEGUI::MouseButton convertMouseButton(OIS::MouseButtonID id);
		void showRecordsMsgCegui();
		bool BotonVolver(const CEGUI::EventArgs &e);
		void fillRecordsTable(CEGUI::Window* win);
};

#endif
