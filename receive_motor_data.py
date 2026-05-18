import serial
import pandas as pd
import time
import matplotlib.pyplot as plt


SERIAL_PORT = 'COM3'  # Update with your serial port
BAUD_RATE = 115200
TIMEOUT = 10

def receive_motor_data():
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=TIMEOUT)
    time.sleep(2)
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
    with open(filename, 'w') as f:
        f.write('sample_num,left_count,right_count\n')
        for line in data:
            f.write(line + '\n')
    print(f"Data saved to {filename}")

def plot_motor_data(filename='motor_data.csv', encoder_ppr=20):
    df = pd.read_csv(filename)
    
    df['left_rpm'] = df['left_count'].diff() * 100 * 60 / (encoder_ppr * 4)
    df['right_rpm'] = df['right_count'].diff() * 100 * 60 / (encoder_ppr * 4)
    
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