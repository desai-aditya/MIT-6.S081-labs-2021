#include<kernel/types.h>
#include<kernel/param.h>
#include<user/user.h>

int main(int argc, char ** argv)
{
	int pid;
	char buf[512];
	while(1)
	{
		gets(buf,sizeof(buf));
		if(strlen(buf)==0)break;
		if(buf[strlen(buf)-1]=='\n')
			buf[strlen(buf)-1]=0;

		if((pid=fork())<0)
			exit(1);

		if(pid==0)
		{
			//child
			//printf("%s debug1\n",buf);
			char* eargs[MAXARG];
			for(int i = 1;i<argc; i++)
				eargs[i-1]=argv[i];
			eargs[argc-1]=(char*)buf;
			eargs[argc-1+1]=0;
			//for(int i = 0; eargs[i]!=0; i++)
			//	printf("%s debug\n",eargs[i]);
			exec(eargs[0],eargs);
		}
		else
		{
			//parent
			wait(0);
		}
	}
	exit(0);
}
