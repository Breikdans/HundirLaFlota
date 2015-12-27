#ifndef __TRACKMANAGERH__
#define __TRACKMANAGERH__

#include <OGRE/Ogre.h>
#include <Track.h>

// Clase encargada de gestionar recursos del tipo "Track".
// Funcionalidad heredada de Ogre::ResourceManager
// y Ogre::Singleton.
class TrackManager: public Ogre::ResourceManager,
                    public Ogre::Singleton<TrackManager>
{
	public:
		TrackManager();
		virtual ~TrackManager();
		virtual TrackPtr load (const Ogre::String& name,
							   const Ogre::String& group = \
							   Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		static TrackManager& getSingleton ();
		static TrackManager* getSingletonPtr ();

	protected:
		Ogre::Resource* createImpl (const Ogre::String& name,
									Ogre::ResourceHandle handle,
									const Ogre::String& group,
									bool isManual,
									Ogre::ManualResourceLoader* loader,
									const Ogre::NameValuePairList* createParams);
};

#endif
