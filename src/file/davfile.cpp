#include "davfile.hpp"


namespace Davix{

struct DavFileInternal{
    DavFileInternal(Context & c, const Uri & u) :
        _c(c), _u(u) {}

    Context & _c;
    Uri _u;
};


DavFile::DavFile(Context &c, const Uri &u) :
    d_ptr(new DavFileInternal(c,u))
{

}

DavFile::~DavFile(){
    delete d_ptr;
}


ssize_t DavFile::getAllReplicas(const RequestParams &params, ReplicaVec &vec, DavixError **err){

}


} //Davix