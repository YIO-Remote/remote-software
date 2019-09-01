#ifndef SERVICE_IR_H
#define SERVICE_IR_H

#include <IRremote.h>
#include <ProntoHex.h>

#define IR_RECV 22
#define IR_SEND 19

IRrecv irrecv(IR_RECV);
IRsend irsend(IR_SEND);

ProntoHex ph = ProntoHex();

class InfraredService
{
public:
    void init()
    {   
        irrecv.enableIRIn();
    }

    bool handlePronto(String line)
    {
        line.trim();
        if (line.startsWith("0000 ")) // Then we assume it is Pronto Hex
        {
            Serial.println("Convert PRONTO");
            ph.convert(line);
            Serial.print("Received PRONTO: ");
            Serial.println(line);
            Serial.print("Len:");
            Serial.println(ph.length);
            irsend.sendRaw(ph.convertedRaw, ph.length, ph.frequency);
            return true;
        }
        else
        {
            return false;
        }
    }

    void recordIrStart()
    {
        recordIrLoop = true;
        serviceIrCapturedCodesI = 0;
        serviceIrCapturedCode = 0;
    }

    void recordIrStop()
    {
        recordIrLoop = false;
        serviceIrCapturedCodesI = 0;
        serviceIrCapturedCode = 0;
    }

    void recordIr()
    {

        decode_results results; // Somewhere to store the results

        if (irrecv.decode(&results))
        { // Grab an IR code
            unsigned int result = irCodeCache(&results);
            if (result > 100)
            { // Ir code need some length otherwise it's probably interferance garbage.
                returnRecordIrResponse(GrabIrCode(&results));
            }
            irrecv.resume(); // Prepare for the next value
        }
    }

    unsigned int irCodeCache(decode_results *results)
    {
        if (serviceIrCapturedCodesI < 10)
        {
            serviceIrCapturedCodes[serviceIrCapturedCodesI] = results->value;
            serviceIrCapturedCodesI = serviceIrCapturedCodesI + 1;
            if (matchRecordedIr(results->value) >= 4)
            {
                return results->value;
            }
        }
        else
        {
            returnRecordIrFailed();
            recordIrStop();
            return 0;
        }
    }

    // Matching IR code in earlier recordings.
    // this returns the ammount of duplicates to filter out recorded ir noise.
    int matchRecordedIr(unsigned int IrCode)
    {
        if (serviceIrCapturedCodesI > 0)
        {
            int foundIrCodes = 0;
            for (int i = 0; i < serviceIrCapturedCodesI; i++)
            {
                if (serviceIrCapturedCodes[i] == IrCode)
                {
                    foundIrCodes++;
                }
            }
            return foundIrCodes;
        }
        else
        {
            return 0;
        }
    }

    String GrabIrCode(decode_results *results)
    {
        // Stores the code for later playback
        // Most of this code is just logging
        char buf[10];
        String cType = "";
        String IRcommand = "";
        DynamicJsonDocument root(1024);
        //JsonObject& root = jsonBuffer.createObject();

        codeType = results->decode_type;
        //int count = results->rawlen;
        if (codeType == UNKNOWN)
        {
            String RawString = generateRawIrCode(results);
            Serial.println(RawString);
            root["type"] = "RAW";
            root["value"] = RawString;
            root["length"] = results->bits;
        }
        else
        {
            if (codeType == NEC)
            {
                Serial.print("Received NEC: ");
                if (results->value == REPEAT)
                {
                    // Don't record a NEC repeat value as that's useless.
                    Serial.println("repeat; ignoring.");
                    //return;
                }
                else
                    cType = "NEC";
            }
            else if (codeType == SONY)
            {
                Serial.print("Received SONY: ");
                cType = "SON";
            }
            else if (codeType == PANASONIC)
            {
                Serial.print("Received PANASONIC: ");
                cType = "PAN";
            }
            else if (codeType == JVC)
            {
                Serial.print("Received JVC: ");
                cType = "JVC";
            }
            else if (codeType == RC5)
            {
                Serial.print("Received RC5: ");
                cType = "RC5";
            }
            else if (codeType == RC6)
            {
                Serial.print("Received RC6: ");
                cType = "RC6";
            }
            else
            {
                Serial.print("Unexpected codeType ");
                Serial.print(codeType, DEC);
                Serial.println("");
                cType = "UNKNOWN";
            }
            codeValue = results->value;
            codeLen = results->bits;
            Serial.println(results->value, HEX);
            Serial.println(results->bits);
            root["type"] = codeType;
            root["value"] = results->value;
            root["length"] = results->bits;
        }
        Serial.println("SERIALIZATION OF JSON...");
        serializeJson(root, IRcommand);
        serializeJson(root, Serial);
        Serial.println(IRcommand);
        return IRcommand;
    }

    String generateRawIrCode(decode_results *results)
    {
        codeLen = results->rawlen - 1;
        String RawString = "";
        for (int i = 1; i <= codeLen; i++)
        {
            if (i % 2)
            {
                // Mark
                rawCodes[i - 1] = results->rawbuf[i] * USECPERTICK - MARK_EXCESS;
                RawString += " m";
            }
            else
            {
                // Space
                rawCodes[i - 1] = results->rawbuf[i] * USECPERTICK + MARK_EXCESS;
                RawString += " s";
            }
            //Serial.print(rawCodes[i - 1], DEC);
            RawString += rawCodes[i - 1];
        }
        return RawString;
    }

    void returnRecordIrResponse(String responseJson)
    {
        Serial.print("Response JSON: ");
        Serial.println(responseJson);
        // serverHttp.send(200, "application/json", responseJson);
    }

    void returnRecordIrFailed()
    {
        Serial.print("Recorded IRcode: Failed!");
        // serverHttp.send(400, "application/json", "{\"Error\":\"Failed to record IR code\"}");
    }

public:
    bool recordIrLoop = false;                //enable to recordIr
    unsigned long serviceIrCapturedCodes[10]; //cache of ir codes to filter out noise
    unsigned long serviceIrCapturedCode = 0;  //Found Ir code.
    int serviceIrCapturedCodesI = 0;          //Counter to count form 0 to max cache size.

    // Storage for the recorded code
    int codeType = -1;             // The type of code
    unsigned long codeValue;       // The code value if not raw
    unsigned int rawCodes[RAWBUF]; // The durations if raw
    int codeLen;                   // The length of the code
    int toggle = 0;                // The RC5/6 toggle state
};

#endif