#ifndef __FIRMWARE_H__
#define __FIRMWARE_H__
#include <string>

class CFirmware {
    private:
        int nTimeout;
        std::string sOpkgConfFile;
        std::string sFirmwarePath;
        std::string sFirmwareName;

    private:
        void GetFirmwareName(std::string &firmware_url);
        void CleanUp(void);
        bool DownLoadFirmware(std::string &firmware_url);
        bool GetFirmwareUrl(const std::string &mac, const std::string &md5, const std::string &server_url, std::string &source_url, std::string &firmware_url);
        void DoTruelyFirmwareUpgrade(void);
        bool IsAlreadyInConf(const std::string &source_url);
        void UpdateOpkgConf(const std::string &source_url);

    public:
        CFirmware(): nTimeout(5), sOpkgConfFile("/etc/opkg.conf"), sFirmwarePath("/var/"){}
        void UpgradeFirmware(const std::string &mac, const std::string &md5, const std::string &server_url);
};

#endif /* __FIRMWARE_H__ */
