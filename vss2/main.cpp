
#include <stdio.h>
#include <windows.h>
#include <winbase.h>
#include <wchar.h>
#include <vss.h>
#include <vswriter.h>
#include <vsbackup.h>
#include <conio.h>

int main(int argc, char *argv[])
{
    int retCode = 0;
    int i=0;
    HRESULT hr;
    IVssEnumObject *pIEnumSnapshots;
    IVssBackupComponents *ab=NULL;
    VSS_OBJECT_PROP Prop;
    VSS_SNAPSHOT_PROP& Snap = Prop.Obj.Snap;
    WCHAR existingFilePath[MAX_PATH] = TEXT("\\test2.txt");
    WCHAR newFileLocation[MAX_PATH] =TEXT("D:\\New");
    TCHAR existingFileLocation[MAX_PATH];

    if (CoInitialize(NULL) != S_OK)
    {
        printf("CoInitialize failed!\n");
        getch();
		return 1;
    }
	else
	{
		 printf("CoInitialize!\n");
       
	}


    hr = CreateVssBackupComponents(&ab);
    if(hr != S_OK)
    {
        printf("Failed at CreateVssBackupComponents Stage");
        getch();
		return 1;
    }
	else
	{
		 printf("CreateVssBackupComponents Stage\n");
       
	}

    hr = ab->InitializeForBackup(NULL);
    if(hr != S_OK)
    {
        printf("Failed at InitializeForBackup Stage");
		getch();
        return 1;
    }
	else
	{
		 printf("InitializeForBackup Stage\n");
       
	}
    hr = ab->SetContext(VSS_CTX_ALL);
    if(hr != S_OK)
    {
        printf("Failed at SetContext Stage");
        getch();
		return 1;
    }

	 
    hr = ab->Query(GUID_NULL,VSS_OBJECT_NONE, VSS_OBJECT_SNAPSHOT, &pIEnumSnapshots);
    if(hr != S_OK){

		 printf("Qurey failed\n");
		switch (hr) 
		{
			 
		case E_ACCESSDENIED:
			printf(" The caller is not an administrator or a backup operator.");
			break;
		case E_INVALIDARG:
			printf(" One of the parameter values is not valid");
			break;
			case E_OUTOFMEMORY:
			printf(" Out of memory or other system resources.");
			break;
			case VSS_E_BAD_STATE:
			printf(" The backup components object is not initialized, this method has been called during a restore operation, or this method has not been called within the correct sequence..");
			break;
			case VSS_E_PROVIDER_VETO:
			printf(" The queried object is not found.");
			break;
			
			case VSS_E_UNEXPECTED_PROVIDER_ERROR:
			printf(" Unexpected provider error. The error code is logged in the error log. For more information, see Event and Error Handling Under VSS.");
			break;

		case E_UNEXPECTED:
			printf(" Unexpected error. The error code is logged in the error log file. For more information, see Event and Error Handling Under VSS.");
			break;

			default:
				printf("error not found");
			
		}
        
		getch();
        return 1;
    }
	




    // Enumerate all shadow copies. 
    printf("Recursing through snapshots...\n");
    while(true)
    {
        // Get the next element
        ULONG ulFetched;
        hr = pIEnumSnapshots->Next( 1, &Prop, &ulFetched );

        // We reached the end of list
        if (ulFetched == 0)
            break;

        wprintf(L"Snapshot:%s\n", Snap.m_pwszSnapshotDeviceObject);
        /*
        At this point you have the Snap object with all the information required for copying the file.
        example:
        Snap.m_pwszSnapshotDeviceObject is the root for snapshot object
        (like \\?\GLOBALROOT\Device\HarddiskVolumeShadowCopy1)
        Snap.m_pwszOriginalVolumeName is the full unicode name
        (like \\?\Volume{1240872a-88de-11df-a94d-806e6f6e6963}\)
        for the original root(c: mostly)

        So, you can use CopyFile() to do what you want
        */
  
      wcscpy(existingFileLocation, Snap.m_pwszSnapshotDeviceObject);
	 // wcscat(existingFileLocation, L"\\");
       wcscat(existingFileLocation, existingFilePath);

	/*   
	
	   
         if (CopyFile(existingFileLocation, newFileLocation, FALSE))
      { 
         printf("copy success");
      } 
      else 
      { 
         printf("\nCould not copy file. %d\n",GetLastError()); 
      
      } 
 */
	}
	wprintf(L"\nlocation:%s\n", existingFileLocation);
	SHFILEOPSTRUCT sf;
    memset(&sf,0,sizeof(sf));
    sf.hwnd = 0;
    sf.wFunc = FO_COPY;
    sf.pFrom = existingFileLocation;
    sf.pTo = L"D:\\New\\";
   sf.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI| FILE_LIST_DIRECTORY | FILE_WRITE_ATTRIBUTES;
   //  sf.fFlags=0;
	int n = SHFileOperation(&sf);
    if( n == 0)
    {

        printf("Success\n");
    }
    else
    {
		
        printf("Error\n");
		switch (n)
		{
			case 0x7C:
			printf("path error %d",GetLastError());
			break;
			case 0x74:
			printf("The source is a root directory, which cannot be moved or renamed.");
			break;
			case 0x78:
				printf("security prob");
				break;
		
		}
		
    }

	getch();
    return retCode;

}