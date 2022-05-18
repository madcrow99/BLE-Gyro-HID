# BLE-Gyro-HID
This project is to create a low-cost human interface device (HID) that enables people with motor impairments control phones and computers. Basically, a mouse that requires very limited head movements to operate.

The device uses a gyroscope mounted to an earpiece to track head movements (very little movement required) and move a mouse pointer accordingly. The device has 4 optional switch / button inputs (3.5mm headphone jack) for use with any switches that the user can control.

left: works the same as the left click button on a regular mouse

right: works the same as the right click button on a regular mouse 

scroll: tap to scroll down and long press to scroll up

dwell: tap to to toggle dwell clicking on and off. Dwell clicking is where the device will left click/tap when the pointer is stationary for a specified time. Being able to turn this feature on and off really helps the user from going crazy when the mouse constantly clicks.


![image](https://user-images.githubusercontent.com/60524115/169169440-575a1dae-fa35-465c-b26f-152073b4841a.png)

![image](https://user-images.githubusercontent.com/60524115/169169907-82e66a26-245c-4c65-b1aa-6736aed9e78a.png)

The device uses an Adafruit Feather Express nRF52840 and an IMU module to act as a Bluetooth low energy mouse. Please see the bill of materials for details. The case and gyro cover are 3d printed. The STL and STEP files ae included.

An instruction manual for building the device is included.

The software has to be uploaded to the nRF52840. Platformio is recommended. Here is a link to the setup instructions:
https://platformio.org/install/ide?install=vscode

