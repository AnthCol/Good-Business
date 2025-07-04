#include "common.h"

int main(int argc, char** argv) {
    std::string path = "./SampleFiles/2ouvenir.70.brstm"; 
    std::ifstream fptr(path, std::ios::binary); 

    int fileSize = std::filesystem::file_size(path); 

    std::vector<char> buffer(fileSize); 

    fptr.read(buffer.data(), buffer.size());    


    BRSTM brstm; 

    brstm.parse(buffer); 



    return 0; 
}


