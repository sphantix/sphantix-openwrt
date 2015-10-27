1. opsp-client based on dynamic library libsol-util
2. jsonParser directory contains cjson files which are used to parse or orgnize json messages
3. webproto directory contains connection functions(socket, http ...)
4. Server interface is separated into 3 partitions as below:

        ***************
        *Json|Soap|XML*
        ***************
        *    HTTP     *
        ***************
        *   Socket    *
        ***************
