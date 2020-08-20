#include <fstream>
#include <asm/unistd.h>
#include <list>
#include <iostream>
#include "pin.H"

using namespace std;
// http://shell-storm.org/blog/Taint-analysis-and-pattern-matching-with-Pin/

/* area of bytes tainted */
struct range
{
  UINT64 start;
  UINT64 end;
};

std::list<struct range> bytesTainted;

INT32 Usage()
{
    cerr << "Ex 1" << endl;
    return -1;
}

VOID ReadMem(UINT64 insAddr, std::string insDis, UINT64 memOp)
{
  list<struct range>::iterator i;
  UINT64 addr = memOp;

  for(i = bytesTainted.begin(); i != bytesTainted.end(); ++i){
      if (addr >= i->start && addr < i->end){
        std::cout << std::hex << "[READ in " << addr << "]\t" << insAddr 
          << ": " << insDis<< std::endl;
      }
  } 
}

VOID WriteMem(UINT64 insAddr, std::string insDis, UINT64 memOp)
{
  list<struct range>::iterator i;
  UINT64 addr = memOp;

  for(i = bytesTainted.begin(); i != bytesTainted.end(); ++i){
      if (addr >= i->start && addr < i->end){
        std::cout << std::hex << "[WRITE in " << addr << "]\t" << insAddr 
          << ": " << insDis<< std::endl;
      }
  } 
}

VOID Instruction(INS ins, VOID *v)
{
    if (INS_MemoryOperandIsRead(ins, 0) && INS_OperandIsReg(ins, 0)){
        INS_InsertCall(
            ins, IPOINT_BEFORE, (AFUNPTR)ReadMem,
            IARG_ADDRINT, INS_Address(ins),
            IARG_PTR, new string(INS_Disassemble(ins)),
            IARG_MEMORYOP_EA, 0,
            IARG_END);
    }
    else if (INS_MemoryOperandIsWritten(ins, 0)){
        INS_InsertCall(
            ins, IPOINT_BEFORE, (AFUNPTR)WriteMem,
            IARG_ADDRINT, INS_Address(ins),
            IARG_PTR, new string(INS_Disassemble(ins)),
            IARG_MEMORYOP_EA, 0,
            IARG_END);
    }
}

static unsigned int lock;

#define TRICKS(){if (lock++ == 0)return;}

VOID Syscall_entry(THREADID thread_id, CONTEXT *ctx, SYSCALL_STANDARD std, void *v)
{
    struct range taint;

    /* If the syscall is read take the branch */
    if (PIN_GetSyscallNumber(ctx, std) == __NR_read){

        TRICKS();

        /* Get the second argument */
        taint.start = static_cast<UINT64>((PIN_GetSyscallArgument(ctx, std, 1)));

        /* Get the third argument */
        taint.end   = taint.start + static_cast<UINT64>((PIN_GetSyscallArgument(ctx, std, 2)));

        /* Add this area in our tainted bytes list */
        bytesTainted.push_back(taint);

        /* Just display information */
        std::cout << "[TAINT]\t\t\tbytes tainted from " << std::hex << "0x" << taint.start \
        << " to 0x" << taint.end << " (via read)"<< std::endl;
    }
}

int main(int argc, char *argv[])
{
    /* Init Pin arguments */
    if(PIN_Init(argc, argv)){
        return Usage();
    }

    // PIN_SetSyntaxIntel();

    /* Add the syscall handler */
    PIN_AddSyscallEntryFunction(Syscall_entry, 0);

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);
    
    /* Start the program */
    PIN_StartProgram();

    return 0;
}