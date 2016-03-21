/**********************************************************
*
* This is a part of TOTEM offline software.
* Authors:
*    Jan Kaspar (jan.kaspar@gmail.com)
*
**********************************************************/

#ifndef _Totem_VFATFrame_h_
#define _Totem_VFATFrame_h_

#include <vector>
#include <cstddef>

/**
 * Representation of VFAT frame plus extra info added by DAQ.
**/
class VFATFrame
{
  public:
    typedef unsigned short word;

  public:
    VFATFrame(word* _data = NULL);

    VFATFrame(const VFATFrame& copy)
    {
      setData(copy.data);
      presenceFlags = copy.presenceFlags;
      daqErrorFlags = copy.daqErrorFlags;
    }

    virtual ~VFATFrame() {}

    /// Copies a memory block to data buffer.
    void setData(const word *_data);

    VFATFrame::word* getData()
    {
      return data;
    }

    /// Returns Bunch Crossing number (BC<11:0>).
    VFATFrame::word getBC() const
    {
      return data[11] & 0x0FFF;
    }

    /// Returns Event Counter (EV<7:0>).
    VFATFrame::word getEC() const
    {
      return (data[10] & 0x0FF0) >> 4;
    }

    /// Returns flags.
    VFATFrame::word getFlags() const
    {
      return data[10] & 0x000F;
    }

    /// Returns ChipID (ChipID<11:0>).
    VFATFrame::word getChipID() const
    {
      return data[9] & 0x0FFF;
    }

    /// Returns the CRC.
    VFATFrame::word getCRC() const
    {
      return data[0];
    }

    /// Returns true if the BC word is present in the frame.
    bool isBCPresent() const
    {
      return presenceFlags & 0x1;
    }

    /// Returns true if the EC word is present in the frame.
    bool isECPresent() const
    {
      return presenceFlags & 0x2;
    }

    /// Returns true if the ID word is present in the frame.
    bool isIDPresent() const
    {
      return presenceFlags & 0x4;
    }

    /// Returns true if the CRC word is present in the frame.
    bool isCRCPresent() const
    {
      return presenceFlags & 0x8;
    }

    /// Checks the fixed bits in the frame.
    /// Returns false if any of the groups (in BC, EC and ID words) is present but wrong.
    bool checkFootprint() const;

    /// Checks the validity of frame (CRC and daqErrorFlags).
    /// Returns false if daqErrorFlags is non-zero.
    /// Returns false if the CRC is present and invalid.
    virtual bool checkCRC() const;

    /// Checks if channel number 'channel' was active.
    /// Returns positive number if it was active, 0 otherwise.
    virtual bool channelActive(unsigned char channel) const
    {
      return ( data[1 + (channel / 16)] & (1 << (channel % 16)) ) ? 1 : 0;
    }

    /// Returns list  of active channels.
    /// It's more efficient than the channelActive(char) for events with low channel occupancy.
    virtual std::vector<unsigned char> getActiveChannels() const;

    /// Prints the frame.
    /// If binary is true, binary format is used.
    void Print(bool binary = false) const;

  public:
    /** Raw data frame as sent by electronics.
    * The container is organized as follows (reversed Figure 8 at page 23 of VFAT2 manual):
    * \verbatim
    * buffer index   content       size
    * ---------------------------------------------------------------
    *   0            CRC           16 bits
    *   1->8         Channel data  128 bits, channel 0 first
    *   9            ChipID        4 constant bits (1110) + 12 bits
    *   10           EC, Flags     4 constant bits (1100) + 8, 4 bits
    *   11           BC            4 constant bits (1010) + 12 bits
    * \endverbatim
    **/
    word data[12];

    /// Flag indicating the presence of various components.
    ///   bit 1: "BC word" (buffer index 11)
    ///   bit 2: "EC word" (buffer index 10)
    ///   bit 3: "ID word" (buffer index 9)
    ///   bit 4: "CRC word" (buffer index 0)
    word presenceFlags;

    /// Error flag as given by certain versions of DAQ.
    word daqErrorFlags;

    /// internaly used to check CRC
    static word calculateCRC(word crc_in, word dato);
};                                                                     

#endif