#include <string>
#pragma once
namespace http {
    namespace mime_types {
        struct mapping {
            const char *extension;
            const char *mime_type;
        } mappings[] =
                {
                        {"gif",  "image/gif"},
                        {"htm",  "text/html"},
                        {"html", "text/html"},
                        {"jpg",  "image/jpeg"},
                        {"png",  "image/png"},
                        {0,      0} // Marks end of list.
                };

        std::string extension_to_type(const std::string &extension) noexcept {
            for (mapping *m = mappings; m->extension; ++m) {
                if (m->extension == extension) {
                    return m->mime_type;
                }
            }

            return "text/plain";
        }
    }
}