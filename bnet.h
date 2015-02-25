#ifndef BNET_H
#define BNET_H
#include <winsock2.h>
#include <ws2tcpip.h>
#include <bassert.h>
#include <buffer.h>

#define VERSION_HIGH    2
#define VERSION_LOW     2
#define TIMEOUT_SECS    2
#define TIMEOUT_USECS   0

class TNet
{
   WSADATA wsaData;
public:
   ~TNet()
   {
      WSACleanup();
   }
   bool init()
   {
      bool bReturn = false;
      if(!WSAStartup(MAKEWORD(VERSION_LOW, VERSION_HIGH), &wsaData))
      {
         if(LOBYTE(wsaData.wVersion) == VERSION_LOW && HIBYTE(wsaData.wVersion) == VERSION_HIGH)
         {
            bReturn = true;
         }
      }
      return bReturn;
   }
};

class TAddressInfo
{
   ADDRINFO* AddrInfo;
public:
   TAddressInfo()
   {
      AddrInfo = 0;
   }
   ~TAddressInfo()
   {
      if(AddrInfo)
      {
         freeaddrinfo(AddrInfo);
      }
   }
};

class TSocket
{
   SOCKET sock;
   ADDRINFO Hints, *AddrInfo;
public:
   TSocket()
   {
      sock = INVALID_SOCKET;
      ZeroMemory(&Hints, sizeof(Hints));
      AddrInfo = 0;
   }
   ~TSocket()
   {
      close();
   }
   bool connect(char* szHost, char* szPort, int socketType)
   {
      bool bReturn = false;
      ZeroMemory(&Hints, sizeof(Hints));
      Hints.ai_family = AF_UNSPEC;
      Hints.ai_socktype = socketType;
      if(!getaddrinfo(szHost, szPort, &Hints, &AddrInfo))
      {
         // Attempt to connect to each address until we find one that succeeds
         for(ADDRINFO* AI = AddrInfo; AI != NULL; AI = AI->ai_next)
         {
            if((AI->ai_family == PF_INET) || (AI->ai_family == PF_INET6)) // only want PF_INET or PF_INET6
            {
               sock = socket(AI->ai_family, AI->ai_socktype, AI->ai_protocol);
               if(sock != INVALID_SOCKET)
               {
                  if (AI->ai_socktype == socketType)
                  {
                     if(::connect(sock, AI->ai_addr, AI->ai_addrlen) == SOCKET_ERROR)
                     {
                        // Connect failed, let's close this socket and try again on the next address in the list
                        closesocket(sock);
                        continue;
                     }
                     bReturn = true;
                     break;
                  }
               }
            }
         }
      }
      return bReturn;
   }
   void close()
   {
      if(sock != INVALID_SOCKET)
      {
        shutdown(sock, SD_BOTH);
        closesocket(sock);
        sock = INVALID_SOCKET;
      }
      if(AddrInfo)
      {
         freeaddrinfo(AddrInfo);
         AddrInfo = 0;
      }
   }
   bool write(char *buf, int bufLen, int *numActualBytes = 0)
   {
      bool bReturn = false;
      if(numActualBytes)
         *numActualBytes = 0;
      if(sock != INVALID_SOCKET)
      {
         int len = send(sock, buf, bufLen, 0);
         if(len != SOCKET_ERROR)
         {
            if(numActualBytes)
               *numActualBytes = len;
            bReturn = true;
         }
      }
      return bReturn;
   }
   bool read(char *buf, int bufLen, int *numActualBytes = 0)
   {
      bool bReturn = false;
      if(numActualBytes)
         *numActualBytes = 0;
      if(sock != INVALID_SOCKET)
      {
         fd_set fdReadSet;
         FD_ZERO(&fdReadSet);
         FD_SET(sock, &fdReadSet);
         TIMEVAL timeout = {TIMEOUT_SECS, TIMEOUT_USECS};
         int iSelect = select(0, &fdReadSet, NULL, NULL, &timeout);
         if(iSelect != SOCKET_ERROR)
         {
            if(iSelect > 0)
            {
               int len = recv(sock, buf, bufLen, 0);
               if(len != SOCKET_ERROR)
               {
                  if(numActualBytes)
                     *numActualBytes = len;
                  bReturn = true;
               }
            }
            else
               bReturn = true;
         }
      }
      return bReturn;
   }
};
#endif //BNET_H
