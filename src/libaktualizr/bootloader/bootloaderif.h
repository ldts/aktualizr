#ifndef BOOTLOADERIF_H_
#define BOOTLOADERIF_H_

class BootloaderIF {
 public:
  virtual ~BootloaderIF(){};
  virtual void setBootOK() const = 0;
  virtual void updateNotify() const = 0;
  virtual bool supportRebootDetection() const = 0;
  virtual bool rebootDetected() const = 0;
  virtual void rebootFlagSet() = 0;
  virtual void rebootFlagClear() = 0;
  virtual void reboot() = 0;
  virtual void fakeReboot() = 0;
};

#endif
