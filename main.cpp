#include "brstm.h"

int main(int argc, char** argv) {
    // std::string path = "./SampleFiles/2ouvenir.70.brstm"; 
    std::string path = "./SampleFiles/j1tters.60.brstm"; 
    std::ifstream fptr(path, std::ios::binary); 

    int fileSize = std::filesystem::file_size(path); 

    std::vector<char> buffer (fileSize); 

    fptr.read(buffer.data(), buffer.size());    


    BRSTM brstm; 
    brstm.parse(buffer); 



    // At this point the brstm object is filled 
    // with the relevant data

    /*
        pcm = brstm.decode();  
        mp3 = pcm.encodeToMP3(); 

        std::ofstream output("output.mp3", std::ios::binary); 
        output.write(mp3)
    */

    return 0; 
}


