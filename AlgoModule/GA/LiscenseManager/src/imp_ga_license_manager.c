#include <stdio.h>
#include <string.h>
#ifdef TD_LICENSE_CHECK
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include "imp_algo_interface.h"
#endif

#ifdef TD_LICENSE_CHECK

IMP_S32 IMP_GetMac(IMP_S8 *ps8EthName, IMP_U32 *pu32Mac)
{
    IMP_S32 i;

    /* get pu32Mac */
    struct ifreq ifreq;
    IMP_S32 sock;

    if((sock = socket(AF_INET, SOCK_STREAM,0)) < 0)
    {
        perror("error");
        return -1;
    }
    strcpy(ifreq.ifr_name, ps8EthName);
    if( ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0 )
    {
        perror("error:");
        return -2;
    }

    for (i = 0; i < 6; i++)
    {
        *(pu32Mac+i) = (unsigned char)ifreq.ifr_hwaddr.sa_data[i];
#if defined(IMP_DEBUG_PRINT)
		printf("MAC: %02x \n", *(pu32Mac + i));
#endif
    }

    return 0;
}
// success return 1 or return 0
// if failed to read pu32Mac address, return -1
// kaer -2011-05-17 start mac static IMP_S32 LicenseAuthorizedStart[6]={0x00,0x90,0xb0,0x1a,0x62,0xe0};
// kaer -2011-05-17 end mac static   IMP_S32 LicenseAuthorizedEnd[6]={0x00,0x90,0xb0,0x1a,0x63,0x12};
static IMP_S32 LicenseAuthorizedStart[6]={0x00,0x50,0xc2,0x2a,0x5d,0x2b};
static IMP_S32 LicenseAuthorizedEnd[6]={0x00,0x50,0xc2,0x2a,0x5d,0x2b};
static IMP_U32 u32LicenseNum = 6;

// IMP_S32 IMP_CheckLicenseByMac()
// {
//     IMP_U32 au32Mac[6];
//     IMP_S32 i;
//     if(IMP_GetMac("eth0", &au32Mac[0]) < 0)
//     {
//         printf("License check: failed to read Mac address!\n");
//         return IMP_STATUS_READ_MAC_FAILED;
//     }
//
//     for (i = 0; i < u32LicenseNum; i++)
// 	{
// 		if (au32Mac[i] >= LicenseAuthorizedStart[i] && au32Mac[i] <= LicenseAuthorizedEnd[i])
// 		{
// 			continue;
// 		}
// 		else
// 		{
// 			printf("License check failed!\n");
// 			return IMP_STATUS_CHECK_LICENSE_FAILED;
// 		}
// 	}
//     return IMP_STATUS_OK;
// }

IMP_S32 IMP_CheckLicenseByMac()
{
    IMP_U32 au32Mac[6];
    IMP_S32 i;
    if(IMP_GetMac("eth0", &au32Mac[0]) < 0)
    {
        printf("License check: failed to read Mac address!\n");
        return IMP_STATUS_READ_MAC_FAILED;
    }

    for (i = 0; i < 6; i++)
    {
        if(i==5)
        {
            if(au32Mac[i] >= LicenseAuthorizedStart[i] && au32Mac[i] <= LicenseAuthorizedEnd[i])
            {
                return IMP_STATUS_OK;
            }
            else
            {
                printf("License check failed!\n");
                return IMP_STATUS_CHECK_LICENSE_FAILED;
            }
        }
        if (  au32Mac[i] == LicenseAuthorizedStart[i] || au32Mac[i] == LicenseAuthorizedEnd[i])
        {
            if (  au32Mac[i] == LicenseAuthorizedStart[i] && au32Mac[i] == LicenseAuthorizedEnd[i])
            {
				continue;
            }
			else if(au32Mac[i] == LicenseAuthorizedStart[i])
			{
                if(au32Mac[i+1] >= LicenseAuthorizedStart[i+1])
                {
					return IMP_STATUS_OK;
                }
                else
                {
                    printf("License check failed!\n");
                    return IMP_STATUS_CHECK_LICENSE_FAILED;
                }
			}
			else
			{
				if(au32Mac[i+1] <= LicenseAuthorizedEnd[i+1])
                {
					return IMP_STATUS_OK;
                }
                else
                {
                    printf("License check failed!\n");
                    return IMP_STATUS_CHECK_LICENSE_FAILED;
                }
			}
        }
        else if(au32Mac[i] > LicenseAuthorizedStart[i] && au32Mac[i] < LicenseAuthorizedEnd[i])
        {
            return IMP_STATUS_OK;
        }
        else
        {
            printf("License check failed!\n");
            return IMP_STATUS_CHECK_LICENSE_FAILED;
        }
    }
	return IMP_STATUS_OK;
}

#endif
