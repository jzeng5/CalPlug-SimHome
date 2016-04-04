//
//  NrCalendarMainViewController.m
//  VirtualAssistant
//
//  Created by Enrique on 9/28/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import "NrCalendarMainViewController.h"
#import "NrAppDelegate.h"



@interface NrCalendarMainViewController ()

- (BOOL)canSpeak;

@end

@implementation NrCalendarMainViewController

@synthesize selectedDayName;
@synthesize itemList;
@synthesize scroll;
//@synthesize backButton;
//@synthesize TimerButton;

BOOL appLocked = YES;
UIAlertView *cityAlertView, *buyAlertView, *facebookAlert, *voteOptionAlert, *pendingNotificatonAlert;
BOOL presentBuyingOptionsAfterSpeaking = NO;
NSString *facebookAppId = @"180024962079347";
BOOL postImmediately = NO;
BOOL fromLoadProfile = NO;
BOOL moreShown = NO;
BOOL pendingNotification = NO;
BOOL started;
int counter;
/*
 - (void)increaseTimerCount
 {
 self.TimerButton.text = [NSString stringWithFormat:@"%d", counter];
 
 if (started) {
 counter ++;
 }
 
 NaradaDownloader *downloader = [[NaradaDownloader alloc] initWithURLString:@"http://128.195.185.104:8080/musicglove/resources/saves/temp/RIVA_log.txt" andDelegate:self];
 [downloader requestDownload];
 }
 */

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        
        self.currentMode = NR_MAIN;
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self loadScrollMainItem];
    [self loadMainPointingBar];
    
    
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    self.assistantView.alpha = 0.0f;
    
    self.daysView.alpha = 0.0f;
    self.activityIndicator.hidden = YES;
    
    if (self.currentMode == NR_MAIN) {
        self.backButton.alpha = 0.0f;
    }
    
    self.switchMoreInfoView.hidden = YES;
    
    appLocked = NO;
}


- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    [self.glController changeSuperview:self.assistantView doStartAnimation:YES];
    
    [UIView animateWithDuration:0.3 animations:^{
        self.assistantView.alpha = 1.0f;
        self.daysView.alpha = 1.0f;
    } completion:^(BOOL finished) {
        NSLog(@"Assistant view shown");
    }];
}


- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
}

- (void)loadMainPointingBar
{
    NrMainItemView *item = [[[NSBundle mainBundle] loadNibNamed:@"NrMainItemView_iPhone" owner:self options:nil] objectAtIndex:0];
    [self loadPointingBarFromFrame:item.frame];
}

- (void)loadTestPointingBar
{
    NrMainItemView *item = [[[NSBundle mainBundle] loadNibNamed:@"NrMainItemView_iPhone" owner:self options:nil] objectAtIndex:0];
    [self loadPointingBarFromFrame:item.frame];
}

- (void)movePointingBarToItem:(NrMainItemView *)item
{
    [self movePointingBarToFrame:item.frame];
}

- (void)loadScrollMainItem
{
    self.scroll = [[UIScrollView alloc] initWithFrame:CGRectMake(0, 0, self.daysView.frame.size.width, self.daysView.frame.size.height)];
    
    
    NSInteger numberOfItems = 6;
    
    NrMainItemView *sampleLittleView = [[NrMainItemView alloc] initWithID:0];
    self.contentsView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, sampleLittleView.frame.size.width * numberOfItems, self.daysView.frame.size.height)];
    
    self.itemList = [NSMutableArray array];
    
    for (int i = 0; i < numberOfItems; i++) {
        NrMainItemView *littleView = [[NrMainItemView alloc] initWithID:i];
        
        
        littleView.backgroundColor = [UIColor clearColor];
        
        
        littleView.frame = CGRectMake(littleView.frame.size.width * i, 0, littleView.frame.size.width, littleView.frame.size.height);
        
        [self.contentsView addSubview:littleView];
        
        [self fillDayItemWithMainData:littleView];
        
        [self.itemList addObject:littleView];
    }
    
    scroll.contentSize = self.contentsView.frame.size;
    [scroll addSubview:self.contentsView];
    
    NSLog(@"Content Size: %@", NSStringFromCGSize(scroll.contentSize));
    NSLog(@"days view Size: %@", NSStringFromCGSize(scroll.frame.size));
    [self.daysView addSubview:scroll];
}

- (void)loadScrollTestItem
{
    self.scroll = [[UIScrollView alloc] initWithFrame:CGRectMake(0, 0, self.daysView.frame.size.width, self.daysView.frame.size.height)];
    
    
    NSInteger numberOfItems = 6;
    
    NrMainItemView *sampleLittleView = [[NrMainItemView alloc] initWithID:0];
    self.contentsView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, sampleLittleView.frame.size.width * numberOfItems, self.daysView.frame.size.height)];
    
    self.itemList = [NSMutableArray array];
    
    for (int i = 0; i < numberOfItems; i++) {
        NrMainItemView *littleView = [[NrMainItemView alloc] initWithID:i];
        
        
        littleView.backgroundColor = [UIColor clearColor];
        
        
        littleView.frame = CGRectMake(littleView.frame.size.width * i, 0, littleView.frame.size.width, littleView.frame.size.height);
        
        [self.contentsView addSubview:littleView];
        
        [self fillDayItemWithTestData:littleView];
        
        [self.itemList addObject:littleView];
    }
    
    scroll.contentSize = self.contentsView.frame.size;
    [scroll addSubview:self.contentsView];
    
    NSLog(@"Content Size: %@", NSStringFromCGSize(scroll.contentSize));
    NSLog(@"days view Size: %@", NSStringFromCGSize(scroll.frame.size));
    [self.daysView addSubview:scroll];
}


- (void)fillDayItemWithMainData:(NrMainItemView *)item
{
    //Here we'll present the main data (i.e. Calendar, Weather, etc.)
    switch (item.itemID) {
        case 0:
            item.dayName.text = @"Data";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"data" ofType:@"png"]];
            break;
        case 1:
            item.dayName.text = @"Calendar";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"calendario" ofType:@"png"]];
            break;
        case 2:
            item.dayName.text = @"Info";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"info" ofType:@"png"]];
            break;
        case 3:
            item.dayName.text = @"Fourth";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"fourth-img" ofType:@"png"]];
            break;
        case 4:
            item.dayName.text = @"Fifth";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"fifth-img" ofType:@"png"]];
            break;
        case 5:
            item.dayName.text = @"Sixth";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"sixth-img" ofType:@"png"]];
            break;
            
        default:
            item.dayName.text = @"First";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"sixth-img" ofType:@"png"]];
            break;
    }
}

- (void)fillDayItemWithTestData:(NrMainItemView *)item
{
    //Here we'll present the main data (i.e. Calendar, Weather, etc.)
    switch (item.itemID) {
        case 0:
            item.dayName.text = @"Home";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"home" ofType:@"png"]];
            break;
        case 1:
            item.dayName.text = @"Graphs";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"graph" ofType:@"png"]];
            break;
        case 2:
            item.dayName.text = @"Settings";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"engranaje" ofType:@"png"]];
            break;
        case 3:
            item.dayName.text = @"Fourth";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"fourth-img" ofType:@"png"]];
            break;
        case 4:
            item.dayName.text = @"Fifth";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"fifth-img" ofType:@"png"]];
            break;
        case 5:
            item.dayName.text = @"Settings";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"engranaje" ofType:@"png"]];
            break;
            
        default:
            item.dayName.text = @"First";
            item.imgView.image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"sixth-img" ofType:@"png"]];
            break;
    }
}

- (IBAction)mainItemViewClicked:(id)sender
{
    
    @synchronized(self) {
        [self cover];
        
        NrMainItemView *calItem = (NrMainItemView *)sender;
        NSLog(@"Clicked at item %d", calItem.itemID);
        
        self.selectedItem = calItem.itemID;
        
        [self movePointingBarToItem:calItem];
        
        self.activityIndicator.hidden = NO;
        [self.activityIndicator startAnimating];
        
        switch (calItem.itemID) {
            case 0:
                [self changeItemsTo:calItem.itemID];
                [self speakFirstOption];
                break;
            case 1:
                [self speakSecondOption];
                break;
            case 2:
                [self speakThirdOption];
                break;
            case 3:
                [self speakFourthOption];
                break;
            case 4:
                [self speakFifthOption];
                break;
            case 5:
                [self speakSixthOption];
                break;
            default:
                break;
        }
        
    }
}


- (void)speakToSpeakSentences
{
    NSLog(@"Files to speak: %@", self.filesToSpeak);
    
    [self.glController.modelManager playTemporaryAction:[self.filesToSpeak objectAtIndex:0] withPath:[NSFileManager documentsPath] andDelay:0];
    [self.filesToSpeak removeObjectAtIndex:0];
}

- (void)addToSpeakFile:(NSString *)filename
{
    NSString *file = [[NSFileManager documentsPath] stringByAppendingPathComponent:filename];
    NSAssert1([[NSFileManager defaultManager] fileExistsAtPath:file], @"File does not exist! File: %@", file);
    [self.filesToSpeak addObject:filename];
}

- (void)speakFirstOption
{
    //  self.filesToSpeak = [NSMutableArray array];
    //  [self addToSpeakFile:@"file1a.wav"];
    //  [self addToSpeakFile:@"file1b.wav"];
    //  [self addToSpeakFile:@"file1c.wav"];
    //  [self speakToSpeakSentences];
    
    NSArray *sentences = [NSArray arrayWithObjects:
                          @"This is where you can find your energy usage data for your different appliances.",
                          nil];
    [self speakSentences:sentences withMaxLength:400 toFileName:@"speakTest1" inLanguage:NSLocalizedString(@"LANG_TTS", nil)];
}

- (void)speakSecondOption
{
    //    self.filesToSpeak = [NSMutableArray array];
    //    [self addToSpeakFile:@"file2a.wav"];
    //    [self addToSpeakFile:@"file2b.wav"];
    //    [self addToSpeakFile:@"file2c.wav"];
    //    [self speakToSpeakSentences];
    
    // Uses text-to-speech from ispeech.org to translate string into speech
    //    NSArray *sentences = [NSArray arrayWithObjects:
    //                          @"This is the way to success once. This is the way to success twice. This is the way to success third.",
    //                          nil];
    NSArray *sentences = [NSArray arrayWithObjects:
                          @"Here you can manage your calendar in order to schedule various energy usage for your appliances.",
                          nil];
    [self speakSentences:sentences withMaxLength:400 toFileName:@"speakTest2" inLanguage:NSLocalizedString(@"LANG_TTS", nil)];
    
}

- (void)speakThirdOption
{
    //    self.filesToSpeak = [NSMutableArray array];
    //    [self addToSpeakFile:@"file3a.wav"];
    //    [self addToSpeakFile:@"file3b.wav"];
    //    [self addToSpeakFile:@"file3c.wav"];
    //    [self speakToSpeakSentences];
    
    // Ading new Audio locally through hard drive
    self.filesToSpeak = [NSMutableArray array];
    [self addToSpeakFile:@"welcome-assistant.wav"];
    [self speakToSpeakSentences];
}

- (void)speakFourthOption
{
    self.filesToSpeak = [NSMutableArray array];
    [self addToSpeakFile:@"welcome-assistant.wav"];
    //    [self addToSpeakFile:@"file4a.wav"];
    //    [self addToSpeakFile:@"file4b.wav"];
    //    [self addToSpeakFile:@"file4c.wav"];
    [self speakToSpeakSentences];
}

- (void)speakFifthOption
{
    self.filesToSpeak = [NSMutableArray array];
    [self addToSpeakFile:@"file5a.wav"];
    [self addToSpeakFile:@"file5b.wav"];
    [self addToSpeakFile:@"file5c.wav"];
    [self speakToSpeakSentences];
}

- (void)speakSixthOption
{
    self.filesToSpeak = [NSMutableArray array];
    [self addToSpeakFile:@"file6a.wav"];
    [self addToSpeakFile:@"file6b.wav"];
    [self addToSpeakFile:@"file6c.wav"];
    [self speakToSpeakSentences];
}

#pragma mark Finish Speaking Handler

- (void)handleWillBeginSpeaking
{
    self.activityIndicator.hidden = YES;
}



- (void)handleDidFinishSpeaking
{       
    self.activityIndicator.hidden = YES;
    [self uncover];
}

- (void)changeItemsTo:(NSInteger)itemID
{
    switch (itemID) {
            
        case 0:
            //          self.currentMode = NR_WEATHER;
            [self changeItemsToTest];
            break;
//        case 1: //CALENDAR -> MG
//            [self changeItemsToCalendar];
//            break;
//        case 2: //ACTIONS DEMO
//            [self changeItemsToActions];
//            break;
    }
}

- (void)changeItemsToTest
{
    self.currentMode = NR_CALENDAR;
    //    self.eventsTableView.hidden = NO;
    self.detailWeatherView.hidden = YES;
    
    [UIView animateWithDuration:0.3 animations:^{
        self.scroll.alpha = 0.0f;
    } completion:^(BOOL finished) {
        NSLog(@"Animation fade out completed");
        [self loadScrollTestItem];
        self.scroll.alpha = 0.0f;
        [UIView animateWithDuration:0.3 animations:^{
            self.scroll.alpha = 1.0f;
        } completion:^(BOOL finished) {
            NSLog(@"Animation fade in completed");
            [self.pointingBar removeFromSuperview];
            [self loadTestPointingBar];
            [self.contentsView addSubview:self.pointingBar];
            self.activityIndicator.hidden = YES;
            [self uncover];
            
//            NSError *error;
//            if (![[GANTracker sharedTracker] trackPageview:@"/app/calendar" withError:&error]) {
//                NSLog(@"Error in tracking: %@", [error localizedDescription]);
//            }
        }];
    }];
}

@end
