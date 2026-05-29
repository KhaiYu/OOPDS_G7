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

    signed char const getReg(int idx) {
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
// 3.5: Arithmetic Operations
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
// 3.9: Shift Operations
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
//Written by :Tan Khai Yu
class RESET : public Instruction
{
private:
    char FlagType;
public:
    RESET(char Type): FlagType(Type){}
    void execute(CPU& cpu) override
    {
        FlagRegister* flags = cpu.getFlags();
        switch (FlagType)
        {
        case 'C':
        {
            flags->setCF(false);
        }
            break;
        case 'Z':
        {
            flags->setZF(false);
        }
            break;
        case 'U':
        {
            flags->setUF(false);
        }
            break;
        case 'O':
        {
            flags->setOF(false);
        }
            break;
        default:
        break;
        }
    }
};
// 3.12: Stack Operations
//Written by:Tan Khai Yu
class PUSH: public Instruction{
    private:
     int Source_Reg;
    public:
    PUSH(int Reg):Source_Reg(Reg){}
    void execute(CPU& cpu){
        signed char value = cpu.getReg(Source_Reg);
        cpu.pushStack(value);
    }
};
class POP : public Instruction{
    private:
     int Destination_Reg;
    public:
    POP(int Reg):Destination_Reg(Reg){}
    void execute(CPU& cpu){
        signed char value = cpu.popStack();
         cpu.setReg(Destination_Reg, value);
    }
};

//*****************************************************************
//                       SECTION 4: Runner
//*****************************************************************


int main()
{
    /***
    Below is just for bug testing!!!
    These are NOT actual code.
    Feel free to make changes for debugging purpose.
    ***/
    std::cout << "========================================" << std::endl;
    std::cout << " RUNNING VIRTUAL MACHINE TEST SUITE     " << std::endl;
    std::cout << "========================================" << std::endl;

    CPU myCpu;

    //---------------------------------------------------------
    // TEST 1: Register Boundary & Explicit IO Flag Manipulations
    //---------------------------------------------------------
    std::cout << "\n[TEST 1] Testing IO boundaries & Flags..." << std::endl;

    // Test Input Instruction (Normal range)
    // Run this manually: Type '55' when prompted
    Instruction* inputNormal = new Input(0); // Targets R0
    std::cout << "-> Enter '55' to test normal assignment:" << std::endl;
    inputNormal->execute(myCpu);

    Instruction* displayR0 = new Display(0);
    std::cout << "Value inside R0 is: ";
    displayR0->execute(myCpu);
    std::cout << std::endl;

    // Test Input Instruction (Overflow boundary trigger)
    // Run this manually: Type '200' when prompted (Exceeds signed char 127)
    Instruction* inputOverflow = new Input(1); // Targets R1
    std::cout << "-> Enter '200' to test Overflow Flag trigger:" << std::endl;
    inputOverflow->execute(myCpu);

    std::cout << "Overflow Flag (OF) state (Expected 1): " << myCpu.getFlags()->getOF() << std::endl;

    //---------------------------------------------------------
    // TEST 2: Reset Flag Instruction
    //---------------------------------------------------------
    std::cout << "\n[TEST 2] Testing RESET instruction..." << std::endl;

    Instruction* resetOF = new RESET('O');
    resetOF->execute(myCpu);
    std::cout << "Overflow Flag (OF) state after RESET (Expected 0): " << myCpu.getFlags()->getOF() << std::endl;

    //---------------------------------------------------------
    // TEST 3: Stack Operations (PUSH & POP)
    //---------------------------------------------------------
    std::cout << "\n[TEST 3] Testing Stack Operations..." << std::endl;

    // Prepare values via direct set methods to bypass std::cin during structural tests
    myCpu.setReg(2, 42);  // Place 42 into R2
    myCpu.setReg(3, 0);   // Clear R3

    Instruction* pushR2 = new PUSH(2); // Push R2 (42) onto stack
    Instruction* popR3 = new POP(3);   // Pop top stack value into R3

    pushR2->execute(myCpu);
    popR3->execute(myCpu);

    std::cout << "Value transferred to R3 via Stack (Expected 42): " << (int)myCpu.getReg(3) << std::endl;

    //---------------------------------------------------------
    // CLEANUP
    //---------------------------------------------------------
    delete inputNormal;
    delete displayR0;
    delete inputOverflow;
    delete resetOF;
    delete pushR2;
    delete popR3;

    std::cout << "\n========================================" << std::endl;
    std::cout << " TEST SUITE COMPLETE                     " << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
