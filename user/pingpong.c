#include<kernel/types.h>
#include<user/user.h>

int main()
{
	int p[2];
	int pid;
	if(pipe(p)<0)exit(1);
	if((pid=fork())<0)exit(1);
	char buf[10];
	//printf("my pid is %d\n",pid);
	if(pid==0)
	{
		// child process
		//while(read(p[0],buf,1)!=1)fprintf(1,"waiting child\n");
		read(p[0],buf,1);
		fprintf(1,"%d: received ping\n",getpid());
		write(p[1],"a",1);
	}
	else
	{
		// parent process
		write(p[1],"a",1);
		read(p[1],buf,1);
		wait((int*)0);
		fprintf(1,"%d: received pong\n",getpid());

	}
	close(p[0]);
	close(p[1]);
	exit(0);


}
