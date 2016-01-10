#ifndef EndGameState_H
#define EndGameState_H

#include <Ogre.h>
#include <OIS/OIS.h>
#include <CEGUI.h>

#include "GameState.h"

class EndGameState : public Ogre::Singleton<EndGameState>, public GameState
{
	public:
		EndGameState() {}

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
		static EndGameState& getSingleton ();
		static EndGameState* getSingletonPtr ();

	protected:
		Ogre::Root* _root;
		Ogre::SceneManager* _sceneMgr;
		Ogre::Viewport* _viewport;
		Ogre::Camera* _camera;
		CEGUI::Window* _ceguiSheet;
		CEGUI::Window* _newRecordWin;

		bool _exitGame;

		CEGUI::MouseButton convertMouseButton(OIS::MouseButtonID id);
		void showEndMsgCegui();
		bool BotonOk(const CEGUI::EventArgs &e);
		bool isNewRecord(unsigned int puntos);
		bool BotonAceptar(const CEGUI::EventArgs &e);
		void showEnterRecordName();
		void saveRecords();
};

#endif
