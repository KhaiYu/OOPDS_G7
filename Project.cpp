#include <iostream>
#include <string>

// SECTION 1: CUSTOM DATA STRUCTURES - VECTOR
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

// SECTION 2: SYSTEM STACK
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

// SECTION 3: HARDWARE REGISTERS
// Written by: Tan Khai Yu

// Base class for registers 
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
    signed char getValue() const 
    {
        return value;
    }
};

// Derived class for R0-R7 registers 
class GeneralRegister : public Register {};

// SECTION 4: FLAG REGISTER STATUS
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

// SECTION 5: MAIN MEMORY (64 BYTES)
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

// SECTION 6: INSTRUCTION BASE CLASS
// Written by: Tan Khai Yu

class CPU; // Forward declaration

// Abstract base class for commands
class Instruction 
{
public:
    virtual ~Instruction() {}
    
    // Pure virtual function for polymorphism
    virtual void execute(CPU& cpu) = 0; 
};