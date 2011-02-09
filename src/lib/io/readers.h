/*
 * <b>CONFIDENTIAL INFORMATION: This software is the confidential and
 * proprietary information of Walt Disney Animation Studios ("Disney").
 * This software is owned by Disney and may not be used, disclosed,
 * reproduced or distributed for any purpose without prior written
 * authorization and license from Disney. Reproduction of any section of
 * this software must include this legend and all copyright notices.
 * (c) Disney. All rights reserved.</b>
 *
 */
#ifndef _READERS_h_
#define _READERS_h_

namespace Partio{
ParticlesDataMutable* readBGEO(const char* filename,const bool headersOnly);
ParticlesDataMutable* readGEO(const char* filename,const bool headersOnly);
ParticlesDataMutable* readPDB(const char* filename,const bool headersOnly);
ParticlesDataMutable* readPDA(const char* filename,const bool headersOnly);
ParticlesDataMutable* readPTC(const char* filename,const bool headersOnly);
ParticlesDataMutable* readMC(const char* filename,const bool headersOnly);
ParticlesDataMutable* readPDC(const char* filename,const bool headersOnly);
ParticlesDataMutable* readBIN(const char* filename,const bool headersOnly);
//ParticlesDataMutable* readPRD(const char* filename,const bool headersOnly);
bool writeBGEO(const char* filename,const ParticlesData& p,const bool compressed);
bool writeGEO(const char* filename,const ParticlesData& p,const bool compressed);
bool writePDB(const char* filename,const ParticlesData& p,const bool compressed);
bool writePDA(const char* filename,const ParticlesData& p,const bool compressed);
bool writePTC(const char* filename,const ParticlesData& p,const bool compressed);
bool writePDC(const char* filename,const ParticlesData& p,const bool compressed);
bool writeBIN(const char* filename,const ParticlesData& p,const bool compressed);
//bool writePRD(const char* filename,const ParticlesData& p,const bool compressed);
}

#endif
