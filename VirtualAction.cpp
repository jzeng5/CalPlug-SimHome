/*
 * VirtualAction.cpp
 *
 *  Created on: 11/01/2012
 *      Author: narada
 */

#include "VirtualAction.h"

#include <fstream>


namespace VisageSDK {



///////////////////////////////////////
///////////////////////////////////////
////  CODE for VirtualBasicAction  ////
///////////////////////////////////////
///////////////////////////////////////



/**
 *
 * @brief Constructor: init data members and load the basic action with name and path passed as arguments
 *
 * @param path 	path to the FBA file with the action to load
 * @param name 	name of the FBA file with the action to load
 * @param loop 	whether the action has to be played only once, or it has to loop
 * @param vc 	pointer to the VirtualCharacter
 */
VirtualBasicAction::VirtualBasicAction(std::string path, std::string name, bool loop, VirtualCharacter* vc)
{
	playing = false;
	character = vc;
	actionPointer = 0;
	if(!loadBasicAction(path, name, loop))
		throw NaradaException("cannot load action");

}




/**
 *
 * @brief Destructor: deletes the action pointer in case it's not null
 *
 */
VirtualBasicAction::~VirtualBasicAction()
{
	if(actionPointer)
		delete actionPointer;
}



/**
 *
 * @brief reads an FBA file with a basic action, and loads it
 *
 * @param path	path to the FBA file
 * @param name 	name of the FBA file
 * @param loop	whether the action has to be played once, or it has to loop
 *
 * @return true if everithing is right, false if there's some issue reading the action file
 */
bool VirtualBasicAction::loadBasicAction(std::string path, std::string name, bool loop)
{
    fullPath = path + "/" + name;
    std::cout << "full path to load basic action: " << fullPath << std::endl;

    //check whether the file exists or not
    FILE *fp;
	if(!(fp = fopen(fullPath.c_str(),"r")))
		return false;
	fclose(fp);

    // To extend FBA (lipsync) by constant amount
    // This actually delays starting point of voice
    
    // longer strings: larger timeoffset?
    // shorter strings: shorter timeoffset?
    // should NOT set timeoffset to 0.
    
	actionPointer = character->getFAPlayer()->addTrack(const_cast<char*>(fullPath.c_str()), (loop?1:0), 0); // long string
    stopBasicAction();

    return true;
}

/**
 *
 * @brief adjusts the msDelay of action using the size of FBA file
 */
long VirtualBasicAction::adjustDelay(long msDelay)
{
    // If the FBA file originates from a TTS command
    if (fullPath.find("speakTest") != std::string::npos) {
        
        // Open a file to calculate the size of it
        std::ifstream file;
        file.open (fullPath.c_str(), ios::binary );
        file.seekg (0, ios::end);
        
        
        return -(0.8 * file.tellg());
    }
    
    return msDelay;
}

/**
 *
 * @brief plays the BasicAction, starting with the specified delay. If the action is being played,
 * schedules the action, so it will be managed when the current action finished
 *
 * @param msDelay delay to start the action in ms
 *
 * @return true
 */
bool VirtualBasicAction::playBasicAction(long msDelay)
{
    msDelay = adjustDelay(msDelay);
    
	if(!playing)
	{
		character->getFAPlayer()->addTrack(actionPointer);
		character->getFAPlayer()->play();
		character->getFAPlayer()->update();//this updates the timer
		long timeRef = character->getFAPlayer()->getTimer()->getCurrentTime();
		actionPointer->start( timeRef + msDelay);
		playing = true;

	}else
	{
		long delayRef = character->getFAPlayer()->getTimer()->getCurrentTime() + msDelay;
		scheduled.push_back(delayRef);
	}

    return true;
}



/**
 *
 * @brief Stops the BasicAction. If there are scheduled actions, clears the schedule list.
 *  Finally, it removes the track from the character player list.
 *
 * @return true
 *
 */
bool VirtualBasicAction::stopBasicAction()
{
	actionPointer->stop();
	if(!scheduled.size())
		scheduled.clear();
    std::cout << "stopBasicAction" << std::endl;

	character->getFAPlayer()->removeTrack(actionPointer);
	playing = false;
	return true;
}



/**
 *
 * @brief removes the BasicAction FBA file. If there are scheduled actions, clears the schedule list.
 *  It also removes the track from the character player list.
 *
 * @return true if file has been removed, false otherwise
 *
 */
bool VirtualBasicAction::removeBasicActionFile()
{

    if(remove(fullPath.c_str())!=0)
    {
    	std::cout << "cannot remove " << fullPath <<" file, created by lipsync" << std::endl;
    	return false;
    }

	return true;
}



/**
 *
 * @brief Check whether the Basic Action has finished or not. In case the current Basic Action has finished
 *  and there were scheduled more Basic Actions like this one, plays the next in the schedule list, inmediately if
 *  there's no remaining delay, or delayed it has been scheduled with enough delay to wait after this one has finished.
 *
 * @return true if the Basic Action has finished and there are not scheduled actions like this one. False otherwise.
 *
 */
bool VirtualBasicAction::checkFinished()
{
	bool ret = actionPointer->isFinished();
	if(ret)
	{
		actionPointer->stop();
		if(!scheduled.empty())
		{
			long timeRef = character->getFAPlayer()->getTimer()->getCurrentTime();
			long delayRef = scheduled.at(0);
			if((delayRef -timeRef) < 0)
				actionPointer->start(character->getFAPlayer()->getTimer()->getCurrentTime());
			else
				actionPointer->start(character->getFAPlayer()->getTimer()->getCurrentTime() + (delayRef -timeRef));


			character->getFAPlayer()->play();
			scheduled.erase(scheduled.begin());
			return false;
		}
		else
		{
			playing = false;
		}

	}
	return ret;
}



/**
 *
 * @brief returns whether the Basic Action is being played right now or not
 *
 * @return true if it's being played at the moment, false otherwise.
 *
 * @see FbaFileAction::isBeingPlayed()
 *
 */
bool VirtualBasicAction::isBeingPlayed()
{
	if(playing)
		return actionPointer->isBeingPlayed();
	else
		return false;
}






///////////////////////////////////////
///////////////////////////////////////
////     CODE for VirtualAction    ////
///////////////////////////////////////
///////////////////////////////////////


/**
 *
 * @brief Constructor: Creates a VirtualAction, initializes data members and loads all the Basic Actions associated
 *  with this Virtual Action. It also associates the name of the VirtualAction regarding the one specified in the description.
 *
 * @param action animationData struct that contains the info about the kind of animation (bg/fg, loop and AnimKind) and the xml
 *  description of the action (with its BasicActions)
 * @param path path to the FBA file (it does not include the filename)
 * @param vc VirtualCharacter that will be associated with this actions
 *
 * @throw NaradaException in case it find problems loading the actions
 * @see loadActions(), findActionName()
 */
VirtualAction::VirtualAction(animationData* action, std::string path, VirtualCharacter* vc)
{
	srand ( time(NULL) );
	character = vc;
	playInLoop = action->loop;
	playInBackground = action->background;
	kind = action->kind;
	findActionName(action->xmlDescription);
	if(!loadActions(action->xmlDescription, path))
		throw NaradaException("VirtualAction for " + name + " is empty!");
	playing = false;
	beingPlayed = false;

}


/**
 *
 * @brief Constructor: Creates a VirtualAction, from a single basic action, as a foreground action.
 *
 * @param path full path to the FBA file, (without file name)
 * @param actionFileName the name of the action file
 * @param vc VirtualCharacter that will be associated with this actions
 * @param actName the name that will be assigned to the action. By default, uses actionFileName value
 * @param loop whether the animation must be played in loop or not.  By default, uses false
 * @param bg whether the animation must be played in bg or not.  By default, uses false
 * @param actKind whether the animation is FACE animation, BODY animation, or BOTH.  By default, uses NONE
 *
 * @throw NaradaException in case it find problems loading the actions
 * @see VirtualBasicAction::VirtualBasicAction()
 */
VirtualAction::VirtualAction(std::string path, std::string actionFileName, VirtualCharacter* vc, std::string actName, bool loop, bool bg, ActionKind actKind)
{

	if(actName.compare(""))
		name = actName;
	else
		name = actionFileName;
	character = vc;
	playInLoop = loop;
	playInBackground = bg;
	kind = actKind;
	playing = false;
	beingPlayed = false;

	try{
		VirtualBasicAction* vba = new VirtualBasicAction(path, actionFileName, playInLoop, character);
		implementations.push_back(vba);
	}
	catch(NaradaException& e)
	{
		e.debug();
		throw NaradaException("VirtualAction for " + name + " is empty!");
	}

}



/**
 *
 * @brief Destructor: clear the implementations vector
 *
 */
VirtualAction::~VirtualAction()
{
    for(int i=0; i<implementations.size(); i++)
    {
        VirtualBasicAction* actionToDelete = implementations.at(i);
        delete actionToDelete;
    }
	implementations.clear();
}



/**
 *
 * @brief Finds the name of the action inside its XML description
 *
 * @param xmlStr sting with the XML description of the action
 *
 * @return true if it finds the action name
 */
bool VirtualAction::findActionName(std::string xmlStr)
{
	size_t start = xmlStr.find("=") +1;
	size_t end = xmlStr.find(">");
	if(end>start)
	{
		name = xmlStr.substr(start,end-start);
		std::cout << "action name: " << name << std::endl;
		return true;
	}
	else
		std::cout << "no action name found " << std::endl;

	return false;

}



/**
 *
 * @brief parse the XML description of the action to find different implementations of the action (basicActions).
 *  Each time a basicAction is found, its description is included as a new string in the vector that is returned at the end
 *   of the process.
 *
 * @param xmlStr string with the XML description of the action
 *
 * @return a vector of strings. Each string contains the XML description a a basicAction
 *
 */
std::vector<std::string> VirtualAction::parseBasicActions(std::string xmlStr)
{
	std::vector<std::string> result;
	std::string spacedResult;
	size_t start = 0;
	size_t end = 0;

	while(start!= std::string::npos)
	{
		start = xmlStr.find("<basicAction>", start);
		end = xmlStr.find("</basicAction>", start);
		if(start!= std::string::npos)
		{
			start += 13; //adds 13 that is size of <basicAction> text
			spacedResult = xmlStr.substr(start, end-start);
			//lets delete spaces
			spacedResult.erase(remove_if(spacedResult.begin(), spacedResult.end(), ::isspace), spacedResult.end());
			std::cout << "found basic action: " << spacedResult << std::endl;
			result.push_back(spacedResult);
			start = end;
		}
	}
	return result;
}



/**
 *
 * @brief load all the basic actions associated with this action in its implementations vector
 *
 * @param xmlStr string with the XML description of the action
 * @param actionsPath path to the folder where the action files are stored
 *
 * @return true if some basic action has been loaded, false otherwise
 *
 * @see parseBasicActions(), VirtualBasicAction::VirtualBasicAction()
 *
 */
bool VirtualAction::loadActions(std::string xmlStr,  std::string actionsPath)
{
	VirtualBasicAction* vba;
	std::vector<std::string> basicActions = parseBasicActions(xmlStr);

	for(int i=0; i< basicActions.size(); i++)
	{
        NaradaNotificationCenter::postNotification(naradaNotificationLoadProgressReport);
		try{
			vba = new VirtualBasicAction(actionsPath, basicActions.at(i), playInLoop, character);
		}catch(NaradaException& e){e.debug(); continue;}

		implementations.push_back(vba);
	}
	if(!implementations.size())
		return false;

    NaradaNotificationCenter::postNotification(naradaNotificationLoadProgressReport);
	return true;
}



/**
 *
 * @brief remove all the basic actions FILES associated with this action in its implementations vector
 *
 *
 * @return true if all action files have been removed, false otherwise
 *
 * @see VirtualBasicAction::removeBasicActionFile()
 *
 */
bool VirtualAction::removeActionFiles()
{
	bool ret = true;
	for(int i=0; i< implementations.size(); i++)
	{
		if(!implementations.at(i)->removeBasicActionFile())
			ret = false;
	}

	return ret;
}



/**
 *
 * @brief Plays a random implementation (basicAction) of this action, with a certain delay (0 by default).
 *  It looks if the random implementation choosen is already in the schedule list or not, handles this issue, and
 *  anycase, calls the function to play the basicAction choosen, with the desired delay.
 *
 * @param delay ms of delay from the moment this function is played, until the action must be played
 * @return false if there are no implementations for this action, or if the action has no delay and it's already being played. True otherwise
 *
 * @see VirtualBasicAction::playBasicAction()
 *
 */
bool VirtualAction::playAction(long delay)
{
	int implToSchedule = 0;

	if(playing)
	{
		if(delay==0)
		{
			std::cout << "action: " << name << "is being played, cannot add same action with 0 delay" << std::endl;
			return false;
		}
	}
	if(implementations.size()==0)
	{
		std::cout << "no implementations found for action: " << name << std::endl;
		return false;
	}
	else if (implementations.size()>1)
		implToSchedule = rand() % implementations.size();


	bool isInList=false;
	for (int i=0; i<scheduledImplementation.size(); i++)
	{
		if(scheduledImplementation.at(i) == implToSchedule )
			isInList = true;
	}
    

	if(!isInList)
		scheduledImplementation.push_back(implToSchedule);
	std::cout << "scheduling action " << name << " with implem. " << implToSchedule << " delayed(ms): " << delay << std::endl;
	implementations.at(implToSchedule)->playBasicAction(delay);
	playing = true;
	return true;

}



/**
 *
 * @brief stops playing this action completely. All the scheduled implementations of this action are stoped and removed.
 *
 * @return true if the action is being played or is scheduled, false otherwise.
 */
bool VirtualAction::stopAction()
{
	if(playing)
	{
		playing = false;
		while(!scheduledImplementation.empty())
		{
			implementations.at(scheduledImplementation.back())->stopBasicAction();
			scheduledImplementation.pop_back();
		}
		std::cout << "Virtual action "<< name << " has been completely stopped" << std::endl;

		return true;
	}

	return false;

}



/**
 *
 * @brief Check if the action has finished playing at all. If a scheduled implementation of this action has finished,
 *  stops and deletes this implementation. In case there are no implementations being played or scheduled, it calls the stopAction
 *  function.
 *
 * @return false if the action is not being played, or in case it's being played or scheduled. Returns true only once, in the moment
 * that the action was being played and finishes completely, so there are not scheduled implementations.
 *
 * @see VirtualBasicAction::checkFinished(), VirtualBasicAction::isBeingPlayed(), stopAction()
 *
 */
bool VirtualAction::checkFinished()
{
	beingPlayed = false;

	if(playing)
	{
		bool finished = true;
		int schedImplsize = scheduledImplementation.size();

		for(int i =0; i< schedImplsize; i++)
		{
			if(!implementations.at(scheduledImplementation.at(i))->checkFinished())
			{
				if(implementations.at(scheduledImplementation.at(i))->isBeingPlayed())
					beingPlayed = true;

				finished = false;
			}
			else
			{
				std::cout << "sched action "<< name << " with implem. " << i << " has finished" << std::endl;
				implementations.at(scheduledImplementation.at(i))->stopBasicAction();
				scheduledImplementation.erase(scheduledImplementation.begin()+i);
				schedImplsize--;
				i--;
			}
		}

		if(finished)
		{
			playing = false;
			stopAction();
		}
		return finished;
	}

	return false;
}



/**
 *
 * @brief returns whether the action is being played (or scheduled to play), or not.
 *
 * @return true if the action is being played, or is scheduled. False otherwise.
 *
 */
bool VirtualAction::isBeingPlayed()
{
	if(playing)
	{
		return beingPlayed;
	}
	return false;
}

} /* namespace VisageSDK */






