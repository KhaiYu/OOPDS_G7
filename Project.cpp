/***********************************************************************
CCP6124 Object-oriented Programming and Data Structures
Trimester 2610
Project: Virtual Machine and Assembly Language Interpreter

Tutorial: TT8L
Group: 7

Member 1: 252UC2543V, Wong Haw Jack, WONG.HAW.JACK@student.mmu.edu.my
Member 2: 252UC2546J, Chen Chee Chuen, CHEN.CHEE.CHUEN@student.mmu.edu.my
Member 3: 252UC2528G, Tan Yi Da, TAN.YI.DA@student.mmu.edu.my
Member 4: 253UC256L3, Tan Khai Yu, TAN.KHAI.YU@student.mmu.edu.my

This program strictly follow below program structure:
- Custom Data Structures
- Virtual Machine Architecture
- Assembly Language Runner (Interpreter)
- Runner

************************************************************************/

#include <iostream>
#include <string>

//*****************************************************************
//              SECTION 1: CUSTOM DATA STRUCTURES
//*****************************************************************
// 1.1: SYSTEM VECTOR
// Written by: Tan Khai Yu

template <class DataType>
class MyVector
{
private:
    DataType* arr;     // Box to hold our actual items
    int capacity;      // Total space we opened in total
    int current;       // Spaces we actually used right now

    // Grow the box size when it gets full
    void resize()
    {
        int NewCapacity;

        if (capacity == 0)
        {
            NewCapacity = 1;
        }
        else
        {
            NewCapacity = capacity * 2;
        }

        DataType* t = new DataType[NewCapacity];

        for (int i = 0; i < current; i++)
        {
            t[i] = arr[i];
        }

        delete[] arr;
        arr = t;
        capacity = NewCapacity;
    }

public:
    // Setup empty values at the start
    MyVector()
    {
        arr = nullptr;
        capacity = 0;
        current = 0;
    }

    // Clear memory when program closes
    ~MyVector()
    {
        if (arr != nullptr)
        {
            delete[] arr;
        }
    }

    // Put a new item to the back
    void push_back(DataType data)
    {
        if (current == capacity)
        {
            resize();
        }
        arr[current] = data;
        current++;
    }

    // Read data using standard brackets vec[i]
    DataType& operator[](int i)
    {
        return arr[i];
    }

    // Return total items inside
    int size() const
    {
        return current;
    }
};

// 1.2: SYSTEM STACK
// Written by: Tan Khai Yu

class MyStack
{
private:
    signed char storage[8]; // Fixed array to hold 8 bytes as required
    int topIndex;           // Tracks the top item position

public:
    // Set stack as empty at first
    MyStack()
    {
        topIndex = -1;
    }

    // Put a new byte on top of the stack
    void push(signed char val)
    {
        if (topIndex < 7)
        {
            topIndex++;
            storage[topIndex] = val;
        }
    }

    // Take the top item off the stack
    signed char pop()
    {
        // Crash program if try to pop empty stack as required
        if (topIndex < 0)
        {
            std::cerr << "Stack Underflow Error!" << std::endl;
            exit(1);
        }

        signed char val = storage[topIndex];
        topIndex--;
        return val;
    }

    int get_top_index() {return topIndex;}
};
// 1.3: SYSTEM QUEUE
// Written By:

class MyQueue{};


//*****************************************************************
//             SECTION 2: VIRTUAL MACHINE ARCHITECTURE
//*****************************************************************
//
// 2.1: Register
// Written by: Tan Khai Yu
// Base class register
class Register
{
protected:
    signed char value; // 1-byte register data slot

public:
    // Setup starting value to 0
    Register()
    {
        value = 0;
    }

    virtual ~Register() {}

    // Method to update register data
    void setValue(signed char val)
    {
        value = val;
    }

    // Method to read register data
    signed char const getValue()
    {
        return value;
    }
};

// Derived class for R0-R7 registers
class GeneralRegister : public Register {};

class StackIndexRegister : public Register
{
public:
    StackIndexRegister()
    {
        value = 0;

    }
    void push()
    {
        value++;
    }
    void pop()
    {
        value--;
    }
};

// 2.2: Program Counter
// Written by:
class ProgramCounter{};

// 2.3: Flags
// Written by: Tan Khai Yu
// Manages CF, ZF, UF, OF bits
class FlagRegister
{
private:
    bool CF; // Carry Flag
    bool ZF; // Zero Flag
    bool UF; // Underflow Flag
    bool OF; // Overflow Flag

public:
    // Reset all flags to false initially
    FlagRegister()
    {
        CF = false;
        ZF = false;
        UF = false;
        OF = false;
    }

    // Update methods for flags
    void setCF(bool b) { CF = b; }
    void setZF(bool b) { ZF = b; }
    void setUF(bool b) { UF = b; }
    void setOF(bool b) { OF = b; }

    // Read methods for checking flags
    bool getCF() const { return CF; }
    bool getZF() const { return ZF; }
    bool getUF() const { return UF; }
    bool getOF() const { return OF; }
};

// 2.4: Memory(64 BYTES)
// Written by: Tan Khai Yu

// Handles storage over 64 bytes
class Memory
{
private:
    signed char storage[64]; // Fixed size 64-byte array

public:
    // Fill all memory space with 0 at first
    Memory()
    {
        for (int i = 0; i < 64; i++)
        {
            storage[i] = 0;
        }
    }

    // Write data to an address
    void write(int address, signed char val)
    {
        if (address >= 0 && address < 64)
        {
            storage[address] = val;
        }
    }

    // Read data from an address
    signed char read(int address) const
    {
        if (address >= 0 && address < 64)
        {
            return storage[address];
        }
        return 0;
    }
};

//2.5 CPU
//Written By Tan Khai YU / Wong Haw Jack
class CPU {
private:
    GeneralRegister r[8];
    Memory cpu_memory;

    FlagRegister* ptr_flag = new FlagRegister();
    ProgramCounter pc;
    MyStack stack;
    StackIndexRegister stack_index;

public:
    CPU() : stack_index() {}

    ~CPU() {delete ptr_flag;}

    signed char readMemory(int address)
    {
        return cpu_memory.read(address);
    }

    void writeMemory(int address, signed char value)
    {
        cpu_memory.write(address, value);
    }

    FlagRegister* getFlags()
    {
        return ptr_flag;
    }

    signed char getReg(int idx) const {
        if (idx >= 0 && idx < 8)
            return r[idx].getValue();
        return 0;
    }
    void setReg(int idx, signed char val) {
        if (idx >= 0 && idx < 8)
            r[idx].setValue(val);
    }


    void pushStack(signed char val) {
        stack.push(val);
        stack_index.push();
    }
    signed char popStack() {
        if (stack_index.getValue() == 0) {
            std::cerr << "Stack Underflow Error!" << std::endl;
            exit(1);
        }
        stack_index.pop();
        return stack.pop();
    }
};
// Forward declaration;
//*****************************************************************
//            SECTION 3: Assembly Language Interpreter
//*****************************************************************
// 3.1: Instruction (Base Class)
// Written by: Tan Khai Yu
// Abstract base class for commands
class Instruction
{
public:
    virtual ~Instruction() {}

    // Pure virtual function for polymorphism
    virtual void execute(CPU& cpu) = 0;
};

// 3.2: Input Operations
// 3.3: Output Operations
// Written by: Wong Haw Jack
class IOInstruction: public Instruction
{
public:
    int register_index;
};

class Input: public IOInstruction
{
    /***
    When executed, your interpreter must print a single '?' character at the start of a new line to signal it is waiting for user input.
    Input Validation: You must handle bounds validation manually.
    If the user types a value outside the signed 8-bit range (<-128 or >127), your logic must catch it and explicitly
    flip the Underflow Flag (UF) or Overflow Flag (OF) in the CPU.
    The Zero Flag (ZF): Pay very close attention to the requirement for the Zero Flag,
    If the input has an ASCII code equal to 0, the ZF flag must be set to true.
    ***/

public:
    Input(int r)
    {
        register_index = r;
    }
    void execute(CPU& cpu) override
    {
        int input;
        std::cout << "?" << std::endl;
        std::cin >> input;
        static_cast<signed char>(input);
        cpu.setReg(register_index, input);
        if(input > 127) {cpu.getFlags()->setOF(true);}
        else if(input < -128) {cpu.getFlags()->setUF(true);}
        else if(input == 0) {cpu.getFlags()->setZF(true);}
    }

};

class Display: public IOInstruction
{
public:
    Display(int r)
    {
        register_index = r;
    }
    void execute(CPU& cpu) override
    {
        std::cout << int(cpu.getReg(register_index));
    }
};
// 3.4: MOV Operations
// Written by: Tan Yi Da
class MovOperation:public Instruction
{
private:
int mov_mode; //MOV mode(1.R1,10 / 2.R1,R0 / 3.R1,[R2])
int destination; //destination of register
int source; // which register to MOV to the destination
signed char number; //number (-128 to 127) to MOV to destination

public:
    MovOperation(int dest, signed char num)
    {
        mov_mode=0;
        destination=dest;
        number=num;
    }
    MovOperation(int dest, int src) //same with mode 2
    {
        mov_mode=1;
        destination=dest;
        source=src;
        }
    MovOperation(int dest, int src, int nums) //same with mode 1 but have int num is just to differentiate mode 1 and 2
    {
        mov_mode=2;
        destination=dest;
        source=src;
    }
    void execute(CPU& cpu)
    {
        switch(mov_mode)
        {
        case 0:
            cpu.register[destination].setValue(number);//still need to modify
        break;
        case 1:
            cpu.register[destination].setValue(source);//still need to modify
        break;
        case 2:
            cpu.register[destination].setValue(memory.read(address));//still need to modify
        break;
        default:
            cout<<"You cant MOVE this!!!"<<endl;
            return;//still need to use class Flag
        }
    };

};
// 3.5: Arithmetic Operations
// Written by: Wong Haw Jack
class Arithmetic: public Instruction
{
private:
    int source_reg;
    int destination_reg;

public:
    Arithmetic(int d, int s): source_reg(s), destination_reg(d){}

    int get_source_reg() const {return source_reg;}
    int get_destination_reg() const {return destination_reg;}
};

class ADD: public Arithmetic
{
public:
    ADD(int d, int s): Arithmetic(d, s){}

    void execute(CPU& cpu)
    {
        int source_index = get_source_reg();
        int dest_index = get_destination_reg();

        signed char source_value = cpu.getReg(source_index);
        signed char dest_value = cpu.getReg(dest_index);

        int signed_result = int(source_value) + int(dest_value);

        unsigned char u_source_value = (unsigned char)source_value;
        unsigned char u_dest_value = (unsigned char)dest_value;

        unsigned char unsigned_result = u_dest_value + u_source_value;

        cpu.getFlags()->setCF(unsigned_result > 255);
        cpu.getFlags()->setUF(signed_result < -128);
        cpu.getFlags()->setOF(signed_result > 127);
        cpu.getFlags()->setZF(signed_result == 0);

        cpu.setReg(dest_index, static_cast<signed char>(signed_result));
    }
};

class SUB: public Arithmetic
{
public:
    SUB(int d, int s): Arithmetic(d, s){}

    void execute(CPU& cpu)
    {
        int source_index = get_source_reg();
        int dest_index = get_destination_reg();

        signed char source_value = cpu.getReg(source_index);
        signed char dest_value = cpu.getReg(dest_index);

        int signed_result = int(dest_value) - int(source_value);

        unsigned char u_source_value = (unsigned char)source_value;
        unsigned char u_dest_value = (unsigned char)dest_value;

        unsigned char unsigned_result = u_dest_value - u_source_value;

        cpu.getFlags()->setCF(unsigned_result > 255);
        cpu.getFlags()->setUF(signed_result < -128);
        cpu.getFlags()->setOF(signed_result > 127);
        cpu.getFlags()->setZF(signed_result == 0);

        cpu.setReg(dest_index, static_cast<signed char>(signed_result));
    }
};

class MUL: public Arithmetic
{
public:
    MUL(int d, int s): Arithmetic(d, s){}

    void execute(CPU& cpu)
    {
        int source_index = get_source_reg();
        int dest_index = get_destination_reg();

        signed char source_value = cpu.getReg(source_index);
        signed char dest_value = cpu.getReg(dest_index);

        int signed_result = int(dest_value) * int(source_value);

        unsigned char u_source_value = (unsigned char)source_value;
        unsigned char u_dest_value = (unsigned char)dest_value;

        unsigned char unsigned_result = u_dest_value * u_source_value;

        cpu.getFlags()->setCF(unsigned_result > 255);
        cpu.getFlags()->setUF(signed_result < -128);
        cpu.getFlags()->setOF(signed_result > 127);
        cpu.getFlags()->setZF(signed_result == 0);

        cpu.setReg(dest_index, static_cast<signed char>(signed_result));
    }
};

class DIV: public Arithmetic
{
public:
    DIV(int d, int s): Arithmetic(d, s){}

    void execute(CPU& cpu)
    {
        int source_index = get_source_reg();
        int dest_index = get_destination_reg();

        signed char source_value = cpu.getReg(source_index);
        signed char dest_value = cpu.getReg(dest_index);

        int signed_result = int(dest_value) / int(source_value) ;

        unsigned char u_source_value = (unsigned char)source_value;
        unsigned char u_dest_value = (unsigned char)dest_value;

        unsigned char unsigned_result =  u_dest_value / u_source_value ;

        cpu.getFlags()->setCF(unsigned_result > 255);
        cpu.getFlags()->setUF(signed_result < -128);
        cpu.getFlags()->setOF(signed_result > 127);
        cpu.getFlags()->setZF(signed_result == 0);

        cpu.setReg(dest_index, static_cast<signed char>(signed_result));
    }
};
// 3.6: Increment and Decrement Operations
// Written by: Chen Chee Chuen
class INC : public Instruction
{
private:
    int reg;

public:
    INC(int r)
    {
        reg = r;
    }

    void execute(CPU& cpu) override
    {
        int result = cpu.getReg(reg) + 1;

        // Reset Flags
        cpu.getFlags()->setOF(false);
        cpu.getFlags()->setUF(false);
        cpu.getFlags()->setZF(false);
        cpu.getFlags()->setCF(false);

        // Overflow
        if(result > 127)
        {
            cpu.getFlags()->setOF(true);
            cpu.getFlags()->setCF(true);
        }

        // Underflow
        if(result < -128)
        {
            cpu.getFlags()->setUF(true);
            cpu.getFlags()->setCF(true);
        }

        if((signed char)result == 0)
        {
            cpu.getFlags()->setZF(true);
        }

        cpu.setReg(reg, (signed char)result);
    }
};

class DEC : public Instruction
{
private:
    int reg;

public:
    DEC(int r)
    {
        reg = r;
    }

    void execute(CPU& cpu) override
    {
        int result = cpu.getReg(reg) - 1;

        // Reset Flags
        cpu.getFlags()->setOF(false);
        cpu.getFlags()->setUF(false);
        cpu.getFlags()->setZF(false);
        cpu.getFlags()->setCF(false);

        // Overflow
        if(result > 127)
        {
            cpu.getFlags()->setOF(true);
            cpu.getFlags()->setCF(true);
        }

        // Underflow
        if(result < -128)
        {
            cpu.getFlags()->setUF(true);
            cpu.getFlags()->setCF(true);
        }

        // Zero Flag
        if((signed char)result == 0)
        {
            cpu.getFlags()->setZF(true);
        }

        cpu.setReg(reg, (signed char)result);
    }
};

// 3.7: ROL Operations
// 3.8: ROR Operations
// Written by Jack Wong
class Rotate : public Instruction
{
private:
    int destination_reg;
    int count;

public:
    Rotate(int d, int c)
    {
        destination_reg = d;
        count = c;
    }

    int get_dest_reg() const {return destination_reg;}
    int get_count() const {return count;}
};

class ROL : public Rotate
{
public:
    ROL(int d, int c) : Rotate(d, c){}

    void execute(CPU& cpu)
    {
        int destination_index = get_dest_reg();
        int count = get_count();

        count %= 8;

        signed char signed_before_ro = cpu.getReg(destination_index);
        unsigned char unsigned_before_ro = static_cast<unsigned char>(signed_before_ro);

        unsigned char unsigned_after_ro = 0;
        if (count == 0)
        {
            unsigned_after_ro = unsigned_before_ro;
        }
        else
        {
            unsigned_after_ro = (unsigned_before_ro << count) | (unsigned_before_ro >> (8 - count));
        }
        signed char signed_after_ro = static_cast<signed char>(unsigned_after_ro);

        cpu.setReg(destination_index, signed_after_ro);

        cpu.getFlags()->setZF(signed_after_ro == 0);
    }
};
class ROR : public Rotate
{
public:
    ROR(int d, int c) : Rotate(d, c){}

    void execute(CPU& cpu)
    {
        int destination_index = get_dest_reg();
        int count = get_count();

        count %= 8;

        signed char signed_before_ro = cpu.getReg(destination_index);
        unsigned char unsigned_before_ro = static_cast<unsigned char>(signed_before_ro);

        unsigned char unsigned_after_ro = 0;
        if (count == 0)
        {
            unsigned_after_ro = unsigned_before_ro;
        }
        else
        {
            unsigned_after_ro = (unsigned_before_ro >> count) | (unsigned_before_ro << (8 - count));
        }
        signed char signed_after_ro = static_cast<signed char>(unsigned_after_ro);

        cpu.setReg(destination_index, signed_after_ro);

        cpu.getFlags()->setZF(signed_after_ro == 0);
    }
};
// 3.9: Shift Operations
// Written By: Tan Yi Da
class SftOperation:public Instruction
{
    private:
        int sft_mode;
        int sft_num;
        int destination;
    public:
        SftOperation(int dest,int Count,int mode)
        {
            sft_mode=mode;
            destination=dest;
            sft_num=Count;
    }
    void execute(CPU& cpu)
    {
        switch(sft_mode)
        {
            case 0: //case 0 is SHL
            for(int i=0; i<sft_num;i++)
            {
                signed char val=cpu.registers[destination].getValue();
                val *=2;
                cpu.write[destination].setValue(val);
            }
            break;

            case 1: //case 0 is SHR
            for(int i=0; i<sft_num;i++)
            {
                signed char val=cpu.registers[destination].getValue();
                val /=2;
                cpu.write[destination].setValue(val);
            }
            break;
        }
    }
};



// 3.10: Load and Store Operations
// Written by: Chen Chee Chuen
class LOAD : public Instruction  //Direct LOAD // LOAD R1, [20]
{
private:
    int reg;
    int address;
public:
    LOAD(int r, int addr)
    {
        reg = r;
        address = addr;
    }

    void execute(CPU& cpu)override
    {
        if(address < 0 || address >= 64)
        {
            std::cout << "Memory Error" << std::endl;
            return;
        }

        cpu.setReg(reg, cpu.readMemory(address));
    }
};

class STORE : public Instruction  // Direct STORE
{
private:
    int reg;
    int address;

public:
    STORE(int r, int addr)
    {
        reg = r;
        address = addr;
    }

    void execute(CPU& cpu) override
    {
        if(address < 0 || address >= 64)
        {
            std::cout << "Memory Error" << std::endl;
            return;
        }

        cpu.writeMemory(address, cpu.getReg(reg));
    }
};

class LOAD_INDIRECT : public Instruction  // Indirect load  // LOAD R1, [R2]
{
private:
    int destReg;
    int addrReg;

public:
    LOAD_INDIRECT(int d, int a)
    {
        destReg = d;
        addrReg = a;
    }

    void execute(CPU& cpu) override
    {
        int address = cpu.getReg(addrReg);

        if(address < 0 || address >= 64)
        {
            std::cout << "Memory Error" << std::endl;
            return;
        }

        cpu.setReg(destReg, cpu.readMemory(address));
    }
};

class STORE_INDIRECT : public Instruction  // Indirect store
{
private:
    int addrReg;
    int sourceReg;

public:
    STORE_INDIRECT(int a, int s)
    {
        addrReg = a;
        sourceReg = s;
    }

    void execute(CPU& cpu) override
    {
        int address = cpu.getReg(addrReg);

        if(address < 0 || address >= 64)
        {
            std::cout << "Memory Error" << std::endl;
            return;
        }

        cpu.writeMemory(address, cpu.getReg(sourceReg));
    }
};



// 3.11: Flag Reset Instruction
// Written by: Tan Khai Yu
// Resets a specific flag (C, Z, U, or O) to false
class RESET : public Instruction
{
private:
    char FlagType;  // Flag to reset: 'C', 'Z', 'U', or 'O'

public:
    RESET(char Type) : FlagType(Type) {}

    void execute(CPU& cpu) override
    {
        FlagRegister* flags = cpu.getFlags();
        switch (FlagType)
        {
        case 'C': flags->setCF(false); break;
        case 'Z': flags->setZF(false); break;
        case 'U': flags->setUF(false); break;
        case 'O': flags->setOF(false); break;
        default: break;
        }
    }
};

// 3.12: Stack Operations
// Written by: Tan Khai Yu
// PUSH: Stores a register's value onto the stack
class PUSH : public Instruction
{
private:
    int Source_Reg;  // Register to push (0-7)

public:
    PUSH(int Reg) : Source_Reg(Reg) {}

    void execute(CPU& cpu)
    {
        signed char value = cpu.getReg(Source_Reg);
        cpu.pushStack(value);  // Push value onto stack
    }
};

// POP: Removes top stack value and stores it into a register
class POP : public Instruction
{
private:
    int Destination_Reg;  // Register to receive popped value (0-7)

public:
    POP(int Reg) : Destination_Reg(Reg) {}

    void execute(CPU& cpu)
    {
        signed char value = cpu.popStack();  // Pop from stack
        cpu.setReg(Destination_Reg, value);  // Store in register
    }
};
//*****************************************************************
//                       SECTION 4: Runner
//*****************************************************************

    /***
    Below is just for bug testing!!!
    These are NOT actual code.
    Feel free to make changes for debugging purpose.
    ***/
int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << " RUNNING VIRTUAL MACHINE TEST SUITE     " << std::endl;
    std::cout << "========================================" << std::endl;

    CPU myCpu;

    //---------------------------------------------------------
    // TEST 1: Register Boundary & Explicit IO Flag Manipulations
    //---------------------------------------------------------
    {
        std::cout << "\n[TEST 1] Testing IO boundaries & Flags..." << std::endl;

        Instruction* inputNormal = new Input(0);
        std::cout << "-> Enter '55' to test normal assignment:" << std::endl;
        inputNormal->execute(myCpu);

        Instruction* displayR0 = new Display(0);
        std::cout << "Value inside R0 is: ";
        displayR0->execute(myCpu);
        std::cout << std::endl;

        Instruction* inputOverflow = new Input(1);
        std::cout << "-> Enter '200' to test Overflow Flag trigger:" << std::endl;
        inputOverflow->execute(myCpu);

        std::cout << "Overflow Flag (OF) state (Expected 1): " << myCpu.getFlags()->getOF() << std::endl;

        delete inputNormal;
        delete displayR0;
        delete inputOverflow;
    }

    //---------------------------------------------------------
    // TEST 2: Reset Flag Instruction
    //---------------------------------------------------------
    {
        std::cout << "\n[TEST 2] Testing RESET instruction..." << std::endl;

        Instruction* resetOF = new RESET('O');
        resetOF->execute(myCpu);
        std::cout << "Overflow Flag (OF) state after RESET (Expected 0): " << myCpu.getFlags()->getOF() << std::endl;

        delete resetOF;
    }

    //---------------------------------------------------------
    // TEST 3: Stack Operations (PUSH & POP)
    //---------------------------------------------------------
    {
        std::cout << "\n[TEST 3] Testing Stack Operations..." << std::endl;

        myCpu.setReg(2, 42);
        myCpu.setReg(3, 0);

        Instruction* pushR2 = new PUSH(2);
        Instruction* popR3 = new POP(3);

        pushR2->execute(myCpu);
        popR3->execute(myCpu);

        std::cout << "Value transferred to R3 via Stack (Expected 42): " << (int)myCpu.getReg(3) << std::endl;

        delete pushR2;
        delete popR3;
    }

    //---------------------------------------------------------
    // TEST 4: Multiple PUSH/POP Operations (LIFO Order)
    //---------------------------------------------------------
    {
        std::cout << "\n[TEST 4] Testing Multiple PUSH/POP (LIFO Order)..." << std::endl;

        for(int i = 0; i < 8; i++) {
            myCpu.setReg(i, 0);
        }

        myCpu.setReg(0, 10);
        myCpu.setReg(1, 20);
        myCpu.setReg(2, 30);
        myCpu.setReg(3, 40);

        std::cout << "Initial values: R0=10, R1=20, R2=30, R3=40" << std::endl;

        Instruction* pushR0 = new PUSH(0);
        Instruction* pushR1 = new PUSH(1);
        Instruction* pushR2 = new PUSH(2);
        Instruction* pushR3 = new PUSH(3);

        pushR0->execute(myCpu);
        pushR1->execute(myCpu);
        pushR2->execute(myCpu);
        pushR3->execute(myCpu);

        std::cout << "Pushed: R0, R1, R2, R3 onto stack" << std::endl;

        Instruction* popR4 = new POP(4);
        Instruction* popR5 = new POP(5);
        Instruction* popR6 = new POP(6);
        Instruction* popR7 = new POP(7);

        popR4->execute(myCpu);
        popR5->execute(myCpu);
        popR6->execute(myCpu);
        popR7->execute(myCpu);

        std::cout << "Popped into R4,R5,R6,R7 (Expected LIFO: 40,30,20,10)" << std::endl;
        std::cout << "Result: R4=" << (int)myCpu.getReg(4)
                  << " R5=" << (int)myCpu.getReg(5)
                  << " R6=" << (int)myCpu.getReg(6)
                  << " R7=" << (int)myCpu.getReg(7) << std::endl;

        bool lifoCorrect = ((int)myCpu.getReg(4) == 40 &&
                            (int)myCpu.getReg(5) == 30 &&
                            (int)myCpu.getReg(6) == 20 &&
                            (int)myCpu.getReg(7) == 10);

        if(lifoCorrect) {
            std::cout << " LIFO order test PASSED!" << std::endl;
        } else {
            std::cout << " LIFO order test FAILED!" << std::endl;
        }

        delete pushR0;
        delete pushR1;
        delete pushR2;
        delete pushR3;
        delete popR4;
        delete popR5;
        delete popR6;
        delete popR7;
    }

    //---------------------------------------------------------
    // TEST 5: Reset ALL Flags (C, Z, U, O)
    //---------------------------------------------------------
    {
        std::cout << "\n[TEST 5] Testing RESET on all flag types..." << std::endl;

        FlagRegister* flags = myCpu.getFlags();

        flags->setCF(true);
        flags->setZF(true);
        flags->setUF(true);
        flags->setOF(true);

        std::cout << "Before RESET - CF:" << flags->getCF()
                  << " ZF:" << flags->getZF()
                  << " UF:" << flags->getUF()
                  << " OF:" << flags->getOF() << " (Expected: 1 1 1 1)" << std::endl;

        Instruction* resetCF = new RESET('C');
        Instruction* resetZF = new RESET('Z');
        Instruction* resetUF = new RESET('U');
        Instruction* resetOF2 = new RESET('O');

        resetCF->execute(myCpu);
        std::cout << "After RESET 'C' - CF:" << flags->getCF() << " (Expected: 0)" << std::endl;

        resetZF->execute(myCpu);
        std::cout << "After RESET 'Z' - ZF:" << flags->getZF() << " (Expected: 0)" << std::endl;

        resetUF->execute(myCpu);
        std::cout << "After RESET 'U' - UF:" << flags->getUF() << " (Expected: 0)" << std::endl;

        resetOF2->execute(myCpu);
        std::cout << "After RESET 'O' - OF:" << flags->getOF() << " (Expected: 0)" << std::endl;

        if(!flags->getCF() && !flags->getZF() && !flags->getUF() && !flags->getOF()) {
            std::cout << " All flags reset successfully!" << std::endl;
        } else {
            std::cout << " Flag reset test FAILED!" << std::endl;
        }

        delete resetCF;
        delete resetZF;
        delete resetUF;
        delete resetOF2;
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << " TEST SUITE COMPLETE                     " << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
