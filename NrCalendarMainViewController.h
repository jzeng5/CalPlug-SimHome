//
//  NrCalendarMainViewController.h
//  VirtualAssistant
//
//  Created by Enrique on 9/28/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "NrMainViewController.h"
#import "GLViewController.h"
#import "NrMainItemView.h"
#import "NaradaDownloader.h"
/*
 #define kLocationNameKey @"kLocationNameKey"
 #define kLocationCityCodeKey @"kCityCodeKey"
 #define kLocationZipCodeKey @"kZipCodeKey"
 #define kLocationCountryKey @"kCountryKey"
 #define kLocationStateKey @"kStateKey"
 #define kOpenInfoView @"kOpenInfoView"
 #define kApplicationUnlocked @"kApplicationUnlocked"
 #define notificationWakeFromLocalNotification @"notificationWakeFromLocalNotification"
 #define kVotedForHoroscope      @"kVotedForHoroscope"
 #define kVotedForRestaurants    @"kVotedForRestaurants"
 #define kVotedForPubs           @"kVotedForPubs"
 #define kVotedForFlights        @"kVotedForFlights"
 #define kVotedForHotels         @"kVotedForHotels"
 #define kVotedForCarRental      @"kVotedForCarRental"
 #ifdef __APPLE__
 #include "TargetConditionals.h"
 #endif
 enum voteOptions {
 NO_VOTE,
 HOROSCOPE,
 RESTAURANTS,
 PUBS,
 FLIGHTS,
 HOTELS,
 CARRENTAL
 };
 */

@interface NrCalendarMainViewController : NrMainViewController <
UITableViewDataSource,
UITableViewDelegate,
NSURLConnectionDataDelegate,
NaradaDownloaderDelegate
>


// UI elements

@property (nonatomic, strong) NSString *selectedDayName;

@property (nonatomic, strong) UIScrollView *scroll;

@property (nonatomic, strong) NSMutableArray *itemList;


- (IBAction)mainItemViewClicked:(id)sender;

- (void)loadMainPointingBar;
- (void)loadTestPointingBar;

- (void)movePointingBarToItem:(NrMainItemView *)item;

// Menu-loading methods
- (void)loadScrollMainItem;
- (void)loadScrollTestItem;

- (void)loadWeatherIcons;

// Menu-filling methods
- (void)fillDayItemWithMainData:(NrMainItemView *)item;
- (void)fillDayItemWithTestData:(NrMainItemView *)item;

// Menu-changing methods
- (void)changeItemsTo:(NSInteger)itemID;
- (void)changeItemsToTest;

@end
