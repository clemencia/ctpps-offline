/****************************************************************************
* This module is directly copied from 
* RPRecoDataFormats/RPTrackCandidateCollection.h and expanded to hold 
* multiple track candidates.
*   
* Original Authors:
*   Hubert Niewiadomski (Hubert.Niewiadomski@cern.ch)
* Secondary Authors:
*   Zhang Zhengkui (zhang.zhengkui.fin@gmail.com)
*
****************************************************************************/


#ifndef RecoTotemRP_RPRecoDataFormats_RPMulTrackCandidateCollection_h
#define RecoTotemRP_RPRecoDataFormats_RPMulTrackCandidateCollection_h
 
#include <vector>
#include <map>

// TODO
typedef int RPTrackCandidate;

class RPMulTrackCandidateCollection : public std::map<unsigned int, std::vector<RPTrackCandidate> >
{
};

class RPMulTrackCandidateSetsCollection : public std::map<unsigned int, std::vector<std::vector<RPTrackCandidate> > >
{
};

#endif
