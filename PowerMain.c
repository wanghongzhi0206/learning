#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "hidapi.h"

hid_device *handle;

typedef struct 
{
    unsigned short uhYear;
    unsigned char  ubMonth;
    unsigned char  ubDay;
    unsigned char  ubHour;
    unsigned char  ubMinute;
    unsigned char  ubSecond;
    unsigned short uhMsec;
} ClockAppTime_ts;

void CommandGetStartup(unsigned char* cmd) {
    unsigned char* work_cmd = cmd;

    time_t T= time(NULL);
    struct  tm tm = *localtime(&T);
    ClockAppTime_ts clockTime = { tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
                                  tm.tm_hour, tm.tm_min, tm.tm_sec, 0 };

    if (NULL != work_cmd) {
        work_cmd[0] = 'S';
        memcpy(&work_cmd[1], &clockTime, sizeof(clockTime))
    }

     
    printf("System Date is: %02d/%02d/%04d\n",tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900);
    printf("System Time is: %02d:%02d:%02d\n",tm.tm_hour, tm.tm_min, tm.tm_sec);


 }

static void *CurrentReadThread(void *argument)
{
    int res;
    //hid_device *handle;
    unsigned char buf[64];

    // res = hid_init();
    // if( res == -1 )
    // {
    //     return (void*)1;
    // }

    // handle = hid_open((0x0483, 0x5710, NULL);
    // if( handle == NULL )
    // {
    //     return (void*)2;
    // }

    printf( "while 2\n");

    while( 1 )
    {
        memset( buf, 0, 64 );
        res = hid_read(handle, buf, 64);
        if( res == -1 )
        {
            return (void*)3;
        }

        printf( "received %d bytes\n", res);

        for (int i = 0; i < res; i++)
            printf("Byte %d: %02x ", i+1, buf[i]);
        //printf( "%02x ", buf[0]);
        fflush(stdout);
    }

    return (void*)0;
}


int main(int argc, char* argv[])
{
    int res;
    //hid_device *handle;
    unsigned char buf[64];

    res = hid_init();
    if( res == -1 )
    {
        return 1;
    }

    handle = hid_open(0x0483, 0x5710, NULL);
    if( handle == NULL )
    {
        return 2;
    }

    hid_set_nonblocking( handle, 0 );

    pthread_t thread;
    int rc = pthread_create(&thread, NULL, CurrentReadThread, NULL);

    printf( "while 1\n");

    while(1)
    {
        int a = getchar();
        if( a == 'a')
        {
            // Get Start Command
            CommandGetStartup(buf);
            res = hid_write(handle, buf, 64);
            if( res != -1 )
                printf( "write ok, transferred %d bytes\n", res );
            else
            {
                printf( "write error\n" );
                char* str = hid_error(handle);
                printf( "error: %s\n", str );
                return 1;
            }
        }
        else if( a== 'b')
            break;
    }

    void* trc;
    rc = pthread_join(thread, &trc);

    printf( "rc code: %d\n", (int)trc );

    // Finalize the hidapi library
    res = hid_exit();

    return 0;
}