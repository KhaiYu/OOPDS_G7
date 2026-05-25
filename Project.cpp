/***********************************************************************
CCP6124 Object-oriented Programming and Data Structures
Trimester 2610
Project: Virtual Machine and Assembly Language Interpreter

Tutorial: TT8L
Group: 7

Member 1: 252UC2543V, Wong Haw Jack, WONG.HAW.JACK@student.mmu.edu.my
Member 2: 252UC2546J, Chen Chee Chuen, CHEN.CHEE.CHUEN@student.mmu.edu.my
Member 3: 252UC2528G, Tan Yi Da, TAN.YI.DA@student.mmu.edu.my
Member 4: ... , Tan Khai Yu, TAN.KHAI.YU@student.mmu.edu.my

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
private:
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
    signed char getValue() const
    {
        return value;
    }
};

// Derived class for R0-R7 registers
class GeneralRegister : public Register {};

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

class CPU
{
public:
    GeneralRegister r[8];
    Memory cpu_memory;

    FlagRegister* ptr_flag = new FlagRegister();
    ProgramCounter pc;
    Register stack_index;

    MyStack cpu_stack;

    ~CPU()
    {
        delete ptr_flag;
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
        cpu.r[register_index].setValue(input);
        if(input > 127) {cpu.ptr_flag->setOF(true);}
        else if(input < -128) {cpu.ptr_flag->setUF(true);}
        else if(input == 0) {cpu.ptr_flag->setZF(true);}
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
        std::cout << int(cpu.r[register_index].getValue());
    }
};
// 3.4: MOV Operations
// 3.5: Arithmetic Operations
// 3.6: Increment and Decrement Operations
// 3.7: ROL Operations
// 3.8: ROR Operations
// 3.9: Shift Operations
// 3.10: Load and Store Operations
// 3.11: Flag Reset Instruction
// 3.12: Stack Operations


//*****************************************************************
//                       SECTION 5: Runner
//*****************************************************************

int main()
{
//test case
    CPU myCpu;
//    myCpu.r[0].setValue(129);
//    std::cout << int(myCpu.r[0].getValue()) << std::endl;
//    std::cout << int(myCpu.stack_index.getValue()) << std::endl;
//
//    std::cout << myCpu.ptr_flag->getOF() << std::endl;
//    std::cout << myCpu.ptr_flag->getUF() << std::endl;
//    std::cout << myCpu.ptr_flag->getCF() << std::endl;
//    std::cout << myCpu.ptr_flag->getZF() << std::endl;
//
//    myCpu.cpu_stack.push(10);
//    myCpu.cpu_stack.push(20);
//    myCpu.cpu_stack.push(30);
//
//    std::cout << myCpu.cpu_stack.get_top_index() << std::endl;
//
//    myCpu.cpu_stack.pop();
//
//    std::cout << myCpu.cpu_stack.get_top_index() << std::endl;
//
    Instruction* instruct1 = new Input(0);
    Instruction* instruct2 = new Display(0);
    instruct1->execute(myCpu);
    instruct2->execute(myCpu);

    return 0;
}
