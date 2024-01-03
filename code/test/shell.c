#include "syscall.h"

int
main()
{
    SpaceId newProc;
    OpenFileId input = 0;
    OpenFileId output = 1;
    char prompt[2], ch, buffer[60];
    int i,len;

    prompt[0] = '-';
    prompt[1] = '-';

    while( 1 )
    {
	Write(prompt, 2, output);

	i = 0;
	
	len = Read(buffer, 60, 0);
    buffer[len] = '\0';

	if( len > 0 ) {
		newProc = Exec(buffer);
		Join(newProc);
	}
    }
}

