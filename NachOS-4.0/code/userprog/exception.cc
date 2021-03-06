// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

void ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);
    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which)
    {
    case NoException:
        return;
        break;

    case PageFaultException:
        cerr << "Page fault exception\n";
        SysHalt();
        break;

    case ReadOnlyException:
        cerr << "Read only exception\n";
        SysHalt();
        break;

    case BusErrorException:
        cerr << "Bus error exception\n";
        SysHalt();
        break;

    case AddressErrorException:
        cerr << "Address error exception\n";
        SysHalt();
        break;

    case OverflowException:
        cerr << "Overflow exception\n";
        SysHalt();
        break;

    case IllegalInstrException:
        cerr << "Illegal instruction exception\n";
        SysHalt();
        break;

    case NumExceptionTypes:
        cerr << "Number exception\n";
        SysHalt();
        break;

    case SyscallException:
        switch (type)
        {
        case SC_Halt:
            DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

            SysHalt();

            ASSERTNOTREACHED();
            break;

        case SC_Add:
            DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

            /* Process SysAdd Systemcall*/
            int result;
            result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
                            /* int op2 */ (int)kernel->machine->ReadRegister(5));

            DEBUG(dbgSys, "Add returning with " << result << "\n");
            /* Prepare Result */
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;

        case SC_ReadNum:
        {
            DEBUG(dbgSys, "Read number\n");

            int result;
            result = SysReadNum();

            DEBUG(dbgSys, "SysReadNum returning with " << result << "\n");
            /* Prepare Result */
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_PrintNum:
            DEBUG(dbgSys, "PrintNum " << kernel->machine->ReadRegister(4) << "\n");
            SysPrintNum((int)kernel->machine->ReadRegister(4));

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;

        case SC_ReadChar:
        {
            DEBUG(dbgSys, "Read character\n");
            char result;
            result = SysReadChar();

            DEBUG(dbgSys, "SysReadChar returning with " << result << "\n");
            /* Prepare Result */
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_PrintChar:
            DEBUG(dbgSys, "Print character: " << kernel->machine->ReadRegister(4) << "\n");

            SysPrintChar(kernel->machine->ReadRegister(4));

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;

        case SC_RandomNum:
        {
            DEBUG(dbgSys, "Random number\n");

            int result;
            result = SysRandomNum();

            DEBUG(dbgSys, "SysRandomNum returning with " << result << "\n");
            /* Prepare Result */
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_ReadString:
        {
            DEBUG(dbgSys, "Read string\n");
            SysReadString((char *) kernel->machine->ReadRegister(4), (int) kernel->machine->ReadRegister(5));

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_PrintString:
            DEBUG(dbgSys, "PrintString " << kernel->machine->ReadRegister(4) << "\n");
            SysPrintString((int)kernel->machine->ReadRegister(4));

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;

            ASSERTNOTREACHED();

            break;

        case SC_CreateFile:
        {
            DEBUG(dbgSys, "Create file\n");
            result = SysCreateFile((int)kernel->machine->ReadRegister(4));
            
            DEBUG(dbgSys, "SysCreateFile returning with " << result << "\n");
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_Open:
        {
            DEBUG(dbgSys, "Open file " << kernel->machine->ReadRegister(4) << "\n" << kernel->machine->ReadRegister(5));
            result = SysOpen(kernel->machine->ReadRegister(4), kernel->machine->ReadRegister(5));

            DEBUG(dbgSys, "Open file returning with " << result << "\n");
            /* Prepare Result */
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;

            ASSERTNOTREACHED();
            break;
        }

        case SC_Close:
        {
            DEBUG(dbgSys, "Close file " << kernel->machine->ReadRegister(4) << "\n");
            result = SysClose(kernel->machine->ReadRegister(4));

            DEBUG(dbgSys, "Close file returning with " << result << "\n");
            /* Prepare Result */
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;

            ASSERTNOTREACHED();
            break;
        }

        case SC_Read:
        {
            DEBUG(dbgSys, "Read file\n");
            int result;
            result = SysRead((int) kernel->machine->ReadRegister(4), (int) kernel->machine->ReadRegister(5), (int) kernel->machine->ReadRegister(6));

            DEBUG(dbgSys, "Read file returning with " << result << "\n");
            /* Prepare Result */
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_Write:
        {
            DEBUG(dbgSys, "Write file\n");
            int result;
            result = SysWrite((int) kernel->machine->ReadRegister(4), (int) kernel->machine->ReadRegister(5), (int) kernel->machine->ReadRegister(6));

            DEBUG(dbgSys, "Write file returning with " << result << "\n");
            /* Prepare Result */
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_Exec:
        {
            DEBUG(dbgSys, "Exec\n");
            int result;
            result = SysExec((int) kernel->machine->ReadRegister(4));
            
            DEBUG(dbgSys, "SysExec returning with " << result << "\n");
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_Join:
        {
            DEBUG(dbgSys, "Join\n");
            result = SysJoin((int)kernel->machine->ReadRegister(4));

            DEBUG(dbgSys, "SysJoin returning with " << result << "\n");
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_Exit:
        {
            DEBUG(dbgSys, "Exit\n");
            SysExit((int)kernel->machine->ReadRegister(4));

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_CreateSemaphore:
        {
            DEBUG(dbgSys, "CreateSemaphore\n");
            int result;
            result = SysCreateSemaphore((int)kernel->machine->ReadRegister(4), (int)kernel->machine->ReadRegister(5));
            
            DEBUG(dbgSys, "SysCreateSemaphore returning with " << result << "\n");
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_Signal:
        {
            DEBUG(dbgSys, "Signal\n");
            int result;
            result = SysSignal((int)kernel->machine->ReadRegister(4));
            
            DEBUG(dbgSys, "SysSignal returning with " << result << "\n");
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_Wait:
        {
            DEBUG(dbgSys, "Wait\n");
            int result;
            result = SysWait((int)kernel->machine->ReadRegister(4));
            
            DEBUG(dbgSys, "SysWait returning with " << result << "\n");
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_GetCurrentProcessId:
        {
            DEBUG(dbgSys, "Get current process ID\n");
            int result;
            result = SysGetCurrentProcessId();
            
            DEBUG(dbgSys, "SysGetCurrentProcessId returning with " << result << "\n");
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                /* set next programm counter for brach execution */
                kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            }

            return;
            ASSERTNOTREACHED();
            break;
        }

        default:
            cerr << "Unexpected system call " << type << "\n";
            break;
        }
        break;

    default:
        cerr << "Unexpected user mode exception" << (int)which << "\n";
        break;
    }
    ASSERTNOTREACHED();
}
