#include "pin.H"
#include <list>
#include <fstream>
#include <iostream>
#include <asm/unistd.h>

using namespace std;

#define ALLOCATE  1
#define FREE      !ALLOCATE

#define CHECKED 1

static size_t lastSize;

struct mallocArea
{
    UINT64 base;
    UINT64 size;
    BOOL status;
    BOOL check;
};

std::list<UINT64>     addressTainted;
std::list<REG>        regsTainted;
std::list<struct mallocArea> mallocAreaList;

INT32 Usage()
{
    cerr << "Ex 6" << endl;
    return -1;
}


bool checkAlreadyRegTainted(REG reg)
{
    list<REG>::iterator i;

    for(i = regsTainted.begin(); i != regsTainted.end(); i++){
        if (*i == reg){
            return true;
        }
    }
    return false;
}

VOID removeMemTainted(UINT64 addr)
{
    addressTainted.remove(addr);
    std::cout << std::hex << "\t\t\t" << addr << " is now freed" << std::endl;
}

VOID addMemTainted(UINT64 addr)
{
    addressTainted.push_back(addr);
    std::cout << std::hex << "\t\t\t" << addr << " is now tainted" << std::endl;
}

bool taintReg(REG reg)
{
    if (checkAlreadyRegTainted(reg) == true){
        std::cout << "\t\t\t" << REG_StringShort(reg) << " is already tainted" << std::endl;
        return false;
    }

  switch(reg){

    case REG_RAX:  regsTainted.push_front(REG_RAX);
    case REG_EAX:  regsTainted.push_front(REG_EAX); 
    case REG_AX:   regsTainted.push_front(REG_AX); 
    case REG_AH:   regsTainted.push_front(REG_AH); 
    case REG_AL:   regsTainted.push_front(REG_AL); 
         break;

    case REG_RBX:  regsTainted.push_front(REG_RBX);
    case REG_EBX:  regsTainted.push_front(REG_EBX);
    case REG_BX:   regsTainted.push_front(REG_BX);
    case REG_BH:   regsTainted.push_front(REG_BH);
    case REG_BL:   regsTainted.push_front(REG_BL);
         break;

    case REG_RCX:  regsTainted.push_front(REG_RCX); 
    case REG_ECX:  regsTainted.push_front(REG_ECX);
    case REG_CX:   regsTainted.push_front(REG_CX);
    case REG_CH:   regsTainted.push_front(REG_CH);
    case REG_CL:   regsTainted.push_front(REG_CL);
         break;

    case REG_RDX:  regsTainted.push_front(REG_RDX); 
    case REG_EDX:  regsTainted.push_front(REG_EDX); 
    case REG_DX:   regsTainted.push_front(REG_DX); 
    case REG_DH:   regsTainted.push_front(REG_DH); 
    case REG_DL:   regsTainted.push_front(REG_DL); 
         break;

    case REG_RDI:  regsTainted.push_front(REG_RDI); 
    case REG_EDI:  regsTainted.push_front(REG_EDI); 
    case REG_DI:   regsTainted.push_front(REG_DI); 
    case REG_DIL:  regsTainted.push_front(REG_DIL); 
         break;

    case REG_RSI:  regsTainted.push_front(REG_RSI); 
    case REG_ESI:  regsTainted.push_front(REG_ESI); 
    case REG_SI:   regsTainted.push_front(REG_SI); 
    case REG_SIL:  regsTainted.push_front(REG_SIL); 
         break;

    default:
      std::cout << "\t\t\t" << REG_StringShort(reg) << " can't be tainted" << std::endl;
      return false;
  }
  std::cout << "\t\t\t" << REG_StringShort(reg) << " is now tainted" << std::endl;
  return true;
}

bool removeRegTainted(REG reg)
{
  switch(reg){

    case REG_RAX:  regsTainted.remove(REG_RAX);
    case REG_EAX:  regsTainted.remove(REG_EAX);
    case REG_AX:   regsTainted.remove(REG_AX);
    case REG_AH:   regsTainted.remove(REG_AH);
    case REG_AL:   regsTainted.remove(REG_AL);
         break;

    case REG_RBX:  regsTainted.remove(REG_RBX);
    case REG_EBX:  regsTainted.remove(REG_EBX);
    case REG_BX:   regsTainted.remove(REG_BX);
    case REG_BH:   regsTainted.remove(REG_BH);
    case REG_BL:   regsTainted.remove(REG_BL);
         break;

    case REG_RCX:  regsTainted.remove(REG_RCX); 
    case REG_ECX:  regsTainted.remove(REG_ECX);
    case REG_CX:   regsTainted.remove(REG_CX);
    case REG_CH:   regsTainted.remove(REG_CH);
    case REG_CL:   regsTainted.remove(REG_CL);
         break;

    case REG_RDX:  regsTainted.remove(REG_RDX); 
    case REG_EDX:  regsTainted.remove(REG_EDX); 
    case REG_DX:   regsTainted.remove(REG_DX); 
    case REG_DH:   regsTainted.remove(REG_DH); 
    case REG_DL:   regsTainted.remove(REG_DL); 
         break;

    case REG_RDI:  regsTainted.remove(REG_RDI); 
    case REG_EDI:  regsTainted.remove(REG_EDI); 
    case REG_DI:   regsTainted.remove(REG_DI); 
    case REG_DIL:  regsTainted.remove(REG_DIL); 
         break;

    case REG_RSI:  regsTainted.remove(REG_RSI); 
    case REG_ESI:  regsTainted.remove(REG_ESI); 
    case REG_SI:   regsTainted.remove(REG_SI); 
    case REG_SIL:  regsTainted.remove(REG_SIL); 
         break;

    default:
      return false;
  }
  std::cout << "\t\t\t" << REG_StringShort(reg) << " is now freed" << std::endl;
  return true;
}

VOID ReadMem(UINT64 insAddr, std::string insDis, UINT32 opCount, REG reg_r, UINT64 memOp, UINT64 sp)
{
    list<UINT64>::iterator i;
    list<struct mallocArea>::iterator j;
    UINT64 addr = memOp;
    
    if (opCount != 2)
        return;

    for (j = mallocAreaList.begin(); j != mallocAreaList.end(); ++j) {
        if (addr >= j->base && addr < (j->base + j->size) && j->check != CHECKED) {
            std::cout << std::hex << "[WRITE in " << addr << " without check]\t\t" << insAddr << ": " << insDis << std::endl;
            return;
        }
    }

    for(i = addressTainted.begin(); i != addressTainted.end(); ++i) {
        if (addr == *i){
            std::cout << std::hex << "[READ in " << addr << "]\t" << insAddr << ": " << insDis<< std::endl;
            taintReg(reg_r);
            return;
        }
    }

    /* if mem != tained and reg == taint => free the reg */
    if (checkAlreadyRegTainted(reg_r)) {
        std::cout << std::hex << "[READ in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
        removeRegTainted(reg_r);
    }
}

VOID WriteMem(UINT64 insAddr, std::string insDis, UINT32 opCount, REG reg_r, UINT64 memOp, UINT64 sp)
{
    list<UINT64>::iterator i;
    list<struct mallocArea>::iterator j;
    UINT64 addr = memOp;

    if (opCount != 2)
        return;
    
    for (j = mallocAreaList.begin(); j != mallocAreaList.end(); ++j) {
        if (addr >= j->base && addr < (j->base + j->size) && j->check != CHECKED) {
            std::cout << std::hex << "[WRITE in " << addr << " without check]\t\t" << insAddr << ": " << insDis << std::endl;
            return;
        }
    }

    for(i = addressTainted.begin(); i != addressTainted.end(); ++i) {
        if (addr == *i){
            std::cout << std::hex << "[WRITE in " << addr << "]\t" << insAddr << ": " << insDis<< std::endl;
            if (!REG_valid(reg_r) || !checkAlreadyRegTainted(reg_r))
                removeMemTainted(addr);
            return;
        }
    }

    if (checkAlreadyRegTainted(reg_r)) {
        std::cout << std::hex << "[WRITE in " << addr << "]\t" << insAddr << ": " << insDis << std::endl;
        addMemTainted(addr);
    }
}

VOID spreadRegTaint(UINT64 insAddr, std::string insDis, UINT32 opCount, REG reg_r, REG reg_w)
{
    if (opCount != 2)
        return;
    
    if (REG_valid(reg_w)) {
        if (checkAlreadyRegTainted(reg_w) && (!REG_valid(reg_r) || !checkAlreadyRegTainted(reg_r))) {
            std::cout << "[SPREAD]\t\t" << insAddr << ": " << insDis << std::endl;
            std::cout << "\t\t\toutput: "<< REG_StringShort(reg_w) << " | input: " << (REG_valid(reg_r) ? REG_StringShort(reg_r) : "constant") << std::endl;
            removeRegTainted(reg_w);
        }
        else if (checkAlreadyRegTainted(reg_r) && !checkAlreadyRegTainted(reg_w)) {
            std::cout << "[SPREAD]\t\t" << insAddr << ": " << insDis << std::endl;
            std::cout << "\t\t\toutput: " << REG_StringShort(reg_w) << " | input: " << REG_StringShort(reg_r) << std::endl;
            taintReg(reg_w);
        }
    }
}

VOID cmpInst(UINT64 insAddr, std::string insDis, UINT64 memOp) {
    list<struct mallocArea>::iterator i;
    UINT64 addr = memOp;

    for (i = mallocAreaList.begin(); i != mallocAreaList.end(); i++) {
        if (addr == i->base) {
            std::cout << std::hex << "[PTR " << *(UINT64 *)addr << " checked]\t\t\t" << insAddr << ": " << insDis << std::endl;
            i->check = CHECKED;
        }
    }
}

VOID testInst(UINT64 insAddr, std::string insDis, ADDRINT val_r0, ADDRINT val_r1) {
    list<struct mallocArea>::iterator i;
    
    for (i = mallocAreaList.begin(); i != mallocAreaList.end(); i++) {
        if (val_r0 == val_r1 && val_r0 == i->base) {
            std::cout << std::hex << "[PTR " << val_r0 << " checked]\t\t\t" << insAddr << ": " << insDis << std::endl;
            i->check = CHECKED;
        }
    }
} 

VOID Instruction(INS ins, VOID *v)
{
    if (INS_OperandCount(ins) > 1 && INS_MemoryOperandIsRead(ins, 0) && INS_OperandIsReg(ins, 0)){
        INS_InsertCall(
            ins, IPOINT_BEFORE, (AFUNPTR)ReadMem,
            IARG_ADDRINT, INS_Address(ins),
            IARG_PTR, new string(INS_Disassemble(ins)),
            IARG_UINT32, INS_OperandCount(ins),
            IARG_UINT32, INS_OperandReg(ins, 0),
            IARG_MEMORYOP_EA, 0,
            IARG_REG_VALUE, REG_STACK_PTR,
            IARG_END);
    }
    else if (INS_OperandCount(ins) > 1 && INS_MemoryOperandIsWritten(ins, 0)){
        INS_InsertCall(
            ins, IPOINT_BEFORE, (AFUNPTR)WriteMem,
            IARG_ADDRINT, INS_Address(ins),
            IARG_PTR, new string(INS_Disassemble(ins)),
            IARG_UINT32, INS_OperandCount(ins),
            IARG_UINT32, INS_OperandReg(ins, 1),
            IARG_MEMORYOP_EA, 0,
            IARG_REG_VALUE, REG_STACK_PTR,
            IARG_END);
    }
    else if (INS_OperandCount(ins) > 1 && INS_OperandIsReg(ins, 0)){
        INS_InsertCall(
            ins, IPOINT_BEFORE, (AFUNPTR)spreadRegTaint,
            IARG_ADDRINT, INS_Address(ins),
            IARG_PTR, new string(INS_Disassemble(ins)),
            IARG_UINT32, INS_OperandCount(ins),
            IARG_UINT32, INS_RegR(ins, 0),
            IARG_UINT32, INS_RegW(ins, 0),
            IARG_END);
    }
    else if (INS_Opcode(ins) == XED_ICLASS_CMP && INS_OperandIsMemory(ins, 0)){
        INS_InsertCall(
            ins, IPOINT_BEFORE, (AFUNPTR)cmpInst,
            IARG_ADDRINT, INS_Address(ins),
            IARG_PTR, new string(INS_Disassemble(ins)),
            IARG_MEMORYOP_EA, 0,
            IARG_END);
    }
    else if (INS_Opcode(ins) == XED_ICLASS_TEST && INS_OperandCount(ins) >= 2 && REG_valid(INS_OperandReg(ins, 0)) && REG_valid(INS_OperandReg(ins, 1))) {
        INS_InsertCall(
            ins, IPOINT_BEFORE, (AFUNPTR)testInst,
            IARG_ADDRINT, INS_Address(ins),
            IARG_PTR, new string(INS_Disassemble(ins)),
            IARG_REG_VALUE, INS_OperandReg(ins, 0),
            IARG_REG_VALUE, INS_OperandReg(ins, 1),
            IARG_END);
    }
}


VOID callbackBeforeMalloc(ADDRINT size) {
    lastSize = size;
}

VOID callbackAfterMalloc(ADDRINT addr) {
    struct mallocArea area;
    list<struct mallocArea>::iterator j;

    std::cout << "[INFO]\t\tmalloc(" << lastSize << ") = " << std::hex << addr << std::endl;

    // store struct information into list
    if (addr) {
        for (j = mallocAreaList.begin(); j != mallocAreaList.end(); ++j) {
            if (addr == j->base) {
                j->size = lastSize;
                j->status = ALLOCATE;
                j->check = !CHECKED;
                return;
            }
        }
        area.base = addr;
        area.size = lastSize;
        area.status = ALLOCATE;
        area.check = !CHECKED;
        mallocAreaList.push_front(area);
    }
}

VOID callbackBeforeFree(ADDRINT addr) {
    // set the status to free
    list<struct mallocArea>::iterator j;

    std::cout << "[INFO]\t\tfree(" << std::hex << addr << ")" << std::endl;

    for (j = mallocAreaList.begin(); j != mallocAreaList.end(); ++j) {
        if (addr == j->base) {
            j->status = FREE;
            break;
        }
    }
}

VOID Image(IMG img, VOID *v) 
{
    RTN mallocRtn = RTN_FindByName(img, "malloc");
    RTN freeRtn = RTN_FindByName(img, "free");

    if (RTN_Valid(mallocRtn)) {
        RTN_Open(mallocRtn);

        RTN_InsertCall(
            mallocRtn, IPOINT_BEFORE, (AFUNPTR)callbackBeforeMalloc,
            IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
            IARG_END);
        
        RTN_InsertCall(
            mallocRtn, IPOINT_AFTER, (AFUNPTR)callbackAfterMalloc,
            IARG_FUNCRET_EXITPOINT_VALUE,
            IARG_END);
        
        RTN_Close(mallocRtn);
    }

    if (RTN_Valid(freeRtn)) {
        RTN_Open(freeRtn);

        RTN_InsertCall(
            freeRtn, IPOINT_BEFORE, (AFUNPTR)callbackBeforeFree,
            IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
            IARG_END);
        
        RTN_Close(freeRtn);
    }

}

int main(int argc, char *argv[])
{
    PIN_InitSymbols();

    /* Init Pin arguments */
    if (PIN_Init(argc, argv)) {
        return Usage();
    }

    PIN_SetSyntaxIntel();

    IMG_AddInstrumentFunction(Image, 0);

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);
    
    /* Start the program */
    PIN_StartProgram();

    return 0;

}