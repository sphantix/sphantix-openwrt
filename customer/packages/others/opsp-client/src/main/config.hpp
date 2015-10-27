#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__
#include <string>
#include <libsol-util/utl.h>

class config{
    public:
        int log_level;
        std::string log_dest;
        std::string log_path;
        std::string log_name;
        std::string log_file;
        std::string server_url;
        std::string server_path;
        std::string server_full_path;
    public:
        config(): log_level(0) {}
        void config_parse(const std::string &configfile);
    private:
        static UBOOL8 config_handler(const char *group, const char *name, const char *value, void *cxdata);
};

#endif /* __CONFIG_HPP__ */
