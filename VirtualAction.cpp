//
//  ModelManager.mm
//  BasicCharacter
//
//  Created by Enrique Oriol on 28/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "ModelManager.h"

#include <cmath>
#include <string>
#include <vector>
//OLD API
#import "LipSync.h"

#import "ES1Renderer.h"

#include "VirtualCharacter.hpp"
#include "OGLViewer.h"

#include <CoreGraphics/CGImage.h>

#import <AVFoundation/AVFoundation.h>
#import <AVFoundation/AVAssetWriterInput.h>
#import <CoreVideo/CVPixelBuffer.h>
#import <UIKit/UIKit.h>

#import <AudioToolbox/AudioToolbox.h>

#import "NaradaNotificationCenter.hpp"



using std::string;
using namespace VisageSDK;


VirtualCharacter *character;
OGLViewer *glViewer;





const NSString* pathToDocuments = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
const NSString* aacPath = [pathToDocuments stringByAppendingPathComponent:@"output.aac"];
const NSString* videoOnlyPath = [pathToDocuments stringByAppendingPathComponent:@"TestVideo.mp4"];
const NSString* videoMixedPath = [pathToDocuments stringByAppendingPathComponent:@"My Virtual Assistant.mp4"];



@implementation ModelManager

@synthesize isExporting;
@synthesize isPlayingRecorded;
@synthesize isTrackingRotAndPos;
@synthesize isPlayingTrackedRotAndPos;

@synthesize changeBackgroundPath;


@synthesize frame_count;
@synthesize margin_beats;
@synthesize nFrames;

@synthesize duration;
@synthesize hideCounter;
@synthesize startSession;

@synthesize width;
@synthesize height;
@synthesize loadingPercentage;

@synthesize modelName = _modelName;
@synthesize background = _modelBacground;



void modelHasFinishedSpeaking()
{
    [[NSNotificationCenter defaultCenter] postNotificationName:notificationFinishedSpeaking object:nil];
    std::cout << "Narada Notification: finished Speaking" << std::endl;
}

void modelHasFinishedTemporaryAction()
{
    [[NSNotificationCenter defaultCenter] postNotificationName:notificationFinishedSpeaking object:nil];
    std::cout << "Narada Notification: finished temporary action" << std::endl;    
}

void modelLoadMonitoring(int percentage)
{
    std::cout << "Loading model, percentage: " << percentage << "/100" <<std::endl;
    
    NSDictionary* dict = [NSDictionary dictionaryWithObject: [NSNumber numberWithFloat:((float)percentage)/100] forKey:@"percentage"];
    [[NSNotificationCenter defaultCenter] postNotificationName:notificationModifiedLoadingBar object:nil userInfo:dict];
    

}



-(ModelManager *)initWithName:(NSString*)name Background:(NSString*)background Width:(int)theWidth Height:(int)theHeight
{
    if (self = [super init]) {
        
        hideCounter = 0;
        self.modelName = name;
        self.background = background;
        
        self.width = theWidth;
        self.height = theHeight;
        changeBackground = NO;
        
        animateRotation = NO;
        animateTranslation = NO;
        
        
        self.changeBackgroundPath = Nil;
        isExporting = false;
        isTrackingRotAndPos = FALSE;
        isPlayingTrackedRotAndPos = FALSE;
        recording_counter = 0;
        
        loadingPercentage = 0;
        frame_count = 0;
        margin_beats = 0;
        nFrames = 0;
        duration = 0.0;
        trackingPosition = 0;
        hideCounter = -1;
        startSession = true;
        rotTrackFrameRateAdjust = 0;
        
    }
    
    return self;
}
 
    
-(void)initModelWithViewWidth:(int)theWidth andHeight:(int)theHeight
{
    
//    CGRect screenBounds = [[UIScreen mainScreen] bounds];
//    CGFloat screenScale = [[UIScreen mainScreen] scale];
//    CGSize screenSize = CGSizeMake(screenBounds.size.width * screenScale, screenBounds.size.height * screenScale);
    
    self.width = theWidth;
    self.height = theHeight;
    isExporting = false;
    isTrackingRotAndPos = FALSE;
    isPlayingTrackedRotAndPos = FALSE;
    recording_counter = 0;
    
    animateRotation = NO;
    animateTranslation = NO;
    
    loadingPercentage = 0;
    frame_count = 0;
    margin_beats = 0;
    nFrames = 0;
    duration = 0.0;
    trackingPosition = 0;
    hideCounter = -1;
    startSession = true;
    rotTrackFrameRateAdjust = 0;
    
    
    [self initCharacter];
}
    


-(void)initCharacter
{
    [self initCharacterWithPerspective:FALSE];
}


-(void) initCharacterWithPerspective:(BOOL)persp
{
    bool perspective = false;
    if(persp)
        perspective = true;

    if(sizeof(self.modelName)>0)
    {
        //Assign handler to NaradaNotificationCenter
        NaradaNotificationCenter::subscribeNotification(VirtualCharacter::naradaNotificationJustFinishedSpeaking, &modelHasFinishedSpeaking);
        NaradaNotificationCenter::subscribeNotification(VirtualCharacter::naradaNotificationJustFinishedTemporaryAction, &modelHasFinishedTemporaryAction);
        NaradaNotificationCenter::subscribeNotification(VirtualCharacter::naradaNotificationLoadingProgressPercentage, &modelLoadMonitoring);
        
        
        glViewer = new OGLViewer(width, height, string([self.background UTF8String]), perspective);
        char *filename = [self getPathFromNSStringTo:self.modelName];
        character = glViewer->loadCharacter(filename);

    //    //in case using perspective
    //    character->zoomToFitWidthPercentage(0.7);
    //    [self moveModelX:0 y:height*0.6 absolute:NO];
        
        character->playBodyBackground();
        character->playFaceBackground();
        character->setSpeakRealTime(false);
        
    }
    else
        NSLog(@"Model name has not been specified");
    
}




- (BOOL)isModelLoaded
{
    if(character)
        return TRUE;
    else
        return FALSE;
}



- (void)dealloc
{
    if(character)
        delete character;
    if(glViewer)
        delete glViewer;

    NaradaNotificationCenter::unsubscribeNotification(VirtualCharacter::naradaNotificationJustFinishedSpeaking, &modelHasFinishedSpeaking);
    NaradaNotificationCenter::unsubscribeNotification(VirtualCharacter::naradaNotificationJustFinishedTemporaryAction, &modelHasFinishedTemporaryAction);
    NaradaNotificationCenter::unsubscribeNotification(VirtualCharacter::naradaNotificationLoadingProgressPercentage, &modelLoadMonitoring);
    
    [super dealloc];
}



- (void) manageCover
{
    if (hideCounter == 10){
		[[NSNotificationCenter defaultCenter] postNotificationName:notificationJustFinishedCoverProtectionTime object:nil];
		hideCounter = -1;
	}
	else if (hideCounter >= 0)
		hideCounter++;
}


//- (void) manageVideoRecording
//{
//    if ([self isModelLoaded]) {
//		if ([self isExporting]) {
//			hasStartedRecording = true;
//			if (startSession) {				
//				videoRecorder = [[NaradaVideoRecorder alloc] initWithOutPath:videoOnlyPath 
//																	   width:[self width] 
//																	  height:[self height] 
//																videoBitRate:0 
//																andFrameRate:[self getFrameRate]];
//				[videoRecorder startSession];
//				startSession = false;
//				cout << "Coding Image " << endl;
//                
//				nFrames = [self getNFrames] + TIME_MARGIN * [self getFrameRate];
//				cout << "N Frames: " << nFrames << endl;
//                
//                loadingPercentage = 0.0;
//                [[NSNotificationCenter defaultCenter] postNotificationName:notificationModifiedLoadingBar object:nil];
//			}
//			else {
//				[videoRecorder codeFrame];
//				frame_count ++;
//				loadingPercentage = 100.0 * frame_count / nFrames;
//				[[NSNotificationCenter defaultCenter] postNotificationName:notificationModifiedLoadingBar object:nil];
//			}
//		}
//		
//		else if (hasStartedRecording) {
//			//cout << "Frame:-" << frame_count << " ";
//			if (margin_beats <= TIME_MARGIN * [self getFrameRate]) {
//				margin_beats++;
//				[videoRecorder codeFrame];
//				frame_count ++;
//				loadingPercentage = 100.0 * frame_count / nFrames;
//				[[NSNotificationCenter defaultCenter] postNotificationName:notificationModifiedLoadingBar object:nil];
//			}
//			else hasStartedRecording = false;
//		}
//		else if (frame_count != 0) {
//			[self restoreDefaultFrameRate];
//			//cshell->rotateToInit();
//			
//			[videoRecorder EndSession];
//			[videoRecorder release];
//			frame_count = 0;
//			margin_beats = 0;
//			startSession = true;
//			
////PENDIENTE!!!!!!!!            
////            if (cshell->getDoPitchShift()) {
////                wavPath= [pathToDocuments stringByAppendingPathComponent:[NSString stringWithFormat:@"%@",_DEFAULT_HIGHPITCH_AUDIO_FILENAME]];
////            } else{
////                wavPath= [pathToDocuments stringByAppendingPathComponent:[NSString stringWithFormat:@"%@",_DEFAULT_NORMALPITCH_AUDIO_FILENAME]];
////            }
//			self.converter = [[[NaradaAudioConverter alloc] initWithSourcePath:wavPath 
//                                                            andDestinationPath:aacPath 
//                                                                 andSampleRate:0] retain];
//			
//			[converter convertWavtoAAC];
//			
//			[converter release];
//			
//			cout << "Video Only path: " << [videoOnlyPath cStringUsingEncoding:1] << endl;
//			cout << "Video Mixed path: " << [videoMixedPath cStringUsingEncoding:1] << endl;
//			
//			self.mixer = [[[NaradaAVMixer alloc] initWithPathsInAudio:aacPath 
//															  inVideo:videoOnlyPath 
//														     outVideo:videoMixedPath 
//														  andDelegate:self] retain];
//            
//			[mixer mixAudioVideo];
//			
//			[mixer release];
//            
//		}
//        
//	}
//}


-(void)manageRecordingCounter
{
    //The notifications about recording label, are sent once a second
    int frameRate = [self getFrameRate];
    
    if (recording_counter < 0) {
		[[NSNotificationCenter defaultCenter] postNotificationName:notificationEmptyRecordingAudioLabel object:nil];
	}
    else if (recording_counter == 0) {
		[[NSNotificationCenter defaultCenter] postNotificationName:notificationSetRecordingAudioLabel1 object:nil];
	}
	else if (recording_counter == frameRate) {
        [[NSNotificationCenter defaultCenter] postNotificationName:notificationSetRecordingAudioLabel2 object:nil];
	}
	else if (recording_counter == frameRate * 2) {
        [[NSNotificationCenter defaultCenter] postNotificationName:notificationSetRecordingAudioLabel3 object:nil];
	}
    
    recording_counter++;
    if(recording_counter == frameRate*3)
        recording_counter = 0;
}




- (void)render
{
    if(changeBackground)
    {
        glViewer->changeBackgroundImage(self.changeBackgroundPath);
        changeBackground = NO;
    }

    if (shouldDoTranslate)
    {
        shouldDoTranslate=NO;
        if(shouldWaitToTranslate)
        {
            shouldWaitToTranslate=NO;
            shouldDoTranslate=YES;
        }else{
            [self moveToSavedTranslation];
        }
    }
    
    std::vector<std::string>finishedActions = glViewer->updateScene();
    
    if(!finishedActions.empty())
    {
        std::cout << "finished action: " << finishedActions.at(0) << std::endl;
        isPlayingRecorded = false;
        [[NSNotificationCenter defaultCenter] postNotificationName:notificationFinishedSpeaking object:nil];
    }
    
    [self manageAnimateTranslation];
    
    [self manageAnimateRotation];
    
    glViewer->RenderToScreen();
    
    [self checkFinishedSpeaking];
	
    [self manageRotAndPos];
    
//    std::cout << "width X height model: " << character->getModelWidth() << " X " << character->getModelHeight() << std::endl;
//    std::cout << "center of the model: " << character->getCenter().x << " , " << character->getCenter().y << " , " << character->getCenter().z << std::endl;
//    std::cout << "limits: " << character->getPerspectiveLimitInX() << " , " << character->getPerspectiveLimitInY() << std::endl;
    
//    if(isRecording)
//    {
//        if(my_recorder->hasExceededMaxSize())
//            [[NSNotificationCenter defaultCenter] postNotificationName:notificationRecordingHasExceedSize object:nil];
//    }
    
}


- (void) resizeGLtoFitWidth:(int)thewidth andHeight:(int)theHeight
{
    self.width = thewidth;
    self.height = theHeight;
    glViewer->resizeGL(thewidth, theHeight);
    
}


- (void)playAction:(NSString*) act
{
    character->playAction(string([act UTF8String]));
}

- (void)playTemporaryAction:(NSString*) act withPath:(NSString*) path andDelay:(long) delay
{
    if([[act pathExtension] isEqualToString:@"wav"]) {
        act = [act stringByReplacingOccurrencesOfString:@".wav" withString:@".FBA"];
    }
    
    character->playTemporaryAction(string([path UTF8String]), string([act UTF8String]), delay);
}



- (void)repeatAudio:(NSString*) act
{
    [self performSelectorOnMainThread:@selector(repeatOnMainThread:) withObject:[self getNSPathFromNSStringTo:act] waitUntilDone:NO];
}



- (void)repeatAudioWithAbsoluteAudioPath:(NSString*)wavFilePath temporary:(BOOL) temp
{
    [self performSelectorOnMainThread:@selector(repeatTemporaryOnMainThread:) withObject:wavFilePath waitUntilDone:NO];
}



- (void)repeatOnMainThread:(NSString*)wavFilePath
{
    if (!character->repeat(std::string([wavFilePath UTF8String]))) {
        NSString* locdesc = NSLocalizedString(@"CONVERSION_ERROR", nil);
        NSMutableDictionary *details = [NSMutableDictionary dictionary];
        [details setValue:locdesc forKey:NSLocalizedDescriptionKey];
        NSError * error = [NSError errorWithDomain:@"tts" code:404 userInfo:details];
        
        NSDictionary *userInfo = [NSDictionary dictionaryWithObject:error forKey:@"error"];
        [[NSNotificationCenter defaultCenter] postNotificationName:notificationDidFailSpeaking object:nil userInfo:userInfo];
    }
}

- (void)repeatTemporaryOnMainThread:(NSString*)wavFilePath
{
    if(!character->repeat(std::string([wavFilePath UTF8String]), true)) {
        NSString* locdesc = NSLocalizedString(@"CONVERSION_ERROR", nil);
        NSMutableDictionary *details = [NSMutableDictionary dictionary];
        [details setValue:locdesc forKey:NSLocalizedDescriptionKey];
        NSError * error = [NSError errorWithDomain:@"tts" code:404 userInfo:details];
        
        NSDictionary *userInfo = [NSDictionary dictionaryWithObject:error forKey:@"error"];
        [[NSNotificationCenter defaultCenter] postNotificationName:notificationDidFailSpeaking object:nil userInfo:userInfo];
    }
 }



- (void)stopAction:(NSString *)actionName
{
    character->stopAction(string([actionName UTF8String]));
}

- (void)stopForegroundActions
{
    character->stopActions();
}


- (void)checkFinishedSpeaking
{
//    if(isPlayingRecorded)
//    {
//        if(character->justFinishedSpeaking())
//        {
//            isPlayingRecorded = false;
//            [[NSNotificationCenter defaultCenter] postNotificationName:notificationFinishedSpeaking object:nil];
//        }
//    }
}



-(BOOL)quitApplication
{
	glViewer->Close();
	
	if(glViewer)
		delete glViewer;
	
	return TRUE;
}


-(void)startHideCounter
{
    	hideCounter = 0;
}


-(void)startTrackingRotAndPos
{
    isTrackingRotAndPos = TRUE;
}

-(void)stopTrackingRotAndPos
{
    isTrackingRotAndPos = FALSE;   
}

-(void)startPlayingTrackedRotAndPos
{
    isPlayingTrackedRotAndPos = TRUE;
}

-(void)stopPlayingTrackedRotAndPos
{
    isPlayingTrackedRotAndPos = FALSE;  
    trackingPosition = 0;
    [[NSNotificationCenter defaultCenter] postNotificationName:notificationJustFinishedPlayingTrackedRotAndPos object:nil]  ;
}

- (bool)playRecMovie
{
    [self playAction:@"playRecMovie"];//ESTO HAY QUE REVISARLO
    isExporting = true;
    return true;
}

- (bool)playRecAudio
{
/*PLAY ANIMATION WITH RECORDED AUDIO*/
//                if ([self getDoPitchShift]) {
//                    cshell->playRecordedAudioInHighPitch();
//                }else{
//                    cshell->playRecordedAudio();
//                }
    [self playAction:@"welcome"];//ESTO HAY QUE REVISARLO
    isPlayingRecorded = true;

    return true;
}

- (void)shutUp
{
    character->shutUp();
}



#pragma mark -
#pragma mark FrameRate functions

- (float)getRealFrameRate
{
    return glViewer->getRealFrameRate();
}

- (int)getFrameRate
{
	return character->getFAPlayer()->getTimer()->getFrameRate();
}


- (int)getNFrames: (NSString*) str
{
//	return character->getNFramesFromAction(int([str UTF8String]));
    return 0;
}


- (void) restoreDefaultFrameRate
{
	character->getFAPlayer()->getTimer()->setFrameRate(defaultFrameRate);
	character->getFAPlayer()->setPlayMode(PLAYMODE_REALTIME);
}





#pragma mark -
#pragma mark Background functions



-(void)changeBackgroundImageFromResource:(NSString*) imageFile
{
    changeBackgroundPath = imageFile;
    changeBackground = YES;
}



-(void)setDefaultBackground
{
    glViewer->setDefaultBackground();
}



-(void)makeMovieFinishedWithError:(NSError *)error
{
	cout << "Make Movie finished, call the controller AviController..." << endl;
        
    //deberia hacer algo tipo
    //[[NSNotificationCenter defaultCenter] postNotificationName:notificationMovieHasFinished object:nil];
    
    isExporting = false;
	//[self setMenuState:MENU_INIT];
}



#pragma mark -
#pragma mark Audio functions

-(void)setSpeakRealTime:(bool)realTime
{
    character->setSpeakRealTime(realTime);
}

-(void)setDoPitchShift:(bool) shouldDo
{
//    if(my_recorder) my_recorder->setDoHigherPitch(shouldDo);
    _shouldDoHigherPitch=shouldDo;
}


-(void)getDoPitchShift
{
//    return my_recorder ?  my_recorder->getDoHigherPitch() : _shouldDoHigherPitch;
}





- (BOOL)calculateAudioDuration 
{
	NSString *mainPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
	NSString *audioPath = [mainPath stringByAppendingPathComponent:_DEFAULT_NORMALPITCH_AUDIO_FILENAME];
	
	if (![[[NSFileManager alloc] init] fileExistsAtPath:audioPath]) {
		cout << "Audio File 'test.wav' does not Exist" << endl;
		return false;
	}
	
	NSURL *audioURL = [[NSURL alloc] initFileURLWithPath:audioPath];
	AVURLAsset *audioAsset = [[AVURLAsset alloc ] initWithURL:audioURL options:nil];
	
	duration = CMTimeGetSeconds(audioAsset.duration);
	
	cout << "Duration of audio: " << duration << endl;
	
	return true;
}





#pragma mark -
#pragma mark Model Rotation and Motion

-(void)rotateToInit
{
    [self rotateModelX:kInitPitch y:kInitYaw absolute:true];
}

-(void)rotateToTrackedPosition:(int)index
{
    float rotX = [self getTrackRotX: index];
    float rotY = [self getTrackRotY: index];        

    [self rotateModelX:rotX y:rotY absolute:true];
}

-(void)translateToTrackedPosition:(int)index
{
    character->setTranslationX(transTrackX.at(index));
}



-(void)trackRotation
{

    rotTrackX.push_back(character->getXRotation());
    rotTrackY.push_back(character->getYRotation());
}

-(void)trackTranslation
{
    transTrackX.push_back(character->getXTranslation());
}

-(void)manageRotAndPos
{
    if (isTrackingRotAndPos)
    {
        //This way, only tracks rotation at recordingFrameRate
        float aux = fmod(rotTrackFrameRateAdjust,(float)defaultFrameRate/recordingFrameRate);
        if(aux<1)
        {
            [self trackRotation];
            [self trackTranslation];
            [self manageRecordingCounter];
        }
        rotTrackFrameRateAdjust++;
        
    }
    
    if (isPlayingTrackedRotAndPos)
    {
        int index = trackingPosition;
        
        if(isExporting)
            index = int(trackingPosition * (float)defaultFrameRate/recordingFrameRate); 
        
        
        if(index >= rotTrackX.size())
        {
            if(isExporting)
                [self clearTrackingVectors];
            
            [self stopPlayingTrackedRotAndPos];
        }
        else
        {      
            [self rotateToTrackedPosition:index];
            [self translateToTrackedPosition:index];
            trackingPosition ++;
        }
    }
}

-(void) clearTrackingVectors
{
    rotTrackX.clear();
    rotTrackY.clear();
    transTrackX.clear();
}




-(float)getTrackRotX:(int) position
{
	return rotTrackX.at(position);	
}


-(float)getTrackRotY:(int) position
{
	return rotTrackY.at(position);	
}

-(float)getTrackTransX:(int) position
{
    return transTrackX.at(position);
}


- (void)moveModelX:(float)incX y:(float)incY absolute:(BOOL)abs
{
    [self moveModelX:incX absolute:abs];
    [self moveModelY:incY absolute:abs];
}

- (void)moveModelX:(float)incX absolute:(BOOL)abs
{
    if(abs)
        character->setNormalizedTranslationX(incX);
    else
        character->addNormalizedTranslationX(incX);
}

- (void)moveModelY:(float)incY absolute:(BOOL)abs
{
    if(abs)
        character->setNormalizedTranslationY(incY);
    else
        character->addNormalizedTranslationY(incY);
}


- (void)rotateModelX:(float)incX y:(float)incY absolute:(BOOL)abs
{
        
    if(incX != 0)
    {
        if(abs)
            character->setXRotation(incX);
        else
            character->addXRotation(incX);

    }

    if(incY != 0)
    {
        if(abs)
            character->setYRotation(incY);
        else
            character->addYRotation(incY);
    }
    
}

-(int)getXCenterModel
{
    return character->getXNormalizedTranslation();
}

-(int)getYCenterModel
{
    return character->getYNormalizedTranslation();
}


-(void)moveCloser:(float) incZ absolute:(BOOL)abs
{
    if(abs)
        character->setTranslationZ(incZ);
    else
        character->addTranslationZ(incZ);
}

- (float)modelPositionX
{
    return character->getXNormalizedTranslation();
}

- (float)modelPositionY
{
    return character->getYNormalizedTranslation();
}

- (float)modelPositionZ
{
    return character->getZNormalizedTranslation();
}


- (void) animateTranslationDuring:(float)seconds incX:(float)x incY:(float)y absolute:(BOOL)abs
{
    if(animateTranslation)
    {
        NSLog(@"Cannot animate model translation: an translation is already being animated");
        return;
    }

    
    pendingFramesToAnimateTranslation = seconds * [self getFrameRate];
    if(abs)
    {
        animateTranslationFinalX = x;
        animateTranslationFinalY = y;
        animateTranslationXStep = (animateTranslationFinalX - [self getXCenterModel])/pendingFramesToAnimateTranslation;
        animateTranslationYStep = (animateTranslationFinalY - [self getYCenterModel])/pendingFramesToAnimateTranslation;
    }
    else
    {
        animateTranslationFinalX = [self getXCenterModel] + x;
        animateTranslationFinalY = [self getYCenterModel] + y;
        animateTranslationXStep = x / pendingFramesToAnimateTranslation;
        animateTranslationYStep = y / pendingFramesToAnimateTranslation;
    }
    
    animateTranslation = YES;
    
    std::cout << "SET UP ANIMATE TRANSLATION: to ( " << x << ", " << y << " ) abs: " << abs << std::endl;
    std::cout << "model is at: ( " << [self getXCenterModel] << ", " << [self getYCenterModel] << " ) " << std::endl;
    std::cout << "Final pos is: ( " << animateTranslationFinalX << ", " << animateTranslationFinalY << " )" <<std::endl;
    std::cout << "pending frames to animate is: " << pendingFramesToAnimateTranslation << std::endl;
    std::cout << "and steps are: (" << animateTranslationXStep << ", " << animateTranslationYStep << " )" << std::endl;
    
    
    
}

- (void) animateRotationDuring:(float)seconds degreesX:(float)degX degreesY:(float)degY absolute:(BOOL)abs
{
    if(animateRotation)
    {
        NSLog(@"Cannot animate model translation: an translation is already being animated");
        return;
    }
    
    pendingFramesToAnimateRotation = seconds * [self getFrameRate];
    if(abs)
    {
        animateRotationFinalDegreesX = degX;
        animateRotationFinalDegreesY = degY;
        animateRotationDegreesXStep = (animateRotationFinalDegreesX - character->getRotation().x)/pendingFramesToAnimateRotation;
        animateRotationDegreesYStep = (animateRotationFinalDegreesY - character->getRotation().y)/pendingFramesToAnimateRotation;

    }
    else
    {
        animateRotationFinalDegreesX = character->getRotation().x + degX;
        animateRotationFinalDegreesY = character->getRotation().y + degY;
        animateRotationDegreesXStep = degX /pendingFramesToAnimateRotation;
        animateRotationDegreesYStep = degY /pendingFramesToAnimateRotation;
    }
    
    animateRotation = YES;
    
    std::cout << "SET UP ANIMATE ROTATION: to ( " << degX << ", " << degY << " ) abs: " << abs << std::endl;
    std::cout << "model rot is: ( " << character->getRotation().x << ", " << character->getRotation().y << " ) " << std::endl;
    std::cout << "Final rot is: ( " << animateRotationFinalDegreesX << ", " << animateRotationFinalDegreesY << " )" <<std::endl;
    std::cout << "pending frames to animate is: " << pendingFramesToAnimateRotation << std::endl;
    std::cout << "and steps are: (" << animateRotationDegreesXStep << ", " << animateRotationDegreesYStep << " )" << std::endl;
}

- (void) manageAnimateTranslation
{
    if (animateTranslation)
    {
        if(pendingFramesToAnimateTranslation > 0)
        {
            [self moveModelX:animateTranslationXStep y:animateTranslationYStep absolute:NO];
            pendingFramesToAnimateTranslation--;
        }
        else
        {
            [self moveModelX:animateTranslationFinalX y:animateTranslationFinalY absolute:YES];
            animateTranslation = NO;
            NSLog(@"finished animating translation");
            [[NSNotificationCenter defaultCenter] postNotificationName:notificationJustFinishedTranslationAnimation object:nil];
        }
    }
}

- (void) manageAnimateRotation
{
    if (animateRotation)
    {
        if(pendingFramesToAnimateRotation > 0)
        {
            [self rotateModelX:animateRotationDegreesXStep y:animateRotationDegreesYStep absolute:NO];
            pendingFramesToAnimateRotation--;
        }
        else
        {
            [self rotateModelX:animateRotationFinalDegreesX y:animateRotationFinalDegreesY absolute:YES];
            animateRotation = NO;
            NSLog(@"finished animating rotation");
            [[NSNotificationCenter defaultCenter] postNotificationName:notificationJustFinishedRotationAnimation object:nil];
        }
    }
}



#pragma mark -
#pragma mark Path and Filename utilities



-(char*)getPathTo:(std::string) file
{
    NSString *fileString =	[NSString stringWithUTF8String:file.c_str()];
	NSString *path = [ [ [NSBundle mainBundle] resourcePath ] stringByAppendingPathComponent:fileString ];
    
	return (char*)[path cStringUsingEncoding:1];
}


-(char*)getPathFromNSStringTo:(NSString*) fileString
{
	NSString *path = [ [ [NSBundle mainBundle] resourcePath ] stringByAppendingPathComponent:fileString ];
    
	return (char*)[path cStringUsingEncoding:1];
}

-(NSString*)getNSPathFromNSStringTo:(NSString*) fileString
{
	NSString *path = [ [ [NSBundle mainBundle] resourcePath ] stringByAppendingPathComponent:fileString ];
    
	return path;
}

- (NSString *)getPathToDocuments:(NSString *)fileString
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	
	return [ [paths objectAtIndex:0] stringByAppendingPathComponent:fileString ];
}



-(char*) getWritablePathTo:(std::string) file
{
	NSString *fileString =	[NSString stringWithUTF8String:file.c_str()];
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	
	NSString *path = [ [paths objectAtIndex:0] stringByAppendingPathComponent:fileString ];
	
	
	NSFileManager *fileManager = [NSFileManager defaultManager];
	[fileManager removeItemAtPath:path error:NULL];
	
	return (char*)[path cStringUsingEncoding:1];
}	



-(char*) getWritablePathToReadFrom:(NSString*) fileString
{
//	NSString *fileString =	[NSString stringWithUTF8String:file.c_str()];
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	
	NSString *path = [ [paths objectAtIndex:0] stringByAppendingPathComponent:fileString ];
	
	return (char*)[path cStringUsingEncoding:1];
}





@end
