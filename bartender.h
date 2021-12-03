#ifndef BARTENDER_H
#define BARTENDER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <psp2/io/dirent.h>
#include <zip.h>
#include <psp2/notificationutil.h> 
#include <psp2/sysmodule.h> 
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	char* nameOfTrophy;
	char* trophyDescription;
	char* trophyType;
	char* isHidden;
	int trophyImageX;
	int trophyImageY;
	char* unlockKey;
	bool trophyAcquired;
	char* userName;
} trophy;



// Starts the service. Returns if the service is running
bool Bartender_StartBartender(char* encryptionKey, char* gameID);

// Unlocks the selected trophy.
bool Bartender_UnlockTrophy(int trophyID);

// Returns if the selected trophy has been unlocked
bool Bartender_CheckIfTrophyHasBeenUnlocked(int trophyNumber);

// Returns the selected trophy. 
trophy Bartender_GetTrophyInfo(int trophyID);


const char* Bartender_GetTrophyImageDirectory();


#ifdef __cplusplus
}
#endif

#endif