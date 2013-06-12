#pragma once
#ifndef DAVIX_DAVIXURI_HPP
#define DAVIX_DAVIXURI_HPP

#include <string>
#include <status/davixstatusrequest.hpp>


/**
  @file davixuri.hpp
  @author Devresse Adrien
  @brief URI utilities functions for davix
 */

#ifndef __DAVIX_INSIDE__
#error "Only davix.h or davix.hpp should be included."
#endif


namespace Davix {

struct UriPrivate;

/// @class Uri
/// @brief Uri parser
/// convenience class for uri parsing
///
class DAVIX_EXPORT Uri
{
public:
    /// Construct an empty invalid Uri
    Uri();
    /// construct a new Davix Uri from a string URL
    Uri(const std::string & uri_string);
    /// Copy constructor
    Uri(const Uri & uri);
    ///
    /// \brief assignment operator
    /// \param orig
    /// \return
    ///
    Uri & operator=(const Uri & orig);
    virtual ~Uri();

    /// get a string representation of the full uri
    /// @return return the path or an empty string if error
    const std::string & getString() const;

    /// get the port number
    /// @return return the  port number of 0 if error
    int getPort() const;

    /// get the protocol scheme
    ///  @return return the protocol scheme or an empty string if error
    const std::string & getProtocol() const;

    /// get the host name
    /// @return return the hostname or an empty string if error
    const std::string & getHost() const;

    /// get the path part of the Uri
    /// @return return the path of the Uri or an empty string if error
    const std::string & getPath()const;

    /// get a concatenation of the path and the query argument of the URI
    /// @return return a path + query arguments concatenation or an empty string if error
    const std::string & getPathAndQuery() const;

    /// get the query argument part of the uri
    /// @return return the query path string or an empty string if not exist or if error
    const std::string & getQuery() const;

    /// Status of the Uri
    /// see StatusCode::Code
    /// @return StatusCode::OK if success or StatusCode::UriParsingError if error
    StatusCode::Code getStatus() const;

    ///
    /// \brief test if two URI are equals
    /// \param u1
    /// \return true if equal, else false
    ///
    bool equal(const Uri & u1) const;

    ///
    /// \brief compare oepration
    /// \param u2
    /// \return true if u2 == current uri
    ///
    bool operator==(const Uri & u2) const;


    ///
    /// \brief Escape string
    /// \param str URL to escape
    /// \return encoded string
    ///
    static std::string escapeString(const std::string & str);

    ///
    /// \brief Unescape url
    /// \param str URL to escape
    /// \return unencoded string
    ///
    static std::string unescapeString(const std::string & str);

private:
    UriPrivate* d_ptr;
    void _init();
};


///
/// check the validity of a Davix::Uri
/// @param uri : davix uri
/// @param err : Davix Error report object
/// @return true if the uri is valid, or false and setup err with a string expression
bool DAVIX_EXPORT uriCheckError(const Uri & uri, DavixError ** err);

} // namespace Davix


#endif // DAVIX_DAVIXURI_HPP
