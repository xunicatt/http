#ifndef LIME_HTTP_RESPONSE_H
#define LIME_HTTP_RESPONSE_H

#include <string>
#include <unordered_map>

#include "status.h"

namespace lime {
  namespace http {
    using Header = std::unordered_map<std::string, std::string>;

    struct Response {
      public:
      /*
      * @brief Create http reponse with http::StatusOk and the given message.
      * @param body Body of the http response.
      */
      Response(const std::string& body);

      /*
      * @brief Create http reponse with http statuscode.
      * @param status_code Status code of the http response.
      */
      Response(const StatusCode& status_code);

      /*
      * @brief Create http reponse with http response body & http statuscode.
      * @param body Body of the http response.
      * @param status_code Status code of the http response.
      */
      Response(const std::string&,const StatusCode&);

      /*
      * @brief Append http options to the headers of the response.
      * @param id Name of the key.
      * @param value Value of the field.
      */
      void append_header(const std::string& id, const std::string& value);

      /*
      * @brief Set the message body of the response.
      * @param body Body of the http response.
      */
      void set_body(const std::string& body);

      /*
      * @brief Set the statuscode of the response.
      * @param status_code Status code of the http response.
      */
      void set_code(const StatusCode& status_code);

      /*
      * @brief Convert http response to string.
      * @return String fromat of the http reponse.
      */
      [[nodiscard]]
      std::string to_string() const;

      private:
      std::string m_body;
      StatusCode  m_code;
      Header      m_header;
    };
  } // http
} // lime

#endif // LIME_HTTP_REQUEST_H
