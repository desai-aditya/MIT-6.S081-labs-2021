#include<kernel/types.h>
#include<user/user.h>

void childfunc(int p[2])
{
	int myprime=0,num=1;
	int newp[2];
	if(pipe(newp)<0)exit(1);
	int newpid;
	int firsttime=1;
	if(read(p[0],&myprime,sizeof(num))==0)return;
	printf("prime %d\n",myprime);
	while(1)
	{
		if(read(p[0],&num,sizeof(num))==0)break;
		//printf(" child %d is printing num: %d\n",getpid(),num);
		if(num%myprime!=0)
		{
			//printf(" %d is not divisible by %d\n",num,myprime);
			//printf("passing on\n");
			if(firsttime==1)
			{
				if((newpid=fork())<0)exit(1);
				if(newpid==0)
				{
					//child
					close(newp[1]);
					childfunc(newp);
					close(newp[0]);
					exit(0);
				}
				firsttime=0;
			}
			write(newp[1],&num,sizeof(num));
		}
	}
	close(newp[1]);
	close(newp[0]);
	wait(0);

}

int main(){
	int pid;
	int p[2];
	int num=1;
	if(pipe(p)<0)exit(2);
	if((pid=fork())<0)exit(1);

	if(pid==0)
	{ //child
		close(p[1]);
		childfunc(p);
	}
	else
	{ //parent
		close(p[0]);
		for(int i = 2 ; i <= 35; i ++)
		{
			num=i;
			write(p[1],&num,sizeof(num));
		}
		close(p[1]);
		wait(0);

	}
	exit(0);
}
