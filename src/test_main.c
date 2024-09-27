#define DEBUG
#include "bgm.h"

int main(int argc, char *argv[])
{
	DWORD	song;
	DWORD	len;
	
	DOUT ("-- Ready to start tests. --\n");
	system("PAUSE");
  
	bgm_Init(0,0,0,0,0);
	
	DOUT ("Initialized BGM. Press a key to load and play a module.\n");
	system("PAUSE");
	
	song = bgm_Load("module2.it", FALSE, FALSE);
	len = bgm_GetLenById(song);
	bgm_PlayById(song, TRUE);
	
	DOUT ("Loaded module OK; now playing the module. Length is %is.\nPress a key to set volume.\n", len);
	system("PAUSE");
	
	bgm_SetAttrById(song,"tVolume0","20");
	
	DOUT ("Set volume. Press a key to close BGM.\n");
	system("PAUSE");
	
	bgm_Close();
	
	DOUT ("BGM closed. Press a key to quit.\n");
	system("PAUSE");
  
	return 0;
}
