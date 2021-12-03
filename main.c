#include "bartender.h"


trophy *listOfTrophies;
int listOfTrophiesSize = 0;
int *listOfTrophyUnlocks;
int listOfTrophyUnlocksSize = 0;
int trophyUnlockTime = 0;

char* trophyPackDirectoryImage = "";
bool pinaColoadIsRunning = false;
zip_file_t* trophyPack;
zip_file_t* trophyImage;
zip_t* myZipFile;
char* gameID2 = "";
char* encryptionKey2 = "";
char* loadedUserName = "";
	
char* MergeStrings(char* original, char* newString)
{
	// Mereges 2 Strings together
	int sizeToPass = (strlen(original)+strlen(newString) + 1) * sizeof(char);
    size_t outputSize =  sizeToPass;
    char* output = malloc(outputSize);

	for(int i = 0; i < strlen(original); i++)
	{
		output[i] = original[i];
	}
	
	for(int i = strlen(original), j=0; j< strlen(newString); i++, j++)
	{
		output[i] = newString[j];
	}
   	output[sizeToPass-1]='\0';

    return output;
}



char* SubStr(char* original, long stringLength)
{
	//Gets a Substring
	int sizeToPass = (strlen(original) + (stringLength+1)) * sizeof(char);
	size_t outputSize =  sizeToPass;
    char *newString = malloc(outputSize);
	strncpy(newString,original,stringLength);	
	newString[stringLength] = '\0';
	
    
    return newString;
}


void ascii2utf(uint16_t* dst, char* src){
	//Converts the char to UTF
	if(!src || !dst)return;
	while(*src)*(dst++)=(*src++);
	*dst=0x00;
}

void SendNotification(char* textToSend)
{
	//Creates a notification pop-up.
	SceNotificationUtilProgressFinishParam param;
	memset(&param,0,sizeof(SceNotificationUtilProgressFinishParam));
	ascii2utf(param.notificationText,textToSend);
	sceNotificationUtilSendNotification (param.notificationText);
	free(textToSend);
}


char* Bartender_Encrypt(char *original) {
    // This function is a secret
    int len =strlen(original);
    char* writingTo = malloc((sizeof(char)*(len*2) )+ 1);	
	
    return writingTo; 
}

char* Bartender_GetTrophyKey(trophy tempTrophy)
{
	// This function is a secret.
	char* trophyKey = malloc(sizeof(char)*100);
	
	
	return trophyKey;
}

void AddToTrophyArray()
{
	//Adds a trophy to the list of trophies
    listOfTrophiesSize++;
    trophy *listOfTrophiesReallocate = realloc(listOfTrophies,listOfTrophiesSize * sizeof(trophy));
    if(listOfTrophiesReallocate)
    {      
        listOfTrophies = listOfTrophiesReallocate;
    }
    else
    {
        free(listOfTrophiesReallocate);
    }
}


void Bartender_LoadFile(char* encryptionKey, char* gameID)
{
	// Loads the trophy pack and extracts the game image and trophy pack image if they do not currently exist.
	
	
	char* folderDirectory = "";
	char* tempDirectoryZip = "ux0:data/Bartender/";
	
	tempDirectoryZip = MergeStrings(tempDirectoryZip,loadedUserName);
	tempDirectoryZip =MergeStrings(tempDirectoryZip,"/");
	tempDirectoryZip =MergeStrings(tempDirectoryZip,gameID);
	tempDirectoryZip =MergeStrings(tempDirectoryZip,"/");
	folderDirectory = MergeStrings(folderDirectory,tempDirectoryZip);
	tempDirectoryZip = MergeStrings(tempDirectoryZip,gameID);
	tempDirectoryZip = MergeStrings(tempDirectoryZip,".dat");
	
	char* tempDirectoryZipImage = "app0:";
	tempDirectoryZipImage = MergeStrings(tempDirectoryZipImage,gameID);
	tempDirectoryZipImage = MergeStrings(tempDirectoryZipImage,".dat");
	
	const char* trophyDirectory = tempDirectoryZip;
	const char* imageDirectory = tempDirectoryZipImage;
	// Open the zip file
	
	myZipFile = zip_open(trophyDirectory, 0, NULL);
	zip_t* imageZipFile = zip_open(imageDirectory,0,NULL);
	zip_set_default_password(myZipFile,encryptionKey);
	zip_set_default_password(imageZipFile,encryptionKey);
	trophyPack = zip_fopen(myZipFile, (char*)"trophy.msv", ZIP_FL_UNCHANGED);
	
	

	
	// Check if game image exist if it does not pull it out of the file from the main trophy pack in the app.
	char* gameImageDirectory = folderDirectory;
	gameImageDirectory = MergeStrings(gameImageDirectory,"game.png");
	SceUID gameImageUID;
	if((gameImageUID = sceIoOpen(gameImageDirectory, SCE_O_RDONLY, 0777) < 0))
	{
		SceUID dst;
		dst = sceIoOpen(gameImageDirectory,SCE_O_CREAT | SCE_O_RDWR ,0777);
		zip_file_t* gameImageFile = zip_fopen(imageZipFile, (char*)"game.png", ZIP_FL_UNCHANGED);
		zip_stat_t gameImageZipStat;
		zip_stat_init(&gameImageZipStat);
		zip_stat(imageZipFile, "game.png", ZIP_FL_UNCHANGED, &gameImageZipStat);
		char* bufferToWriteTemp[gameImageZipStat.size];
		zip_fread(gameImageFile, bufferToWriteTemp, gameImageZipStat.size);
		sceIoWrite(dst,bufferToWriteTemp,gameImageZipStat.size);
		sceIoClose(dst);
		zip_fclose(gameImageFile);
	}
	else
	{
		sceIoClose(gameImageUID);
	}

	// Check if Trophy image exist in package directory, if it does not it will copy it. If it does it will load it into memory
	char* trophyPackImageDirectory = folderDirectory;
	trophyPackImageDirectory = MergeStrings(trophyPackImageDirectory,"trophy.png");
	SceUID trophyImageUID;
	if((trophyImageUID = sceIoOpen(trophyPackImageDirectory, SCE_O_RDONLY, 0777) < 0))
	{
		trophyImage = zip_fopen(imageZipFile, (char*)"trophy.png", ZIP_FL_UNCHANGED);
		
		zip_stat_t zipStatImage;
		zip_stat_init(&zipStatImage);
		zip_stat(imageZipFile, "trophy.png", ZIP_FL_UNCHANGED, &zipStatImage);
		
		char* imageBuffer[zipStatImage.size];
		zip_fread(trophyImage, imageBuffer, zipStatImage.size);
		SceUID dst;
		dst = sceIoOpen(trophyPackImageDirectory,SCE_O_CREAT|SCE_O_RDWR ,0777);
		
		sceIoWrite(dst, imageBuffer,zipStatImage.size);
		sceIoClose(dst);
		
		
		zip_fclose(trophyImage);
		zip_close(imageZipFile);
			
		trophyPackDirectoryImage = trophyPackImageDirectory;
		
		
	}
	else
	{
		
		trophyPackDirectoryImage = trophyPackImageDirectory;
		zip_close(imageZipFile);
		sceIoClose(trophyImageUID);		
	}

	
	if(trophyPack!= NULL)
	{
		
		// Get the size of the trophy file
		zip_stat_t zipStatPack;
		zip_stat_init(&zipStatPack);
		zip_stat(myZipFile, "trophy.msv", ZIP_FL_UNCHANGED, &zipStatPack);
		
		char trophyBuffer[zipStatPack.size + 1];
	
		

		// Get the size of the trophy image file
		
		
		zip_fread(trophyPack, trophyBuffer, zipStatPack.size);
		
		trophyBuffer[zipStatPack.size] = '\0';
		// Load the text file
		
		char* trophyText = "";
		
		
		trophyText = MergeStrings(trophyText,trophyBuffer);
		
		char* currentItemBuilding = "";
		int trophyBuilding = 0;
		int itemNumber = 0;
		
		for (int i = 0; i < strlen(trophyBuffer); i++)
		{
			// Trophy name, Trophy Description, Trophy type, Trophy Acquired, Trophy Image x, Trophy Image Y
			if (itemNumber == 0)
			{
				AddToTrophyArray();
				itemNumber++;
			}
			char* currentItem = "";
			currentItem = MergeStrings("",SubStr(&trophyText[i],1));

			
			if (strcmp(currentItem, "|")!= 0 && strcmp(currentItem, "\n") != 0)
			{
				
				currentItemBuilding = MergeStrings(currentItemBuilding,currentItem);
			}

			if (strcmp(currentItem, "|") == 0 || strcmp(currentItem, "\n") == 0|| i == strlen(trophyBuffer) - 1)

			{
				if (itemNumber == 1)
				{
					listOfTrophies[trophyBuilding].nameOfTrophy = "";
					listOfTrophies[trophyBuilding].nameOfTrophy = MergeStrings(listOfTrophies[trophyBuilding].nameOfTrophy,currentItemBuilding);
				}
				if (itemNumber == 2)
				{
					listOfTrophies[trophyBuilding].trophyDescription ="";
					listOfTrophies[trophyBuilding].trophyDescription = MergeStrings(listOfTrophies[trophyBuilding].trophyDescription,currentItemBuilding);
				}

				if (itemNumber == 3)
				{
					listOfTrophies[trophyBuilding].trophyType ="";
					listOfTrophies[trophyBuilding].trophyType = MergeStrings(listOfTrophies[trophyBuilding].trophyType,currentItemBuilding);
				}

				if (itemNumber == 4)
				{
					listOfTrophies[trophyBuilding].isHidden ="";
					listOfTrophies[trophyBuilding].isHidden = MergeStrings(listOfTrophies[trophyBuilding].isHidden,currentItemBuilding);
				}

				if (itemNumber == 5)
				{
					listOfTrophies[trophyBuilding].trophyImageX = atoi(currentItemBuilding);
				}

				if (itemNumber == 6)
				{
					listOfTrophies[trophyBuilding].trophyImageY = atoi(currentItemBuilding);
				}
				
				if(itemNumber == 7)
				{
					listOfTrophies[trophyBuilding].unlockKey ="";
					listOfTrophies[trophyBuilding].unlockKey = MergeStrings(listOfTrophies[trophyBuilding].unlockKey,currentItemBuilding);
					
					listOfTrophies[trophyBuilding].trophyAcquired = false;
					if(strcmp(listOfTrophies[trophyBuilding].unlockKey,"1") == 0 ||
						strlen(listOfTrophies[trophyBuilding].unlockKey) > 1)
						{
							listOfTrophies[trophyBuilding].trophyAcquired = true;
						}
					
				}
				
				if(itemNumber == 8)
				{
					listOfTrophies[trophyBuilding].userName ="";
					listOfTrophies[trophyBuilding].userName = MergeStrings(listOfTrophies[trophyBuilding].userName,currentItemBuilding);
				}

				currentItemBuilding = "";
				
				itemNumber++;
				if (itemNumber == 9)
				{
					
					itemNumber = 0;
					trophyBuilding++;
				}
			}
		}
				
		pinaColoadIsRunning = true;
		zip_fclose(trophyPack);
		zip_close(myZipFile);
		
	}
	
	
		
	
	free(folderDirectory);
	free(tempDirectoryZip);
	free(tempDirectoryZipImage);
	free(gameImageDirectory);
	free(trophyPackImageDirectory);

}


void Bartender_SaveFile()
{
	// Saves the trophy pack.
	
	//Write the file to a string
				char* myFileString = "";
				for (int i = 0; i < listOfTrophiesSize; i++)
				{
					myFileString = MergeStrings(myFileString,listOfTrophies[i].nameOfTrophy);
					myFileString = MergeStrings(myFileString,"|");
					myFileString = MergeStrings(myFileString,listOfTrophies[i].trophyDescription);
					myFileString = MergeStrings(myFileString,"|");
					myFileString = MergeStrings(myFileString,listOfTrophies[i].trophyType);
					myFileString = MergeStrings(myFileString,"|");
					myFileString = MergeStrings(myFileString,listOfTrophies[i].isHidden);
					myFileString = MergeStrings(myFileString,"|");
					char tempTrophyX[10];
					int tempXSize = sprintf(tempTrophyX, "%d",listOfTrophies[i].trophyImageX);
					tempTrophyX[tempXSize] = '\0';
					myFileString = MergeStrings(myFileString,tempTrophyX);
					myFileString = MergeStrings(myFileString,"|");
					char tempTrophyY[10];
					int tempYSize = sprintf(tempTrophyY, "%d",listOfTrophies[i].trophyImageY);
					tempTrophyY[tempYSize] = '\0';
					myFileString = MergeStrings(myFileString,tempTrophyY);
					myFileString = MergeStrings(myFileString,"|");
					myFileString = MergeStrings(myFileString,listOfTrophies[i].unlockKey);
					myFileString = MergeStrings(myFileString,"|");
					myFileString = MergeStrings(myFileString,listOfTrophies[i].userName);
					
					
					if(i != listOfTrophiesSize -1)
					{
						myFileString = MergeStrings(myFileString,"\n");
					}
					
				}
				
				
			char* tempDirectoryZip = "ux0:data/Bartender/";
			tempDirectoryZip = MergeStrings(tempDirectoryZip,loadedUserName);
			tempDirectoryZip = MergeStrings(tempDirectoryZip,"/");
			tempDirectoryZip = MergeStrings(tempDirectoryZip,gameID2);
			tempDirectoryZip = MergeStrings(tempDirectoryZip,"/");
			tempDirectoryZip = MergeStrings(tempDirectoryZip,gameID2);
			tempDirectoryZip = MergeStrings(tempDirectoryZip,".dat");
			
			const char* trophyDirectory = tempDirectoryZip;
			sceIoRemove(trophyDirectory);
			
			// Open the zip file
			myZipFile = zip_open(trophyDirectory, ZIP_CREATE, NULL);	
			//zip_set_default_password(myZipFile,encryptionKey2);
			// Save the file
			
			const char *fileToWrite = myFileString;
			zip_source_t * sourceFileToWrite = zip_source_buffer_create(fileToWrite, strlen(fileToWrite), 0, 0);
			zip_file_add(myZipFile,"trophy.msv", sourceFileToWrite,ZIP_FL_OVERWRITE|ZIP_FL_ENC_GUESS);
			zip_stat_t zipStatPack;
			zip_stat_init(&zipStatPack);
			
			zip_close(myZipFile);
			myZipFile = zip_open(trophyDirectory,ZIP_CHECKCONS,NULL);
			
						
			int myStatResponse = zip_stat(myZipFile, "trophy.msv", ZIP_FL_UNCHANGED, &zipStatPack);
			
			
			char responseText[5];
			int responseSize = sprintf(responseText,"%d",myStatResponse);
			responseText[responseSize] = '\0';
						
			zip_uint64_t fileIndex = zipStatPack.index;
			zip_file_set_encryption(myZipFile, fileIndex, ZIP_EM_AES_256, encryptionKey2);
			
			sceIoRemove(trophyDirectory);
			
			zip_close(myZipFile);
			
			free(myFileString);
			free(tempDirectoryZip);
			
}


void PinaColada_CheckIfFileExistInFolder(char* encryptionKey, char* gameID)
{
	// Checks for Bartender folder, and if it doesn't exist it creates one.
	if(sceIoDopen("ux0:data/Bartender") < 0)
	{		
		sceIoMkdir("ux0:data/Bartender",0777); 
	}
	
	
	
	//Check the Bartender folder for the file that contains the user name and password. 
	//If the file does not exist it will add a -1 to the trophy list and return.
	char* settingDirectory = "ux0:Data/Bartender/settings.ini";
	SceUID settingsFile = sceIoOpen(settingDirectory,SCE_O_RDONLY, 0777);
	if(settingsFile >=0)
	{
		// Read the file
		long fileSize = sceIoLseek(settingsFile,0,SCE_SEEK_END);
		sceIoLseek(settingsFile,0,SCE_SEEK_SET);
		char fileToRead[fileSize+1];
		
		
		sceIoRead(settingsFile,fileToRead,fileSize);
		
		fileToRead[fileSize] = '\0';
		
		
				
		// Parse file
		int itemNumber = 0;
		char* stringBuilding = "";
		sceIoClose(settingsFile);
		for(int i = 0; i < strlen(fileToRead); i++)
		{
			if(fileToRead[i] == '|' || i ==  strlen(fileToRead)-1)
			{
				if(i == strlen(fileToRead)-1)
				{
					if(fileToRead[i] != '\n')
					{				
						stringBuilding = MergeStrings(stringBuilding,&fileToRead[i]);
					}
				}
				if(itemNumber == 0)
				{
					loadedUserName = MergeStrings(loadedUserName,stringBuilding);
					
					break;
				}
				
				
				itemNumber++;
				stringBuilding = "";
			}				
			else
			{
				
				char* addingCharacter = SubStr(&fileToRead[i],1);
				stringBuilding = MergeStrings(stringBuilding,addingCharacter);
			}
		}
		
	}
	else
	{
		// Adds the error message to the list of trophy unlocks and returns.
		char * messageToSend = "";
		messageToSend = MergeStrings(messageToSend,"Log into the Bartender app to unlock drinks");
		SendNotification(messageToSend);
		return;
	}
	
	
	char *textToSend = "Logged in as ";
	textToSend = MergeStrings(textToSend,loadedUserName);
	SendNotification(textToSend);
	
	// Checks if the directory with user name has been created, if not it creates it.
	
	char* directoryWithUserName = "ux0:data/Bartender/";
	directoryWithUserName = MergeStrings(directoryWithUserName,loadedUserName);
	if(sceIoDopen(directoryWithUserName) < 0)
	{		
		sceIoMkdir(directoryWithUserName,0777); 
	}
	
	
	// Checks the Bartender folder for the packs folder. If it does not exist is creates one.
	char* filePackDirectory = directoryWithUserName;
	filePackDirectory = MergeStrings(filePackDirectory,"/");
	filePackDirectory = MergeStrings(filePackDirectory,gameID);
	if(sceIoDopen(filePackDirectory) < 0)
	{

		sceIoMkdir(filePackDirectory,0777);
	}
	
	
	
	//Checks if File with Game ID exist in the Pina Coloda Folder
	
	char* tempDirCheck = filePackDirectory;
	tempDirCheck = MergeStrings(tempDirCheck,"/");
	tempDirCheck = MergeStrings(tempDirCheck,gameID);
	tempDirCheck = MergeStrings(tempDirCheck,".dat");
	const char* tempDir = tempDirCheck;
	if (sceIoOpen(tempDir, SCE_O_RDONLY,0777)>=0)
	{
		//Load the file
		Bartender_LoadFile(encryptionKey, gameID);
	}
	else
	{
		//Since the file does not exist we will copy the file from app0 to the correct directory.
		char* directoryString = "app0:";
		directoryString = MergeStrings(directoryString,gameID);
		directoryString = MergeStrings(directoryString,".dat");
		const char* originalDirectory = directoryString;
		SceUID tempFileOpen;
		if((tempFileOpen=  sceIoOpen(originalDirectory, SCE_O_RDONLY,0777)<0))
		{
			pinaColoadIsRunning = false;
			return;
		}
		else
		{
			sceIoClose(tempFileOpen);
		}
		
		char* directoryString2 = "ux0:data/Bartender/";
		directoryString2 = MergeStrings(directoryString2,loadedUserName);
		directoryString2 = MergeStrings(directoryString2,"/");
		directoryString2 = MergeStrings(directoryString2,gameID);
		directoryString2 = MergeStrings(directoryString2,"/");
		directoryString2 = MergeStrings(directoryString2,gameID);
		directoryString2 = MergeStrings(directoryString2,".dat");
		const char* newDirectory = directoryString2;
		
		
		//Copy File
		
		SceUID src = sceIoOpen(originalDirectory, SCE_O_RDONLY, 0777);
		SceUID dst = sceIoOpen(newDirectory, SCE_O_CREAT |SCE_O_RDWR,0777);
		SceOff srcSize = sceIoLseek(src,0,SCE_SEEK_END);
		sceIoLseek(src,0,SCE_SEEK_SET);
	
		char bufferToWrite[srcSize];
		sceIoRead(src,bufferToWrite,srcSize);
		sceIoWrite(dst,bufferToWrite,srcSize);
		
		sceIoClose(src);
		sceIoClose(dst);
			
		// Load the file
		Bartender_LoadFile(encryptionKey, gameID);
		
		free(directoryString2);

		free(directoryString);


	}
	
	
	
	free(directoryWithUserName);
	free(filePackDirectory);
	free(tempDirCheck);

}




bool Bartender_CheckVitaIslandIceTea()
{
	//Checks if the Vita Island Ice Tea has been unlocked.
	
	if(listOfTrophies[0].trophyAcquired == false && strcmp(listOfTrophies[0].trophyType,"4") != 0)
	{
		for(int i = 1; i < listOfTrophiesSize; i++)
		{
			if(listOfTrophies[i].trophyAcquired == false)
			{
				return false;
			}
		}
		
			char* tempKey = "";
		
			tempKey = MergeStrings(tempKey,gameID2);
			tempKey = MergeStrings(tempKey,"0");
			tempKey = MergeStrings(tempKey,encryptionKey2);
			tempKey = MergeStrings(tempKey,listOfTrophies[0].trophyType);
			
			listOfTrophies[0].trophyAcquired = true;
			listOfTrophies[0].unlockKey = Bartender_Encrypt(tempKey);
			listOfTrophies[0].userName = loadedUserName;
			
			
			char *textToPass = "";
				
				if(strcmp(listOfTrophies[0].trophyType,"1") == 0)
				{
					textToPass = MergeStrings(textToPass,"[RMCK] ");
				}
				
				if(strcmp(listOfTrophies[0].trophyType,"2") == 0)
				{
					textToPass = MergeStrings(textToPass,"[MJTO] ");
				}
				
				if(strcmp(listOfTrophies[0].trophyType,"3") == 0)
				{
					textToPass = MergeStrings(textToPass,"[PICA] ");
				}
				
				if(strcmp(listOfTrophies[0].trophyType,"4") == 0)
				{
					textToPass = MergeStrings(textToPass,"[VIIT] ");
				}
				
				textToPass = MergeStrings(textToPass,listOfTrophies[0].nameOfTrophy);
				
				if(strlen(textToPass) > 55)
				{
					textToPass = SubStr(textToPass,55);
					textToPass = MergeStrings(textToPass,"...");
				}
				
				SendNotification(textToPass);
				
			
			return true;
	}
	
	return false;
}

bool Bartender_UnlockTrophy(int trophyID)
{
	//Unlocks the selected trophy.
	
	if (pinaColoadIsRunning == true)
	{
		// Write Yes to the file
		if (trophyID < listOfTrophiesSize && listOfTrophies[trophyID].trophyAcquired == false  && strcmp(listOfTrophies[trophyID].trophyType,"4") != 0)
		{
			char* tempKey = "";
			
			tempKey = Bartender_GetTrophyKey(listOfTrophies[trophyID]);
			
			listOfTrophies[trophyID].trophyAcquired = true;
			listOfTrophies[trophyID].unlockKey = Bartender_Encrypt(tempKey);
			listOfTrophies[trophyID].userName = loadedUserName;
			
			
			
			if(strcmp(listOfTrophies[0].trophyType,"4") == 0)
			{
				Bartender_CheckVitaIslandIceTea();
			}
			
				char *textToPass = "";
				
				if(strcmp(listOfTrophies[trophyID].trophyType,"1") == 0)
				{
					textToPass = MergeStrings(textToPass,"[RMCK] ");
				}
				
				if(strcmp(listOfTrophies[trophyID].trophyType,"2") == 0)
				{
					textToPass = MergeStrings(textToPass,"[MJTO] ");
				}
				
				if(strcmp(listOfTrophies[trophyID].trophyType,"3") == 0)
				{
					textToPass = MergeStrings(textToPass,"[PICA] ");
				}
				
				if(strcmp(listOfTrophies[trophyID].trophyType,"4") == 0)
				{
					textToPass = MergeStrings(textToPass,"[VIIT] ");
				}
				
				textToPass = MergeStrings(textToPass,listOfTrophies[trophyID].nameOfTrophy);
				
				if(strlen(textToPass) > 55)
				{
					textToPass = SubStr(textToPass,55);
					textToPass = MergeStrings(textToPass,"...");
				}
				
				SendNotification(textToPass);
				Bartender_SaveFile();
				
				
				return true;
		}

	
	}

	return false;
}



bool Bartender_CheckIfTrophyHasBeenUnlocked(int trophyNumber)
{
	//Checks if the trophy has been unlocked.
	
	bool tempUnlock = false;
	if(trophyNumber < listOfTrophiesSize)
	{
		tempUnlock = listOfTrophies[trophyNumber].trophyAcquired;
	}
	
	return tempUnlock;
}

trophy Bartender_GetTrophyInfo(int trophyID)
{
	// Returns the selected trophy
	
	trophy tempTrophy;
	
	if (pinaColoadIsRunning == true)
	{
		tempTrophy = listOfTrophies[trophyID];
		tempTrophy.unlockKey="";
		return tempTrophy;
	}
	
	return  tempTrophy;
}

const char* Bartender_GetTrophyImageDirectory()
{
	// Returns the directory of the trophy image.
	
	return trophyPackDirectoryImage;
}

	
	
bool Bartender_StartBartender(char* encryptionKey, char* gameID)
{
	// Starts the bartender app.
	
	trophyPackDirectoryImage = "";
	gameID2 = MergeStrings(gameID2,gameID);
	encryptionKey2 = MergeStrings(encryptionKey2,encryptionKey);
	
	sceSysmoduleLoadModule(SCE_SYSMODULE_NOTIFICATION_UTIL); 

	PinaColada_CheckIfFileExistInFolder(encryptionKey2, gameID2);
	
	if(pinaColoadIsRunning == true)
	{
			
			bool trophyUnlocked = Bartender_CheckVitaIslandIceTea();
			
			if(trophyUnlocked == true)
			{
				Bartender_SaveFile();
			}
	}
	
	return pinaColoadIsRunning;
}