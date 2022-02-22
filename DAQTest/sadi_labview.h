#ifndef SADI_LABVIEW_H
#define SADI_LABVIEW_H

    #if _MSC_VER > 1000
        #pragma once
    #endif

    #if defined(SADI_LABVIEW_LIBRARY)
    #  define SADI_LABVIEW_API __declspec(dllexport)//Q_DECL_EXPORT
    #else
    #  define SADI_LABVIEW_API __declspec(dllimport)//Q_DECL_IMPORT
    #endif

    // GNU defines
    typedef signed char        int8_t;
    typedef short              int16_t;
    typedef int                int32_t;
    typedef long long          int64_t;
    typedef unsigned char      uint8_t;
    typedef unsigned short     uint16_t;
    typedef unsigned int       uint32_t;
    typedef unsigned long long uint64_t;

    #ifdef __cplusplus
        extern "C" {
    #endif

        //new functions
        /*ANALOG IN*/
        //returns whether the ADC channel is enabled
        SADI_LABVIEW_API bool getAdcInfo_enable(uint8_t adcChannel);

        //returns the gain setting of the ADC channel
        SADI_LABVIEW_API uint8_t getAdcInfo_gain(uint8_t adcChannel);

        /*ANALOG OUT*/
        //returns if the DAC peripheral has been enabled
        SADI_LABVIEW_API bool getDacInfo_enable(uint8_t dacNumber);

        /*ENCODERS*/
        //return whether the encoder has been enabled
        SADI_LABVIEW_API bool getEncoderInfo_enable(uint8_t encoderNumber);
        //return whether the encoder has been enabled to be absolute
        SADI_LABVIEW_API bool getEncoderInfo_absolute(uint8_t encoderNumber);
        //return whether the encoder has been enabled to have an index signal
        SADI_LABVIEW_API bool getEncoderInfo_index(uint8_t encoderNumber);

        SADI_LABVIEW_API uint8_t getEncoderInfo_PLOC(uint8_t encoderNumber);

        //return what resolution the enoder CPR
        SADI_LABVIEW_API uint16_t getEncoderInfo_CPR(uint8_t encoderNumber);

        //return what resolution the enoder resolution = CPR*4
        SADI_LABVIEW_API uint16_t getEncoderInfo_resolution(uint8_t encoderNumber);

        //returns the pin array (boolean) where 1's signify a pin is utilized vs free
        SADI_LABVIEW_API uint16_t getDigitalInfo_pinsAllocated();
        //returns the pin array (boolean) where 1's signify a direction of output.  Note a 0 is either input or non-utilized
        SADI_LABVIEW_API uint16_t getDigitalInfo_pinDirections();
        //returns 1 if any input pins are enabled
        SADI_LABVIEW_API uint16_t getDigitalIn_enable();
        /*Actuators*/

        //returns what motors are enabled
        SADI_LABVIEW_API uint8_t getmotorAlloc();
        //returns what servos are enabled
        SADI_LABVIEW_API uint8_t getservoAlloc();

        /*Other*/

        //returns the loop frequency
        SADI_LABVIEW_API uint16_t getLoopFrequency();

        //returns the current state of the SADI device
        SADI_LABVIEW_API uint8_t getState();


        // Helper Functions
        SADI_LABVIEW_API
            int16_t getLowWord(
                int32_t LongWord
                );

        SADI_LABVIEW_API
            float getVersionNumber_FW(
                void
                );

        SADI_LABVIEW_API
            float getVersionNumber_SW(
                void
                );

        // SADI Control Functions
        SADI_LABVIEW_API
            uint8_t connectSadi(
                void
                );

        SADI_LABVIEW_API
            uint8_t disconnectSadi(
                void
                );

        SADI_LABVIEW_API
            uint8_t sendSetTimerFreq(
                uint16_t freq
                );

        SADI_LABVIEW_API
            uint8_t getData(
                int32_t packet[]
                );

        SADI_LABVIEW_API
            uint8_t getDataLatest(
                int32_t packet[]
                );

        SADI_LABVIEW_API
            uint8_t processPackets(
                void
                );

        SADI_LABVIEW_API
            uint8_t maintenance(
                void
                );

        SADI_LABVIEW_API
            uint16_t sadiRead(
                void
                );

        SADI_LABVIEW_API
            uint16_t sadiReset(
                void
                );

        SADI_LABVIEW_API
            uint8_t getSadiReady(
                void
                );

        SADI_LABVIEW_API
            uint8_t sadiStart(
                void
                );

        SADI_LABVIEW_API
            uint8_t sadiStop(
                void
                );

        // ADC
        SADI_LABVIEW_API
            uint8_t setAdcOffet(
                uint8_t chn,
                double mV,
                uint8_t gain
                );

        SADI_LABVIEW_API
            uint8_t sendAdcInit(
                uint8_t adcMsk,
                uint8_t adcGain
                );

        // DAC
        SADI_LABVIEW_API
            uint8_t sendDacEnable(
                uint8_t DACM
                );

        SADI_LABVIEW_API
            uint8_t sendDacOut(
                uint8_t DACM,
                uint16_t data
                );

        // DIO
        SADI_LABVIEW_API
            uint8_t sendDigitalInEnable(
                uint16_t inputPinMsk
                );

        SADI_LABVIEW_API
            uint8_t sendDigitalOutEnable(
                uint16_t outputPinMsk
                );

        SADI_LABVIEW_API
            uint8_t digitalOutSet(
                uint16_t outputPinMsk
                );

        SADI_LABVIEW_API
            uint8_t digitalOutClr(
                uint16_t outputPinMsk
                );

        // EEPROM
        SADI_LABVIEW_API
            int16_t requestEepromByte(
                uint16_t offset
                );

        SADI_LABVIEW_API
            int32_t requestEepromWord(
                uint16_t offset
                );

        SADI_LABVIEW_API
            float requestEepromFloat(
                uint16_t offset
                );

        SADI_LABVIEW_API
            uint8_t sendEepromByte(
                uint8_t data,
                uint16_t offset
                );

        SADI_LABVIEW_API
            uint8_t sendEepromWord(
                uint16_t data,
                uint16_t offset
                );

        // Encoders
        SADI_LABVIEW_API
            uint8_t sendEnocderInit(
                uint8_t PLOC,
                bool index,
                uint8_t enc_num,
                bool ABS,
                uint8_t EDFL,
                uint16_t res
                );

        // PWM
        SADI_LABVIEW_API
            uint8_t sendMotorEnable(
                uint8_t msk
                );

        SADI_LABVIEW_API
            uint8_t sendMotorDisable(
                uint8_t msk
                );

        SADI_LABVIEW_API
            uint8_t setMotorEffort(
                uint8_t motor,
                uint16_t effort
                );

        SADI_LABVIEW_API
            uint8_t sendServoEnable(
                uint8_t msk
                );

        SADI_LABVIEW_API
            uint8_t sendServoDisable(
                uint8_t msk
                );

        SADI_LABVIEW_API
            uint8_t setServoPosition(
                uint8_t servo,
                uint16_t position
                );

        SADI_LABVIEW_API
            uint8_t sendActuatorEfforts(
                void
                );

        SADI_LABVIEW_API
            uint8_t sendWriteSuperPacket(
                uint16_t AOUT[2]    ,
                uint16_t servoVal[4],
                uint16_t motorVal[4],
                uint16_t digitalSET ,
                uint16_t digitalCLR ,
                uint16_t digitalTGL
                );

        // Long Capture Functions
        SADI_LABVIEW_API
            uint8_t sendLongCaptureInit(
                void
                );

        SADI_LABVIEW_API
            uint8_t sendLongCaptureStart(
                void
                );

        SADI_LABVIEW_API
            uint8_t getLongCaptureReady(
                void
                );

        SADI_LABVIEW_API
            uint8_t getLongCaptureInProgress(
                void
                );

        SADI_LABVIEW_API
            uint8_t getLongCaptureData(
                uint16_t data[]
                );

    #ifdef __cplusplus
        }
    #endif
#endif // SADI_LABVIEW_H
