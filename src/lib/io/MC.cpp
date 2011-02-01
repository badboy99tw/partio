#include "../Partio.h"
#include "../core/ParticleHeaders.h"
#include "endian.h" // read/write big-endian file
#include "ZIP.h" // for zip file

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <memory>

namespace Partio{

using namespace std;

static const int MC_MAGIC = ((((('F'<<8)|'O')<<8)|'R')<<8)|'4';

typedef struct{
    int magic;
    int headerSize;
    int blockSize;
} MC_HEADER;

typedef struct{
    string name;
    string type;
    unsigned int numParticles;
    unsigned int blockSize;
} ATTR_HEADER;

bool readStr(istream& input, string& target, unsigned int size){
    char* tmp = new char [size];
    input.read(tmp, size);
    target = string(tmp, tmp+size);
    delete [] tmp;
    return true;
}

bool readMcHeader(istream& input, MC_HEADER& mc){
    read<BIGEND>(input, mc.magic);

    int headerSize;
    read<BIGEND>(input, headerSize);
    input.seekg((int)input.tellg() + headerSize);

    char tag[4];
    input.read(tag, 4); // FOR4
    read<BIGEND>(input, mc.blockSize);
    input.read(tag, 4); // MYCH

    mc.headerSize = 4 + 4 + headerSize + 4 + 4 + 4; // magic + sizeof(int) + blockSize + FOR4 + blocksize + MYCH
    mc.blockSize = mc.blockSize - 4; // minus 4 bytes for MYCH

    return true;
}

bool readAttrHeader(istream& input, ATTR_HEADER& attr){
    char tag[4];
    input.read(tag, 4); // tag CHNM

    // read attribute name
    int chnmSize;
    read<BIGEND>(input, chnmSize);
    if(chnmSize%4 > 0){
        chnmSize = chnmSize - chnmSize%4 + 4;
    }
    readStr(input, attr.name, chnmSize);
    attr.name = attr.name.substr(attr.name.find_first_of("_")+1);

    input.read(tag, 4); // tag SIZE
    int dummy;
    read<BIGEND>(input, dummy); // 4
    read<BIGEND>(input, attr.numParticles);
    readStr(input, attr.type, 4);
    read<BIGEND>(input, attr.blockSize);

    return true;
}

// maybe replaced by reading xml file.
bool mcInfo(const char* filename, ParticlesDataMutable* simple){
    auto_ptr<istream> input(Gzip_In(filename,ios::in|ios::binary));
    MC_HEADER header;
    readMcHeader(*input, header);
    int numParticles = 0;
    while((int)input->tellg()-header.headerSize < header.blockSize){
        ATTR_HEADER attrHeader;
        readAttrHeader(*input, attrHeader);
        input->seekg((int)input->tellg() + attrHeader.blockSize);

        if(attrHeader.name == string("id")){
            numParticles = attrHeader.numParticles;
        }
        if(attrHeader.name == string("count")){
            continue;
        }
        // add attribute
        if(attrHeader.type == std::string("FVCA")){
            simple->addAttribute(attrHeader.name.c_str(), VECTOR, 3);
        }
        else if(attrHeader.type == std::string("DBLA")){
            simple->addAttribute(attrHeader.name.c_str(), FLOAT, 1);
        }
    }
    simple->addParticles(numParticles);
    return true;
}

ParticlesDataMutable* readMC(const char* filename, const bool headersOnly){

    std::auto_ptr<std::istream> input(Gzip_In(filename,std::ios::in|std::ios::binary));
    if(!*input){
        std::cerr << "Partio: Unable to open file " << filename << std::endl;
        return 0;
    }

    MC_HEADER header;
    readMcHeader(*input, header);
    if(MC_MAGIC != header.magic){
        std::cerr << "Partio: Magic number '" << header.magic << "' of '" << filename << "' doesn't match mc magic '" << MC_MAGIC << "'" << std::endl;
        return 0;
    }
    
    ParticlesDataMutable* simple = headersOnly ? new ParticleHeaders: create();
    mcInfo(filename, simple);

    if(headersOnly){
        return simple;
    }
     
    input->seekg(header.headerSize);
    while((int)input->tellg()-header.headerSize < header.blockSize){
        ATTR_HEADER attrHeader;
        readAttrHeader(*input, attrHeader);

        ParticleAttribute attr;
        if(simple->attributeInfo(attrHeader.name.c_str(), attr) == false){
            input->seekg((int)input->tellg() + attrHeader.blockSize);
            continue;
        }

        if(attr.type == FLOAT){
            double tmp;
            for(int partIndex = 0; partIndex < simple->numParticles(); partIndex++){
                read<BIGEND>(*input, tmp);
                simple->dataWrite<float>(attr, partIndex)[0] = (float)tmp;
            }
        }
        else if(attr.type == VECTOR){
            float tmp[3];
            for(int partIndex = 0; partIndex < simple->numParticles(); partIndex++){
                read<BIGEND>(*input, tmp[0], tmp[1], tmp[2]);
                memcpy(simple->dataWrite<float>(attr, partIndex), tmp, sizeof(float)*attr.count);
            }
        }
    }
    return simple; 
}

}// end of namespace Partio
