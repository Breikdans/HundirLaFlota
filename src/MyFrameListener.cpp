#include "MyFrameListener.h"

MyFrameListener::MyFrameListener(Ogre::RenderWindow* win,
                 Ogre::Camera* cam,
                 Ogre::SceneNode *node,
                 Ogre::OverlayManager *om) {
  OIS::ParamList param;
  size_t windowHandle;  std::ostringstream wHandleStr;

  _camera = cam;  _node = node; _overlayManager = om;

  win->getCustomAttribute("WINDOW", &windowHandle);
  wHandleStr << windowHandle;
  param.insert(std::make_pair("WINDOW", wHandleStr.str()));

  _inputManager = OIS::InputManager::createInputSystem(param);
  _keyboard = static_cast<OIS::Keyboard*>
    (_inputManager->createInputObject(OIS::OISKeyboard, false));
  _mouse = static_cast<OIS::Mouse*>
    (_inputManager->createInputObject(OIS::OISMouse, false));
}

MyFrameListener::~MyFrameListener() {
  _inputManager->destroyInputObject(_keyboard);
  _inputManager->destroyInputObject(_mouse);
  OIS::InputManager::destroyInputSystem(_inputManager);
}

bool MyFrameListener::frameStarted(const Ogre::FrameEvent& evt) {
  Ogre::Vector3 vt(0,0,0);     Ogre::Real tSpeed = 20.0;
  Ogre::Real r = 0;
  Ogre::Real deltaT = evt.timeSinceLastFrame;
  int fps = 1.0 / deltaT;

  _keyboard->capture();
  if(_keyboard->isKeyDown(OIS::KC_ESCAPE)) return false;
  if(_keyboard->isKeyDown(OIS::KC_UP))    vt+=Ogre::Vector3(0,0,-1);
  if(_keyboard->isKeyDown(OIS::KC_DOWN))  vt+=Ogre::Vector3(0,0,1);
  if(_keyboard->isKeyDown(OIS::KC_LEFT))  vt+=Ogre::Vector3(-1,0,0);
  if(_keyboard->isKeyDown(OIS::KC_RIGHT)) vt+=Ogre::Vector3(1,0,0);


  _camera->moveRelative(vt * deltaT * tSpeed);
  if (_camera->getPosition().length() < 10.0) {
    _camera->moveRelative(-vt * deltaT * tSpeed);
  }


  if(_keyboard->isKeyDown(OIS::KC_R)) r+=180;
  _node->yaw(Ogre::Degree(r * deltaT));

  _mouse->capture();
  float rotx = _mouse->getMouseState().X.rel * deltaT * -1;
  float roty = _mouse->getMouseState().Y.rel * deltaT * -1;
  _camera->yaw(Ogre::Radian(rotx));
  _camera->pitch(Ogre::Radian(roty));

  Ogre::OverlayElement *oe;
  oe = _overlayManager->getOverlayElement("fpsInfo");
  oe->setCaption(Ogre::StringConverter::toString(fps));
  oe = _overlayManager->getOverlayElement("camPosInfo");
  oe->setCaption(Ogre::StringConverter::toString(_camera->getPosition()));
  oe = _overlayManager->getOverlayElement("camRotInfo");
  oe->setCaption(Ogre::StringConverter::toString(_camera->getDirection()));
  oe = _overlayManager->getOverlayElement("modRotInfo");
  Ogre::Quaternion q = _node->getOrientation();
  oe->setCaption(Ogre::String("RotZ: ") + Ogre::StringConverter::toString(q.getYaw()));

  return true;
}
