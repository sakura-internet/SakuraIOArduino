// MPU-6050 Accelerometer + Gyro
// This code based on arduino playground and URL below (http://cranberrytree.blogspot.jp/2014/06/gy-521mpu-6050.html)
/**
 * Channel definitions in Sakura IoT Platform
 *
 * Channel 0: Temperature in celsius
 * Channel 1: Accelerometer X (G)
 * Channel 2: Accelerometer Y
 * Channel 3: Accelerometer Z
 * Channel 4: Gyro X (degrees / sec)
 * Channel 5: Gyro Y
 * Channel 6: Gyro Z
 */
#include <Arduino.h>
#include <Wire.h>
#include <SakuraIO.h>

SakuraIO_I2C sakuraio;

#define MPU6050_ACCEL_XOUT_H 0x3B   // R
#define MPU6050_WHO_AM_I     0x75   // R
#define MPU6050_PWR_MGMT_1   0x6B   // R/W
#define MPU6050_I2C_ADDRESS  0x68

#define CH_TEMP    0
#define CH_ACC_X   1
#define CH_ACC_Y   2
#define CH_ACC_Z   3
#define CH_GYRO_X  4
#define CH_GYRO_Y  5
#define CH_GYRO_Z  6
#define SEND_INTERVAL 500 // ms

typedef union {
    struct {
        uint8_t x_accel_h;
        uint8_t x_accel_l;
        uint8_t y_accel_h;
        uint8_t y_accel_l;
        uint8_t z_accel_h;
        uint8_t z_accel_l;
        uint8_t t_h;
        uint8_t t_l;
        uint8_t x_gyro_h;
        uint8_t x_gyro_l;
        uint8_t y_gyro_h;
        uint8_t y_gyro_l;
        uint8_t z_gyro_h;
        uint8_t z_gyro_l;
    }
            reg;
    struct {
        int16_t x_accel;
        int16_t y_accel;
        int16_t z_accel;
        int16_t temperature;
        int16_t x_gyro;
        int16_t y_gyro;
        int16_t z_gyro;
    }
            value;
} accel_t_gyro_union;

void setup() {
    Wire.begin();
    Serial.begin(115200);
    Serial.println("InvenSense MPU-6050");
    Serial.println("June 2012");

    int error;
    uint8_t c;

    error = MPU6050_read(MPU6050_WHO_AM_I, &c, 1);
    Serial.print("WHO_AM_I : ");
    Serial.print(c, HEX);
    Serial.print(", error = ");
    Serial.println(error, DEC);

    error = MPU6050_read(MPU6050_PWR_MGMT_1, &c, 1);
    Serial.print("PWR_MGMT_1 : ");
    Serial.print(c, HEX);
    Serial.print(", error = ");
    Serial.println(error, DEC);

    MPU6050_write_reg(MPU6050_PWR_MGMT_1, 0);

    Serial.print("Waiting to come online");
    for (;;) {
        if ((sakuraio.getConnectionStatus() & SAKURAIO_CONNECTION_STATUS_MASK) != 0x00) {
            Serial.println(" connected");
            break;
        }
        Serial.print(".");
        delay(100);
    }

    Serial.println("***********************");
}

void loop() {
    int error;
    float dT;
    accel_t_gyro_union accel_t_gyro;
    error = MPU6050_read(MPU6050_ACCEL_XOUT_H, (uint8_t * ) & accel_t_gyro, sizeof(accel_t_gyro));
    Serial.print("error=");
    Serial.println(error, DEC);

    uint8_t swap;
#define SWAP(x, y) swap = x; x = y; y = swap
    SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
    SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
    SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
    SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
    SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
    SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
    SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l);

    dT = ((float) accel_t_gyro.value.temperature + 12412.0) / 340.0;
    Serial.print("temperature=");
    Serial.println(dT, 1);
    sakuraio.enqueueTx(CH_TEMP, dT);

    float acc_x = accel_t_gyro.value.x_accel / 16384.0; //FS_SEL_0 16,384 LSB / g
    float acc_y = accel_t_gyro.value.y_accel / 16384.0;
    float acc_z = accel_t_gyro.value.z_accel / 16384.0;

    Serial.print("Accelerometer (xyz)=");
    Serial.print(acc_x, 2);
    Serial.print("\t");
    Serial.print(acc_y, 2);
    Serial.print("\t");
    Serial.println(acc_z, 2);
    sakuraio.enqueueTx(CH_ACC_X, acc_x);
    sakuraio.enqueueTx(CH_ACC_Y, acc_y);
    sakuraio.enqueueTx(CH_ACC_Z, acc_z);

    float gyro_x = accel_t_gyro.value.x_gyro / 131.0;  //FS_SEL_0 131 LSB / (°/s)
    float gyro_y = accel_t_gyro.value.y_gyro / 131.0;
    float gyro_z = accel_t_gyro.value.z_gyro / 131.0;

    Serial.print("Gyrometer (xyz)=");
    Serial.print(gyro_x, 2);
    Serial.print("\t");
    Serial.print(gyro_y, 2);
    Serial.print("\t");
    Serial.println(gyro_z, 2);
    sakuraio.enqueueTx(CH_GYRO_X, gyro_x);
    sakuraio.enqueueTx(CH_GYRO_Y, gyro_y);
    sakuraio.enqueueTx(CH_GYRO_Z, gyro_z);

    sakuraio.send();
    Serial.println("***********************");
    delay(SEND_INTERVAL);
}

// MPU6050_read
int MPU6050_read(int start, uint8_t *buffer, int size) {
    int i, n, error;
    Wire.beginTransmission(MPU6050_I2C_ADDRESS);
    n = Wire.write(start);
    if (n != 1)
        return (-10);
    n = Wire.endTransmission(false);   // hold the I2C-bus
    if (n != 0)
        return (n);
    // Third parameter is true: relase I2C-bus after data is read.
    Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
    i = 0;
    while (Wire.available() && i < size) {
        buffer[i++] = Wire.read();
    }
    if (i != size)
        return (-11);
    return (0);  // return : no error
}

// MPU6050_write
int MPU6050_write(int start, const uint8_t *pData, int size) {
    int n, error;
    Wire.beginTransmission(MPU6050_I2C_ADDRESS);
    n = Wire.write(start);        // write the start address
    if (n != 1)
        return (-20);
    n = Wire.write(pData, size);  // write data bytes
    if (n != size)
        return (-21);
    error = Wire.endTransmission(true); // release the I2C-bus
    if (error != 0)
        return (error);
    return (0);         // return : no error
}

// MPU6050_write_reg
int MPU6050_write_reg(int reg, uint8_t data) {
    int error;
    error = MPU6050_write(reg, &data, 1);
    return (error);
}
