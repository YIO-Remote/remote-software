#ifndef SERVICE_IR_H
#define SERVICE_IR_H

#include <Arduino.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRremoteESP8266.h> // https://platformio.org/lib/show/1089/IRremoteESP8266
#include <IRac.h>
#include <IRutils.h>
#include <IRtimer.h>

const uint16_t kRecvPin = 22;
const uint16_t kIrLedPin = 19;
const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024; // 1024 == ~511 bits
const uint8_t kTimeout = 50;              // Milli-Seconds
const uint16_t kFrequency = 38000;        // in Hz. e.g. 38kHz.
const uint16_t kMinUnknownSize = 12;

IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
IRsend irsend(kIrLedPin);

class InfraredService
{
public:
    void init()
    {
        irrecv.enableIRIn();
        irsend.begin();
    }

    void doRestart(const char *str, const bool serial_only)
    {
        delay(2000); // Enough time for messages to be sent.
        ESP.restart();
        delay(5000); // Enough time to ensure we don't return.
    }

    void receive()
    {
        if (irrecv.decode(&results))
        {
            decode_type_t protocol = results.decode_type;
            uint16_t size = results.bits;
            bool success = true;

            if (protocol == decode_type_t::RAW)
            {
                Serial.println("RAW");
            }
            else if (protocol == decode_type_t::PRONTO)
            {
                Serial.println("PRONTO");
            }
            else {
                Serial.println("UNKNOWN");
                uint16_t *raw_array = resultToRawArray(&results);
                size = getCorrectedRawLength(&results);
                delete[] raw_array;
            }

            Serial.print(resultToHumanReadableBasic(&results));
            String description = IRAcUtils::resultAcToString(&results);
            if (description.length())
                Serial.println("Mesg Desc.: " + description);
            yield(); // Feed the WDT (again)
            // Output the results as source code
            Serial.println(resultToSourceCode(&results));
            Serial.println(); // Blank line between entries
            yield();          // Feed the WDT (again)
            irrecv.resume();
        }
    }

    // Count how many values are in the String.
    // Args:
    //   str:  String containing the values.
    //   sep:  Character that separates the values.
    // Returns:
    //   The number of values found in the String.
    uint16_t countValuesInStr(const String str, char sep)
    {
        int16_t index = -1;
        uint16_t count = 1;
        do
        {
            index = str.indexOf(sep, index + 1);
            count++;
        } while (index != -1);
        return count;
    }

    // Dynamically allocate an array of uint16_t's.
    // Args:
    //   size:  Nr. of uint16_t's need to be in the new array.
    // Returns:
    //   A Ptr to the new array. Restarts the ESP if it fails.
    uint16_t *newCodeArray(const uint16_t size)
    {
        uint16_t *result;

        result = reinterpret_cast<uint16_t *>(malloc(size * sizeof(uint16_t)));
        // Check we malloc'ed successfully.
        if (result == NULL) // malloc failed, so give up.
            doRestart(
                "FATAL: Can't allocate memory for an array for a new message! "
                "Forcing a reboot!",
                true); // Send to serial only as we are in low mem
        return result;
    }

    // Parse a Pronto Hex String/code and send it.
    // Args:
    //   irsend: A ptr to the IRsend object to transmit via.
    //   str: A comma-separated String of nr. of repeats, then hexadecimal numbers.
    //        e.g. "R1,0000,0067,0000,0015,0060,0018,0018,0018,0030,0018,0030,0018,
    //              0030,0018,0018,0018,0030,0018,0018,0018,0018,0018,0030,0018,
    //              0018,0018,0030,0018,0030,0018,0030,0018,0018,0018,0018,0018,
    //              0030,0018,0018,0018,0018,0018,0030,0018,0018,03f6"
    //              or
    //              "0000,0067,0000,0015,0060,0018". i.e. without the Repeat value
    //        Requires at least kProntoMinLength comma-separated values.
    //        sendPronto() only supports raw pronto code types, thus so does this.
    //   repeats:  Nr. of times the message is to be repeated.
    //             This value is ignored if an embeddd repeat is found in str.
    // Returns:
    //   bool: Successfully sent or not.
    bool send(const String str, uint16_t repeats)
    {
        uint16_t count;
        uint16_t *code_array;
        int16_t index = -1;
        uint16_t start_from = 0;

        // Find out how many items there are in the string.
        count = countValuesInStr(str, ',');

        // Check if we have the optional embedded repeats value in the code string.
        if (str.startsWith("R") || str.startsWith("r"))
        {
            // Grab the first value from the string, as it is the nr. of repeats.
            index = str.indexOf(',', start_from);
            repeats = str.substring(start_from + 1, index).toInt(); // Skip the 'R'.
            start_from = index + 1;
            count--; // We don't count the repeats value as part of the code array.
        }

        // We need at least kProntoMinLength values for the code part.
        if (count < kProntoMinLength)
            return false;

        // Now we know how many there are, allocate the memory to store them all.
        code_array = newCodeArray(count);

        // Rest of the string are values for the code array.
        // Now convert the hex strings to integers and place them in code_array.
        count = 0;
        do
        {
            index = str.indexOf(',', start_from);
            // Convert the hexadecimal value string to an unsigned integer.
            code_array[count] = strtoul(str.substring(start_from, index).c_str(),
                                        NULL, 16);
            start_from = index + 1;
            count++;
        } while (index != -1);

        irsend.sendPronto(code_array, count, repeats); // All done. Send it.
        free(code_array);                              // Free up the memory allocated.
        if (count > 0)
            return true; // We sent something.
        return false;    // We probably didn't.
    }

    // Parse an IRremote Raw Hex String/code and send it.
    // Args:
    //   irsend: A ptr to the IRsend object to transmit via.
    //   str: A comma-separated String containing the freq and raw IR data.
    //        e.g. "38000,9000,4500,600,1450,600,900,650,1500,..."
    //        Requires at least two comma-separated values.
    //        First value is the transmission frequency in Hz or kHz.
    // Returns:
    //   bool: Successfully sent or not.
    bool send(const String str)
    {
        uint16_t count;
        uint16_t freq = 38000; // Default to 38kHz.
        uint16_t *raw_array;

        // Find out how many items there are in the string.
        count = countValuesInStr(str, ',');

        // We expect the frequency as the first comma separated value, so we need at
        // least two values. If not, bail out.
        if (count < 2)
            return false;
        count--; // We don't count the frequency value as part of the raw array.

        // Now we know how many there are, allocate the memory to store them all.
        raw_array = newCodeArray(count);

        // Grab the first value from the string, as it is the frequency.
        int16_t index = str.indexOf(',', 0);
        freq = str.substring(0, index).toInt();
        uint16_t start_from = index + 1;
        // Rest of the string are values for the raw array.
        // Now convert the strings to integers and place them in raw_array.
        count = 0;
        do
        {
            index = str.indexOf(',', start_from);
            raw_array[count] = str.substring(start_from, index).toInt();
            start_from = index + 1;
            count++;
        } while (index != -1);

        irsend.sendRaw(raw_array, count, freq); // All done. Send it.
        free(raw_array);                        // Free up the memory allocated.
        if (count > 0)
            return true; // We sent something.
        return false;    // We probably didn't.
    }

public:
    decode_results results;
    bool receiving = true;
};

#endif