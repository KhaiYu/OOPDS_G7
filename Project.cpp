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
- CPU
- Runner

************************************************************************/

#include <iostream>
#include <string>
using namespace std;
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
//*****************************************************************
//            SECTION 3: Assembly Language Interpreter
//*****************************************************************
// 3.1: Instruction (Base Class)
// Written by: Tan Khai Yu
// Abstract base class for commands

class CPU; // Forward declaration
class Instruction
{
public:
    virtual ~Instruction() {}

    // Pure virtual function for polymorphism
    virtual void execute(CPU& cpu) = 0;
};
// 3.2: Input Operations
// 3.3: Output Operations
// 3.4: MOV Operations
// Written by: Tan Yi Da
class MovOperation : public Instruction
{
private:
    int mov_mode; // MOV mode (0: R1,10 / 1: R1,R0 / 2: R1,[R2])
    int destination;
    int source;
    signed char number;

public:

    MovOperation(int dest, signed char num)
    {
        mov_mode = 0;
        destination = dest;
        number = num;
        source = 0;
    }

    // Mode 1: MOV R1, R0 (Copies the value stored in the source register to the destination register)
    MovOperation(int dest, int src)
    {
        mov_mode = 1;
        destination = dest;
        source = src;
        number = 0;
    }

    // Mode 2: MOV R1, [R2] (Reads data from the memory address stored in the source register and copies it to the destination register)
    // An extra dummy parameter (int mode) is added to differentiate the overload from the two-argument constructor
    MovOperation(int dest, int src, int mode)
    {
        mov_mode = 2;
        destination = dest;
        source = src;
        number = 0;
    }

    void execute(CPU& cpu) override
    {
        switch(mov_mode)
        {
            case 0:
                cpu.setReg(destination, number);
                break;
            case 1:
                cpu.setReg(destination, cpu.getReg(source));
                break;
            case 2:
                {
                    // Fetch the memory address value stored inside the source register
                    int address = static_cast<int>(cpu.getReg(source));
                    if (address >= 0 && address < 64)
                    {
                        cpu.setReg(destination, cpu.readMemory(address));
                    }
                    else
                    {
                        std::cerr << "Memory Access Error in MOV Instruction!" << std::endl;
                    }
                }
                break;
            default:
                std::cout << "You can't MOVE this!!!" << std::endl;
                return;
        }
    }
};

// 3.5: Arithmetic Operations
// 3.6: Increment and Decrement Operations
// 3.7: ROL Operations
// 3.8: ROR Operations
// 3.9: Shift Operations
// 3.9: Shift Operations
// Written By: Tan Yi Da
class SftOperation : public Instruction
{
private:
    int sft_mode; // 0: SHL (Shift Left), 1: SHR (Shift Right)
    int sft_num; // Number of shifts
    int destination; // Destination register index

public:
    SftOperation(int dest, int Count, int mode)
    {
        sft_mode = mode;
        destination = dest;
        sft_num = Count;
    }

    void execute(CPU& cpu) override
    {
        // Retrieve the original value of the register via the public interface
        signed char val = cpu.getReg(destination);

        switch(sft_mode)
        {
            case 0: // SHL: Logical Shift Left
                for(int i = 0; i < sft_num; i++)
                {
                    val = val << 1;
                }
                break;

            case 1: // SHR: Logical Shift Right
                // Casting to unsigned char ensures that the higher bits are padded with 0s during right shift (logical shift)
                unsigned char uVal = static_cast<unsigned char>(val);
                for(int i = 0; i < sft_num; i++)
                {
                    uVal = uVal >> 1;
                }
                val = static_cast<signed char>(uVal);
                break;
        }

        // Store the shifted value back to the register and update the Zero Flag (ZF)
        cpu.setReg(destination, val);
        cpu.getFlags()->setZF(val == 0);
    }
};



// 3.10: Load and Store Operations
// 3.11: Flag Reset Instruction
// 3.12: Stack Operations
// 3.13： Parser Class
//Writen by : Tan Yi Da

class Parser {
private:
    string line;
    string ins;

    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

public:
    Parser() {
        line = " ";
        ins = " ";
    }

    Instruction* parseLine(const string& inputLine) {
        size_t spacePos = inputLine.find_first_of(" \t");

        if (spacePos == string::npos) {
            string cmd = trim(inputLine);
            if (cmd == "RESET") return new RESET();
            return nullptr;
        }

        string opcode = inputLine.substr(0, spacePos);
        string args = trim(inputLine.substr(spacePos + 1));

        size_t commaPos = args.find(',');

        if (commaPos == string::npos) {
            if (opcode == "INPUT")   return new INPUT(args[1] - '0');
            if (opcode == "POP")     return new POP(args[1] - '0');
            if (opcode == "INC")     return new INC(args[1] - '0');
            if (opcode == "DEC")     return new DEC(args[1] - '0');

            if (opcode == "PUSH") {
                if (args[0] == 'R') return new PUSH_REG(args[1] - '0');
                return new PUSH_NUM((signed char)stoi(args));
            }
            if (opcode == "DISPLAY") {
                if (args[0] == '[') return new DISPLAY_MEM(stoi(args.substr(1, args.length() - 2)));
                return new DISPLAY_REG(args[1] - '0');
            }
            return nullptr;
        }

        string first = trim(args.substr(0, commaPos));
        string second = trim(args.substr(commaPos + 1));

        if (opcode == "ADD")   return new ADD(first[1] - '0', second[1] - '0');
        if (opcode == "SUB")   return new SUB(first[1] - '0', second[1] - '0');
        if (opcode == "MUL")   return new MUL(first[1] - '0', second[1] - '0');
        if (opcode == "DIV")   return new DIV(first[1] - '0', second[1] - '0');
        if (opcode == "ROL")   return new ROL(first[1] - '0', stoi(second));
        if (opcode == "ROR")   return new ROR(first[1] - '0', stoi(second));
        if (opcode == "SHL")   return new SftOperation(first[1] - '0', stoi(second), 0);
        if (opcode == "SHR")   return new SftOperation(first[1] - '0', stoi(second), 1);

        if (opcode == "MOV") {
            if (second[0] == 'R') return new MovOperation(first[1] - '0', second[1] - '0');
            if (second[0] == '[') return new MovOperation(first[1] - '0', second[2] - '0', 0);
            return new MovOperation(first[1] - '0', (signed char)stoi(second));
        }

        if (opcode == "LOAD") {
            if (second[1] == 'R') return new LOAD_INDIRECT(first[1] - '0', second[2] - '0');
            return new LOAD(first[1] - '0', stoi(second.substr(1, second.length() - 2)));
        }

        if (opcode == "STORE") {
            int srcReg = second[1] - '0';
            if (first[1] == 'R') return new STORE_INDIRECT(first[2] - '0', srcReg);
            return new STORE(srcReg, stoi(first.substr(1, first.length() - 2)));
        }

        return nullptr;
    }
};

//*****************************************************************
//                         SECTION 4: CPU
//*****************************************************************

class CPU; // Forward declaration

//*****************************************************************
//                       SECTION 5: Runner
//*****************************************************************

int main()
{
    return 0;
}
