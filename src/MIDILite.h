/*!
 *  @file       MIDI.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino
 *  @author     Francois Best, lathoub
 *  @date       24/02/11
 *  @license    MIT - Copyright (c) 2015 Francois Best
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include "midi_Defs.h"
#include "midi_Platform.h"
#include "midi_Settings.h"
#include "midi_Message.h"

#include "serialMIDI.h"

// -----------------------------------------------------------------------------

BEGIN_MIDI_NAMESPACE

#define MIDI_LIBRARY_VERSION        0x060000
#define MIDI_LIBRARY_VERSION_MAJOR  6
#define MIDI_LIBRARY_VERSION_MINOR  0
#define MIDI_LIBRARY_VERSION_PATCH  0

/*! \brief The main class for MIDI handling.
It is templated over the type of serial port to provide abstraction from
the hardware interface, meaning you can use HardwareSerial, SoftwareSerial
or ak47's Uart classes. The only requirement is that the class implements
the begin, read, write and available methods.
 */
template<class Transport, class _Settings = DefaultSettings, class _Platform = DefaultPlatform>
class MidiInterface
{
public:
    typedef _Settings Settings;
    typedef _Platform Platform;
    typedef Message MidiMessage;

public:
    inline  MidiInterface(Transport&);
    inline ~MidiInterface();

public:
    void begin(Channel inChannel = 1);

    // -------------------------------------------------------------------------
    // MIDI Output

public:
    inline void sendNoteOn(DataByte inNoteNumber,
                           DataByte inVelocity,
                           Channel inChannel);

    inline void sendNoteOff(DataByte inNoteNumber,
                            DataByte inVelocity,
                            Channel inChannel);

    inline void sendProgramChange(DataByte inProgramNumber,
                                  Channel inChannel);

    inline void sendControlChange(DataByte inControlNumber,
                                  DataByte inControlValue,
                                  Channel inChannel);

    inline void sendPitchBend(int inPitchValue,    Channel inChannel);
    inline void sendPitchBend(double inPitchValue, Channel inChannel);

    inline void sendPolyPressure(DataByte inNoteNumber,
                                 DataByte inPressure,
                                 Channel inChannel) __attribute__ ((deprecated));

    inline void sendAfterTouch(DataByte inPressure,
                               Channel inChannel);
    inline void sendAfterTouch(DataByte inNoteNumber,
                               DataByte inPressure,
                               Channel inChannel);

    inline void sendTimeCodeQuarterFrame(DataByte inTypeNibble,
                                         DataByte inValuesNibble);
    inline void sendTimeCodeQuarterFrame(DataByte inData);

    inline void sendSongPosition(unsigned inBeats);
    inline void sendSongSelect(DataByte inSongNumber);
    inline void sendTuneRequest();

    inline void sendCommon(MidiType inType, unsigned = 0);

    inline void sendClock()         { sendRealTime(Clock); };
    inline void sendStart()         { sendRealTime(Start); };
    inline void sendStop()          { sendRealTime(Stop);  };
    inline void sendTick()          { sendRealTime(Tick);  };
    inline void sendContinue()      { sendRealTime(Continue);  };
    inline void sendActiveSensing() { sendRealTime(ActiveSensing);  };
    inline void sendSystemReset()   { sendRealTime(SystemReset);  };

    inline void sendRealTime(MidiType inType);

    inline void beginRpn(unsigned inNumber,
                         Channel inChannel);
    inline void sendRpnValue(unsigned inValue,
                             Channel inChannel);
    inline void sendRpnValue(byte inMsb,
                             byte inLsb,
                             Channel inChannel);
    inline void sendRpnIncrement(byte inAmount,
                                 Channel inChannel);
    inline void sendRpnDecrement(byte inAmount,
                                 Channel inChannel);
    inline void endRpn(Channel inChannel);

    inline void beginNrpn(unsigned inNumber,
                          Channel inChannel);
    inline void sendNrpnValue(unsigned inValue,
                              Channel inChannel);
    inline void sendNrpnValue(byte inMsb,
                              byte inLsb,
                              Channel inChannel);
    inline void sendNrpnIncrement(byte inAmount,
                                  Channel inChannel);
    inline void sendNrpnDecrement(byte inAmount,
                                  Channel inChannel);
    inline void endNrpn(Channel inChannel);

    inline void send(const MidiMessage&);

public:
    void send(MidiType inType,
              DataByte inData1,
              DataByte inData2,
              Channel inChannel);

    // -------------------------------------------------------------------------
    // MIDI Input

public:
    inline bool read();
    inline bool read(Channel inChannel);

public:
    inline MidiType getType() const;
    inline Channel  getChannel() const;
    inline DataByte getData1() const;
    inline DataByte getData2() const;
    inline bool check() const;

public:
    inline Channel getInputChannel() const;
    inline void setInputChannel(Channel inChannel);

public:
    static inline MidiType getTypeFromStatusByte(byte inStatus);
    static inline Channel getChannelFromStatusByte(byte inStatus);
    static inline bool isChannelMessage(MidiType inType);

    // -------------------------------------------------------------------------
    // MIDI Parsing

private:
    bool parse();
    inline void handleNullVelocityNoteOnAsNoteOff();
    inline bool inputFilter(Channel inChannel);
    inline void resetInput();
    inline void updateLastSentTime();

    // -------------------------------------------------------------------------
    // Transport

public:
    Transport* getTransport() { return &mTransport; };

private:
    Transport& mTransport;

    // -------------------------------------------------------------------------
    // Internal variables

private:
    Channel         mInputChannel;
    StatusByte      mRunningStatus_RX;
    StatusByte      mRunningStatus_TX;
    byte            mPendingMessage[3];
    byte            mPendingMessageExpectedLength;
    unsigned        mPendingMessageIndex;
    unsigned        mCurrentRpnNumber;
    unsigned        mCurrentNrpnNumber;
    MidiMessage     mMessage;
    unsigned long   mLastMessageSentTime;
    unsigned long   mLastMessageReceivedTime;
    unsigned long   mSenderActiveSensingPeriodicity;
    bool            mReceiverActiveSensingActivated;
    int8_t          mLastError;

private:
    inline StatusByte getStatus(MidiType inType,
                                Channel inChannel) const;
};

// -----------------------------------------------------------------------------

END_MIDI_NAMESPACE

#include "MIDI.hpp"
