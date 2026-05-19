import serial
import pandas as pd
import time
import matplotlib.pyplot as plt

# ============================================================================
# CHANGE THIS TO YOUR SERIAL PORT
# On Mac: /dev/ttyUSB0 or /dev/ttyACM0
# On Windows: COM3, COM4, etc.
# ============================================================================
SERIAL_PORT = 'COM10'
BAUD_RATE = 115200
TIMEOUT = 10

def receive_motor_data():
    print(f"Connecting to {SERIAL_PORT}...")
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=TIMEOUT)
    time.sleep(2)

    # Start logging
    print("Sending 'S' to start logging...")
    ser.write(b'S\n')

    # Wait for logging to finish (30 seconds + buffer)
    print("Waiting 32 seconds for logging to complete...")
    time.sleep(32)

    # Dump data
    print("Sending 'D' to dump data...")
    ser.write(b'D\n')

    data = []

    try:
        while True:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if line:
                print(f"Received: {line}")

                if 'EOF' in line:
                    break

                if 'sample_num' in line or 'Dump complete' in line:
                    continue

                if ',' in line:
                    data.append(line)
    except KeyboardInterrupt:
        print("Data collection stopped.")
    finally:
        ser.close()

    return data

def save_data_to_csv(data, filename='motor_data.csv'):
    if not data:
        print("No data to save!")
        return
    with open(filename, 'w') as f:
        f.write('sample_num,left_count,right_count\n')
        for line in data:
            f.write(line + '\n')
    print(f"Data saved to {filename}")

def plot_motor_data(filename='motor_data.csv'):
    df = pd.read_csv(filename)

    if df.empty:
        print("No data to plot!")
        return

    # Your encoder measured at ~102 PPR (4079 counts / 10 rotations / 4 quadrature)
    ENCODER_PPR = 102

    df['left_rpm'] = df['left_count'].diff() * 100 * 60 / (ENCODER_PPR * 4)
    df['right_rpm'] = df['right_count'].diff() * 100 * 60 / (ENCODER_PPR * 4)

    fig, axes = plt.subplots(2, 1, figsize=(10, 8))

    axes[0].plot(df['sample_num'], df['left_count'], label='Left Motor', linewidth=2)
    axes[0].plot(df['sample_num'], df['right_count'], label='Right Motor', linewidth=2)
    axes[0].set_xlabel('Sample Number')
    axes[0].set_ylabel('Encoder Count')
    axes[0].set_title('Raw Encoder Counts')
    axes[0].legend()
    axes[0].grid(True)

    axes[1].plot(df['sample_num'], df['left_rpm'], label='Left Motor', linewidth=2)
    axes[1].plot(df['sample_num'], df['right_rpm'], label='Right Motor', linewidth=2)
    axes[1].set_xlabel('Sample Number')
    axes[1].set_ylabel('RPM')
    axes[1].set_title('Motor Speed (RPM)')
    axes[1].legend()
    axes[1].grid(True)

    plt.tight_layout()
    plt.savefig('motor_analysis.png')
    print("Plot saved to motor_analysis.png")
    plt.show()

if __name__ == "__main__":
    motor_data = receive_motor_data()
    save_data_to_csv(motor_data)
    plot_motor_data()
