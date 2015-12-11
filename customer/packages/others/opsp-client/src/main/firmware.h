#ifndef __FIRMWARE_H__
#define __FIRMWARE_H__
#include <string>

class CUpgradeInfo {
    public:
        int nSaveConfig; 
        std::string sSourceUrl;
        std::string sFirmwareUrl;
        std::string sFirmwareMd5;
        std::string sUbootUrl;
        std::string sUbootMd5;
};

class CFirmware {
    private:
        typedef enum {
            eUboot,
            eFirmware,
        } eFirmwareType;

        int nTimeout;
        std::string sOpkgConfFile;
        std::string sFirmwarePath;
        std::string sFirmwareName;
        std::string sUbootName;

        void GetFileName(eFirmwareType type, std::string &url);
        void CleanUp(eFirmwareType type);
        bool MD5Check(eFirmwareType type, std::string &md5sum);
        bool DownLoadFile(eFirmwareType type, std::string &url, std::string &md5sum);
        bool GetUpgradeInfo(const std::string &mac, const std::string &md5, const std::string &server_url, CUpgradeInfo &upgrade_info);
        void DoTruelyUpgrade(eFirmwareType type, int save_config);
        bool IsAlreadyInConf(const std::string &source_url);
        void UpdateOpkgConf(const std::string &source_url);

    public:
        CFirmware(): nTimeout(5), sOpkgConfFile("/etc/opkg.conf"), sFirmwarePath("/tmp/"){}
        void UpgradeFirmware(const std::string &mac, const std::string &md5, const std::string &server_url);
};

#endif /* __FIRMWARE_H__ */
