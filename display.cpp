#include <iomanip>

// Runner Memory Matrix Display
void printMemoryToConsole(CPU& cpu){
    std::cout << "#Memory#\n";
    for (int row = 0; row < 8; ++row){
        std::cout << "#";
        for (int col = 0; col < 8; ++col){
            int address = (row * 8) + col;
            int memVal = static_cast<int>(cpu.readMemory(address));
            if(memVal >= 0){
                std::cout <<std::setw(4) << std::setfill('0') << memVal << "#";
            }
            else{
                std::cout << "-" << std::setw(3) << std::setfill('0') << (-memVal) << "#";
            }
        }
        std::cout << "\n";
    }
}

//Virtual Machine Snapshot State Display
void dumpStateToConsole(CPU& cpu,int pcValue){
    std::cout << "#Begin#\n#Registers#";
    for(int i = 0; i < 8; ++i){
        int regVal = static_cast<int>(cpu.getReg(i));
        if (regVal >= 0){
            std::cout << std::setw(4) << std::setfill('0') << regVal << "#";
        }
        else{
            std::cout << "-" << std::setw(3) << std::setfill('0') << (-regVal) << "#";
        }
    }
    std::cout << "\n";

    FlagRegister* flags = cpu.getFlags();
    std::cout << "#FLAGS#OF#" << flags -> getOF()
              << "#UF#" << flags -> getUF()
              << "#CF#" << flags -> getCF()
              << "#ZF#" << flags -> getZF() << "#\n";
    
    std::cout << "#PC#" << std::setw(4) << std::setfill('0') << pcValue << "#\n";
    printMemoryToConsole(cpu);
    std::cout << "#End#\n";
}
