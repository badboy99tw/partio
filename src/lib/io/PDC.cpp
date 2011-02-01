#include "../Partio.h"
#include "../core/ParticleHeaders.h"
#include "endian.h"
#include "ZIP.h"

#include <iostream>
#include <fstream>
#include <string>
//#include <cassert>
#include <memory>

namespace Partio{

using namespace std;

typedef struct{
    int magic;
    int version;
    int bitorder;
    int tmp1;
    int tmp2;
    int numParticles;
    int numAttrs;
} PDC_HEADER;

string readName(istream& input){
    int nameLen = 0;
    read<BIGEND>(input, nameLen);
    char* name = new char[nameLen];
    input.read(name, nameLen);
    string result(name, name+nameLen);
    delete [] name;
    return result;
}

static const int MC_MAGIC = (((((' '<<8)|'C')<<8)|'D')<<8)|'P'; // " CDP"
ParticlesDataMutable* readPDC(const char* filename, const bool headersOnly){

    auto_ptr<istream> input(Gzip_In(filename,std::ios::in|std::ios::binary));
    if(!*input){
        std::cerr << "Partio: Unable to open file " << filename << std::endl;
        return 0;
    }

    PDC_HEADER header;
    input->read((char*)&header, sizeof(header)); 
    if(MC_MAGIC != header.magic){
        std::cerr << "Partio: Magic number '" << header.magic << "' of '" << filename << "' doesn't match mc magic '" << MC_MAGIC << "'" << std::endl;
        return 0;
    }

    BIGEND::swap(header.numParticles);
    BIGEND::swap(header.numAttrs);

    ParticlesDataMutable* simple=0;
    if(headersOnly){
        simple = new ParticleHeaders;
    } 
    else{
        simple = create();
    }

    simple->addParticles(header.numParticles);

    for(int i = 0; i < header.numAttrs; i++){
        string attrName = readName(*input);
        int type;
        read<BIGEND>(*input, type);
        if(type == 3){
            ParticleAttribute attrHandle = simple->addAttribute(attrName.c_str(), FLOAT, 1);
            if(headersOnly){
                input->seekg((int)input->tellg() + header.numParticles*sizeof(double));
                continue;
            }
            else{
                double tmp;
                for(int j = 0; j < simple->numParticles(); j++){
                    read<BIGEND>(*input, tmp);
                    simple->dataWrite<float>(attrHandle, j)[0] = tmp;
                }
            }
        }
        else if(type == 5){
            ParticleAttribute attrHandle = simple->addAttribute(attrName.c_str(), VECTOR, 3);
            if(headersOnly){
                input->seekg((int)input->tellg() + header.numParticles*sizeof(double)*attrHandle.count);
                continue;
            }
            else{
                for(int j = 0; j < simple->numParticles(); j++){
                    double tmp[3];
                    read<BIGEND>(*input, tmp[0], tmp[1], tmp[2]);
                    float* data = simple->dataWrite<float>(attrHandle, j);
                    data[0] = tmp[0];
                    data[1] = tmp[1];
                    data[2] = tmp[2];
                }
            }
        }
        cout << attrName << " " << type << endl;
    }

    return simple;
}

bool writePDC(const char* filename,const ParticlesData& p,const bool compressed){
    return 0;
}

}// end of namespace Partio
