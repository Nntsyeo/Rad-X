#include "EEPROM.h"

#define numChars 40

char received_chars[numChars];
boolean cmd_serv = false;
boolean cmd_char = false;

struct ble_UUID
{
    String service = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
    String characteristic = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
};



ble_UUID *UUID, uuid;

// namespace UUID_const {
//     constexpr static bleUUID *UUID, uuid;
// }

void storeUUID(ble_UUID *ID)
{
    // store service UUID
    if (cmd_serv)
    {
        for (uint8_t i = 0; i < numChars; i++)
        {
            EEPROM.write(i, ID->service[i]);
        }
        EEPROM.write(200, true); // set EEPROM flag
    }
    else if (cmd_char)
    {
        for (uint8_t i = numChars; i < numChars * 2; i++)
        {
            EEPROM.write(i, ID->characteristic[i]);
        }
        EEPROM.write(201, true); // set EEPROM flag
    }
}

void readUUID(ble_UUID *ID)
{
    char s = EEPROM.read(200);
    char c = EEPROM.read(201);
    // read service UUID
    if (s)
    {
        for (uint8_t i = 0; i < numChars; i++)
        {
            UUID->service[i] = EEPROM.read(i);
        }
        EEPROM.write(200, false);
    }

    // read characteristic UUID
    if (c)
    {
        for (uint8_t i = numChars; i < numChars * 2; i++)
        {
            UUID->characteristic[i] = EEPROM.read(i);
        }
        EEPROM.write(201, false);
    }
}

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

void checkForCmd(ble_UUID *ID)
{
    boolean status = false;
    status = recvWithEndMarker();
    if (status)
    {
        if (strcmp(received_chars, "cmd_char"))
        {
            cmd_char = true;
            Serial.print("Enter Characteristic UUID: ");
        }
        else if (strcmp(received_chars, "cmd_serv") == 0)
        {
            cmd_serv = true;
            Serial.print("Enter Service UUID: ");
        }
        if (cmd_serv)
        {
            storeUUID(ID);
            ID->service = received_chars;
            Serial.println(received_chars);
            cmd_serv = false;
        }
        else if (cmd_char)
        {
            storeUUID(ID);
            ID->characteristic = received_chars;
            Serial.println(received_chars);
            cmd_char = false;
        }
    }
}
