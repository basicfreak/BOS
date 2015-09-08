#include <api/threadman.h>

#define MAX_THREADS 4096
#define MAX_MSGS 2048

uint32_t CurrentThread;

#define TF_ACTIVE	0x00000001
#define TF_LOW		0x00000002
#define TF_MID		0x00000004
#define TF_HIGH		0x00000006
#define TF_WAITMSG	0x00000008
#define TF_USESSE	0x00000010

typedef struct ThreadEntry {
	char Name[12];
	uint32_t Flags;
	uint32_t CR3;
	regs TRegs;
	ssed TSSEd;
}__attribute__((packed)) Thread_t, *Thread_p;

typedef struct ThreadList {
	Thread_p Thread[MAX_THREADS];
}__attribute__((packed)) ThreadMan_t, *ThreadMan_p;

typedef struct MSGEntry {
	uint32_t SrcID;
	uint32_t DestID;
	size_t DataSize;
	void* Data;
}__attribute__((packed)) MSG_t, *MSG_p;

typedef struct MSGList {
	MSG_p MSG[MAX_MSGS];
}__attribute__((packed)) MSGMan_t, *MSGMan_p;

ThreadMan_p MyThreads;
MSGMan_p MyMSGs;

void _IDLE_THREAD(void);

void _TM_init()
{
	MyThreads = (ThreadMan_p) calloc(sizeof(ThreadMan_t));
	MyMSGs = (MSGMan_p) calloc(sizeof(MSGMan_t));
	CurrentThread = 0;
	MyThreads->Thread[0] = (Thread_p) calloc(sizeof(Thread_t));
	MyThreads->Thread[1] = (Thread_p) calloc(sizeof(Thread_t));
	strcpy(MyThreads->Thread[0]->Name, "KERNEL");
	strcpy(MyThreads->Thread[1]->Name, "IDLE");
	MyThreads->Thread[0]->Flags = (TF_ACTIVE | TF_HIGH);
	MyThreads->Thread[1]->Flags = (TF_ACTIVE | TF_LOW);
}

void ThreadManager(regs *r)
{
	//Save Current Thread
	memcpy(&MyThreads->Thread[CurrentThread]->TRegs, r, sizeof(regs));
	if(MyThreads->Thread[CurrentThread]->Flags & TF_USESSE)
		SSE_Save(&MyThreads->Thread[CurrentThread]->TSSEd);

	//Calculate Next Thread
	//... Temperary, Use Ring
	for(uint32_t NewThread = (CurrentThread + 1); ; NewThread++) {
		if((MyThreads->Thread[NewThread]) && (MyThreads->Thread[NewThread]->Flags & TF_ACTIVE)) {
			CurrentThread = NewThread
			break;
		}
		if(NewThread >= MAX_THREADS)
			NewThread = 0xFFFFFFFF; // that way when we loop it will be 0.
	}

	//Load New Thread
	memcpy(r, &MyThreads->Thread[CurrentThread]->TRegs, sizeof(regs));
	if(MyThreads->Thread[CurrentThread]->Flags & TF_USESSE)
		SSE_Load(&MyThreads->Thread[CurrentThread]->TSSEd);
	_VMM_setCR3(MyThreads->Thread[CurrentThread]->CR3);
}

void SendMessage(regs *r, uint32_t Destination, size_t MSGSize, void* MSG)
{
	//Send Message to Thread, If thread was waitng for message switch to it.
	for(uint32_t x = 0; x < MAX_MSGS; x++)
		if(!MyMSGs->MSG[x]) {
			MyMSGs->MSG[x] = (MSG_p) calloc(sizeof(MSG_t));
			MyMSGs->MSG[x]->SrcID = CurrentThread;
			MyMSGs->MSG[x]->DestID = Destination;
			MyMSGs->MSG[x]->DataSize = MSGSize;
			MyMSGs->MSG[x]->Data = calloc(MSGSize);
			memcpy(MyMSGs->MSG[x]->Data, MSG, MSGSize);
			if(MyThreads->Thread[Destination]->Flags & TF_WAITMSG) {
				MyThreads->Thread[Destination]->Flags &= ~(TF_WAITMSG);
				MyThreads->Thread[Destination]->Flags |= TF_ACTIVE;
				memcpy(&MyThreads->Thread[CurrentThread]->TRegs, r, sizeof(regs));
				if(MyThreads->Thread[CurrentThread]->Flags & TF_USESSE)
					SSE_Save(&MyThreads->Thread[CurrentThread]->TSSEd);
				CurrentThread = Destination;
				memcpy(r, &MyThreads->Thread[CurrentThread]->TRegs, sizeof(regs));
				if(MyThreads->Thread[CurrentThread]->Flags & TF_USESSE)
					SSE_Load(&MyThreads->Thread[CurrentThread]->TSSEd);
				_VMM_setCR3(MyThreads->Thread[CurrentThread]->CR3);
			}
			return;
		}
}

bool GetMessage(regs *r, uint32_t SrcID, size_t MaxSize, void *Buffer)
{
	//Get Message
	for(uint32_t x = 0; x < MAX_MSGS; x++)
		if(MyMSGs->MSG[x] && (MyMSGs->MSG[x]->DestID == CurrentThread) && (MyMSGs->MSG[x]->DataSize <= MaxSize)) {
			memcpy(Buffer, MyMSGs->MSG[x]->Data, MyMSGs->MSG[x]->DataSize);
			free(MyMSGs->MSG[x]->Data, MyMSGs->MSG[x]->DataSize);
			free(MyMSGs->MSG[x], sizeof(MSG_t));
			return TRUE;
		}
	return FALSE;
}

void WaitForMSG(regs *r)
{
	//Put Thread On Hold For Message
	MyThreads->Thread[CurrentThread]->Flags &= ~(TF_ACTIVE);
	MyThreads->Thread[CurrentThread]->Flags |= TF_WAITMSG;
	ThreadManager(r); // And Let the tread manager save this thread and find a new thread.
}

void _IDLE_THREAD()
{
	while(TRUE)
		hlt();
}