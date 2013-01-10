#include "neonsession.hpp"

#include <string>
#include <cstring>

#include <davix_context_internal.hpp>
#include <ne_redirect.h>
#include <libs/time_utils.h>

#include <auth/davixx509cred_internal.hpp>


namespace Davix{


static int validate_all_certificate(void *userdata, int failures,
                                const ne_ssl_certificate *cert){
    return 0;
}

const int n_max_auth = 20;


void NEONSession::provide_clicert_fn(void *userdata, ne_session *sess,
                                         const ne_ssl_dname *const *dnames,
                                         int dncount){

    NEONSession* req = static_cast<NEONSession*>(userdata);
    DavixError* tmp_err=NULL;

    X509Credential cert;
    std::pair<authCallbackClientCertX509,void*> retcallback = req->_params.getClientCertCallbackX509();
    DAVIX_DEBUG("NEONSession > clicert callback ");
    if( retcallback.first != NULL){
        DAVIX_DEBUG("NEONSession > call client cert callback ");
        SessionInfo infos;


        if( retcallback.first(retcallback.second, infos, &cert, &tmp_err) != 0 || cert.hasCert() == false){
            if(!tmp_err)
                DavixError::setupError(&(req->_last_error), davix_scope_http_request(), StatusCode::AuthentificationError,
                                       "No valid credential given ");
             return;
        }

        ne_ssl_set_clicert(req->_sess, X509CredentialExtra::extract_ne_ssl_clicert(cert));
        DAVIX_DEBUG("NEONSession > end call client cert callback");
    }
    return;
}

int NEONSession::provide_login_passwd_fn(void *userdata, const char *realm, int attempt,
                                char *username, char *password){
    NEONSession * req = static_cast<NEONSession*>(userdata);
    DavixError * tmp_err=NULL;
    int ret =-1;
    std::string tmp_login, tmp_password;

    DAVIX_DEBUG("NEONSession > Try to get auth/password authentification ");

     if(attempt > n_max_auth ){
         DavixError::setupError(&(req->_last_error), davix_scope_http_request(), StatusCode::LoginPasswordError,
                                "Overpass maximum number of try for login/password authentication ");
     }

     const std::pair<authCallbackLoginPasswordBasic, void*> retcallback(req->_params.getClientLoginPasswordCallback());
     const std::pair<std::string , std::string > id(req->_params.getClientLoginPassword());
     if(retcallback.first != NULL){
         DAVIX_DEBUG("NEONSession > Try callback for login/passwd for %d time", attempt+1);
         SessionInfo infos;

         if( (ret = retcallback.first(retcallback.second, infos, tmp_login, tmp_password, attempt, &tmp_err) ) <0){
             if(!tmp_err)
                 DavixError::setupError(&tmp_err, davix_scope_http_request(), StatusCode::LoginPasswordError,
                                        "No valid login/passwd given in after ");
              DavixError::propagateError(&(req->_last_error), tmp_err);
              return -1;
         }
     }else if(id.first.empty() == false){
        tmp_login = id.first;
        tmp_password = id.second;
     }

    if( tmp_login.empty()
        || tmp_password.empty() ){
        DAVIX_DEBUG("NEONSession > no login/passwd : abort ");
        DavixError::setupError(&(req->_last_error), davix_scope_http_request(),
                               StatusCode::LoginPasswordError,
                               "Server requested login/password authentication and no valid login/password have been given");
        return -1;
    }
    DAVIX_DEBUG("NEONSession > setup authentification pwd/login....");
    g_strlcpy(username, tmp_login.c_str(), NE_ABUFSIZ);
    g_strlcpy(password, tmp_password.c_str(), NE_ABUFSIZ);
    req->_login.clear();
    req->_passwd.clear();
    DAVIX_DEBUG("NEONSession > get login/password with success...try server submission ");
    return 0;

}



NEONSession::NEONSession(Context & c, const Uri & uri, const RequestParams & p, DavixError** err) :
    _f(ContextExplorer::SessionFactoryFromContext(c)),
    _sess(NULL),
    _params(p),
    _last_error(NULL),
    _login(),
    _passwd()
{
        _f.createNeonSession(uri, &_sess, err);
        if(_sess)
            configureSession(_sess, p, &NEONSession::provide_login_passwd_fn, this, &NEONSession::provide_clicert_fn, this);
}


NEONSession::NEONSession(NEONSessionFactory & f, const Uri & uri, const RequestParams & p, DavixError** err) :
    _f(f),
    _sess(NULL),
    _params(p),
    _last_error(NULL),
    _login(),
    _passwd()
{
    _f.createNeonSession(uri, &_sess, err);
    if(_sess)
        configureSession(_sess, p, &NEONSession::provide_login_passwd_fn, this, &NEONSession::provide_clicert_fn, this);
}


NEONSession::~NEONSession(){
#   ifndef _DISABLE_SESSION_REUSE
        if(_sess)
         _f.storeNeonSession(_sess, NULL);
#   endif

}


void configureSession(ne_session *_sess, const RequestParams &params, ne_auth_creds lp_callback, void* lp_userdata,
                      ne_ssl_provide_fn cred_callback,  void* cred_userdata){
    if(strcmp(ne_get_scheme(_sess), "https") ==0) // fix a libneon bug with non ssl connexion
        ne_ssl_trust_default_ca(_sess);

    // register redirection management
    ne_redirect_register(_sess);

    // define user agent
    ne_set_useragent(_sess, params.getUserAgent().c_str());

    if(params.getSSLCACheck() == false){ // configure ssl check
        DAVIX_DEBUG("NEONRequest : disable ssl verification");
        ne_ssl_set_verify(_sess, validate_all_certificate, NULL);
    }

    // if authentification for login/password
    if( params.getClientLoginPassword().first.empty() == false
            || params.getClientLoginPasswordCallback().first != NULL){
        DAVIX_DEBUG("NEONSession : enable login/password authentication");
        ne_set_server_auth(_sess, lp_callback, lp_userdata);
    }else{
        DAVIX_DEBUG("NEONSession : disable login/password authentication");
    }

    // if authentification for cli cert by callback
    if( params.getClientCertCallbackX509().first != NULL){
        DAVIX_DEBUG("NEONSession : enable client cert authentication by callback ");
        ne_ssl_provide_clicert(_sess, cred_callback, cred_userdata);
    }else if( params.getClientCertX509().hasCert()){
        ne_ssl_set_clicert(_sess, X509CredentialExtra::extract_ne_ssl_clicert(params.getClientCertX509()));
        DAVIX_DEBUG("NEONSession : enable client cert authentication with predefined cert");
    }else{
          DAVIX_DEBUG("NEONSession : disable client cert authentication");
    }

    if( timespec_isset(params.getOperationTimeout())){
        DAVIX_DEBUG("NEONSession : define operation timeout to %d", params.getOperationTimeout());
        ne_set_read_timeout(_sess, (int) params.getOperationTimeout()->tv_sec);
    }
    if(timespec_isset(params.getConnectionTimeout())){
        DAVIX_DEBUG("NEONSession : define connection timeout to %d", params.getConnectionTimeout());
#ifndef _NEON_VERSION_0_25
        ne_set_connect_timeout(_sess, (int) params.getConnectionTimeout()->tv_sec);
#endif
    }

    ne_set_session_flag(_sess, NE_SESSFLAG_PERSIST, params.getKeepAlive());

}





}