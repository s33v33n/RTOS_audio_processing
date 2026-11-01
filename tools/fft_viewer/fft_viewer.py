"""
The board sends one frame every ~64 ms over UART:
    [BB BB BB BB] + 512 x float32 (little-endian) = magnitude spectrum
"""
import matplotlib.pyplot as plt
import numpy as np
import serial

# --- Settings (must match the firmware and USB-UART adapter) ---

PORT = "/dev/ttyUSB0"       # check your port 
BAUD_RATE = 921600          
SAMPLE_RATE = 16000         # in Hz, the same as I2S, etc. fft window up to 8 kHz
FFT_SIZE = 1024             # from AudioApp
SYNC = b"\xBB\xBB\xBB\xBB"

N_BINS = FFT_SIZE // 2
FRAME_LEN = len(SYNC) + N_BINS * 4
FULL_SCALE = 32768.0                # int16 full scale -> 0 dBFS
DB_MIN = -120


def read_newest_frame(ser, buf):
    """Read the bytes that have arrived and return the newest completed data or none """
    buf += ser.read(ser.in_waiting)
    newest = None
    while True:
        start = buf.find(SYNC)
        if start < 0:
            del buf[:-(len(SYNC) - 1)]      # keep a possible partial sync word
            return newest
        if len(buf) - start < FRAME_LEN:
            del buf[:start]                 # frame not complete yet
            return newest
        payload = bytes(buf[start + len(SYNC):start + FRAME_LEN])
        newest = np.frombuffer(payload, dtype="<f4")
        del buf[:start + FRAME_LEN]


def main():
    ser = serial.Serial(PORT, BAUD_RATE, timeout=0)
    freqs = np.arange(N_BINS) * SAMPLE_RATE / FFT_SIZE

    plt.ion()                                   # window does not block the loop
    fig, ax = plt.subplots(figsize=(10, 5))
    line, = ax.plot(freqs, np.full(N_BINS, DB_MIN))
    peak_text = ax.text(0.99, 0.99, "", transform=ax.transAxes, ha="right", va="top")
    ax.set(xlim=(0, SAMPLE_RATE / 2), ylim=(DB_MIN, 0),
           xlabel="Frequency [Hz]", ylabel="Level [dBFS]", title="Microphone spectrum")
    ax.grid(True, alpha=0.3)

    buf = bytearray()

    while plt.fignum_exists(fig.number):        # run while window is open
        mag = read_newest_frame(ser, buf)
        if mag is not None:
            db = 20 * np.log10(np.maximum(mag / FULL_SCALE, 10 ** (DB_MIN / 20)))
            line.set_ydata(db)
            k = int(np.argmax(db[1:])) + 1  # skip DC bin
            peak_text.set_text(f"peak: {freqs[k]:.0f} Hz, {db[k]:.1f} dBFS")
        plt.pause(0.05)                         # redraw the window, wait 50 ms

    ser.close()


if __name__ == "__main__":
    main()
