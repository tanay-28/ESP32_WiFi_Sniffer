import serial
import time
import os
import re

# Configuration Variables
SERIAL_PORT = '/dev/cu.usbserial-11320'
BAUD_RATE = 115200
OUTPUT_CSV = 'esp32_sniffer_data.csv'

# Sample Size
MAX_SAMPLES = 50000  

print('🚀 Initializing connection to the ESP32...')

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2)
    file_exists = os.path.isfile(OUTPUT_CSV)

    # Initialize our software packet counter
    packet_count = 0

    with open(OUTPUT_CSV, 'a', encoding='utf-8') as csv_file:
        if not file_exists:
            csv_file.write("Timestamp_MS,Channel,Frame_Size_Bytes,RSSI_dBm,MAC_Address\n")
            csv_file.flush()
            print(f"📝 Created new file with structured schema headers.")

        print(f"💾 Streaming data directly to '{OUTPUT_CSV}'.")
        print(f"📊 Target cap set to: {MAX_SAMPLES} samples. Capturing...")

        # Loop runs until we hit our exact target count
        while packet_count < MAX_SAMPLES:
            if ser.in_waiting:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                
                if line:
                    # Validate that the line is actual raw packet data matrix rows
                    if re.match(r'^\d+,\d+,\d+,-?\d+,[0-9A-Fa-f:]+$', line):
                        packet_count += 1
                        
                        # Print live count updates so you can watch the progress
                        print(f"⚡ [{packet_count}/{MAX_SAMPLES}] Captured: {line}")
                        
                        csv_file.write(line + "\n")
                        csv_file.flush()
                    else:
                        # Print system boot logs without counting them as samples
                        print(f"⚙️ System Log: {line}")

        print(f"\n🎯 Target reached! Successfully logged exactly {MAX_SAMPLES} packets.")

except KeyboardInterrupt:
    print("\n🛑 Logging terminated early by user. Data saved up to this point.")
except Exception as e:
    print(f"\n❌ Operational Error: {e}")