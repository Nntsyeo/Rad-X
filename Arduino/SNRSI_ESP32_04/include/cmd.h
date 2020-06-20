#include <methods.h>
#include "EEPROM.h"

#define numChars 40

char received_chars[numChars];
boolean cmd_serv = false;
boolean cmd_char = false;

struct bleUUID
{
    String service = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
    String characteristic = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
};

bleUUID *UUID, uuid;

boolean recvWithEndMarker()
{
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;

    while (Serial.available() > 0)
    {
        rc = Serial.read();

        if (rc != endMarker)
        {
            received_chars[ndx] = rc;
            ndx++;
            if (ndx >= numChars)
            {
                ndx = numChars - 1;
            }
        }
        else
        {
            received_chars[ndx] = '\0'; // terminate the string
            ndx = 0;
            // Serial.println(received_chars);
            return true;
        }
    }
    return false;
}
