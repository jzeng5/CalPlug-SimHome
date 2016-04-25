/*
 * VirtualAction.h
 *
 *  Created on: 11/01/2012
 *      Author: narada
 */

#ifndef VIRTUALACTION_H_
#define VIRTUALACTION_H_


#ifdef VISAGE_STATIC
	#define VISAGE_DECLSPEC
#else
	#if defined(MAC) || defined(LINUX)
		#define VISAGE_DECLSPEC __attribute__((__visibility__("default")))

	#else
		#ifdef VISAGE_EXPORTS
			#define VISAGE_DECLSPEC __declspec(dllexport)
		#else
			#define VISAGE_DECLSPEC __declspec(dllimport)
		#endif
	#endif

#endif


#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include "FbaFileAction.h"
#include "NaradaException.h"
#include <time.h>


enum ActionKind {FACE, BODY, BOTH, NONE};

struct animationData
{
	std::string xmlDescription;
	bool background;
	bool loop;
	ActionKind kind;

};


#include "VirtualCharacter.hpp"



namespace VisageSDK {



class VirtualCharacter;


/**
* @class VirtualBasicAction
* @brief Class that simplifies basic usage of low level actions
*
* @see VirtualAction
*
*/
class VISAGE_DECLSPEC VirtualBasicAction
{
public:

	VirtualBasicAction(std::string path, std::string name, bool loop, VirtualCharacter* vc);
	virtual ~VirtualBasicAction();

	bool playBasicAction(long msDelay);
	bool stopBasicAction();
	bool removeBasicActionFile();
	bool checkFinished();
	bool isBeingPlayed();


private:
	bool loadBasicAction(std::string path, std::string name, bool loop);
	VirtualCharacter *character;
	FbaFileAction *actionPointer;
	std::vector<long> scheduled;
	std::string fullPath;
    long adjustDelay(long msDelay);

	bool playing;
};





/**
* @class VirtualAction
* @brief Class that simplifies basic usage of actions. Allows to associate different implementations
* with the same action.
*
* @see VirtualActionManager, VirtualBasicAction
*
*/
class VISAGE_DECLSPEC VirtualAction
{
public:

	VirtualAction(animationData* action, std::string path, VirtualCharacter* vc);
	VirtualAction(std::string path, std::string actionFileName, VirtualCharacter* vc, std::string actName="", bool loop=false, bool bg=false, ActionKind actKind = NONE);


	virtual ~VirtualAction();

	//bool loadAction();
	bool playAction(long msDelay=0);
	bool stopAction();
	bool removeActionFiles();
	bool checkFinished();
	bool isPlaying(){return playing;}
	bool isBeingPlayed();
	bool isInBackground(){return playInBackground;}
	bool isActionName(std::string actName)
	{
		if(name.compare(actName) == 0)
			return true;
		else
			return false;
	}
	std::string getActionName(){return name;}
	ActionKind getKind(){return kind;}


private:

	bool findActionName(std::string xmlStr);
	bool loadActions(std::string xmlStr,  std::string actionsPath);

	std::vector<std::string> parseBasicActions(std::string xmlStr);
	std::string name;
	std::vector<int> scheduledImplementation;
	std::vector<VirtualBasicAction *> implementations;
	bool playing;
	bool beingPlayed;
	VirtualCharacter *character;
	bool playInBackground;
	bool playInLoop;
	bool bodyBackground;
	ActionKind kind;

};

} /* namespace VisageSDK */
#endif /* VIRTUALACTION_H_ */
