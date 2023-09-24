void setup()
{
    BT_SERIAL.begin(9600); // Initialize serial communication at 9600 baud rate
    // wait for serial monitor to open
}

void loop()
{
    if (BT_SERIAL.available())
    {                                               // Check if there is data available on serial port
        String data = BT_SERIAL.readStringUntil('\n'); // Read data until newline character
        if (data == "TESTOUTPUT")
        { // If the data is "TESTOUTPUT"
            for (int i = 1; i <= 100; i++)
            {                      // Loop from 1 to 100
                BT_SERIAL.println(i); // Send the current number over the serial port
            }
        }
        else
        {                        // If the data is not "TESTOUTPUT"
            BT_SERIAL.print("{");   // Send opening curly brace
            BT_SERIAL.print(data);  // Send data received from serial monitor
            BT_SERIAL.println("}"); // Send closing curly brace and newline character
        }
    }
}
