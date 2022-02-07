#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.

  uint64 first_user_page_va, user_buf;
  int npages;

  if(argaddr(0,&first_user_page_va)<0 || argint(1,&npages) < 0 || argaddr(2,&user_buf) < 0)
	  return -1;

  // can only check 32 pages at max
  if(npages > 32) 
	  return -1;
  
  uint32 kernel_bitmask_buffer = 0;
  
  struct proc * p = myproc();

  // go through all virtual pages
  for( int i = 0 ; i < npages; i ++)
  {
	  // get the corrent page table entry of the page
	  pte_t * pte = walk(p->pagetable,first_user_page_va,0);
	  // if the page's access bit was set
	  if(*pte  & PTE_A)
	  {
		  // clear the access bit 
		  *pte = *pte & (~PTE_A);
		  kernel_bitmask_buffer = kernel_bitmask_buffer | (1<<i);
	  }

	  first_user_page_va += PGSIZE;
  }

  // copyout the kernel bitmask buffer to user space

  if(copyout(p->pagetable, user_buf, (char *)&kernel_bitmask_buffer, sizeof(kernel_bitmask_buffer)) < 0)
     return -1;

  
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
