
# XcaliburInjector
![Build](https://github.com/Ryzee119/XcaliburInjector/workflows/Build/badge.svg)  
Inject registers values into Xcalibur Video Encoder on a Original Xbox console with a v1.6 motherboard.
This proof of concept (PoC) was written to fix Xcalibur setting issues on a Xbox v1.6 console when running XeniumOS at 480p.
Usage:
1. Hook Arduino to Xboxes SMBus, (SDA,SCL and GND), wait for Xenium to boot into XeniumOS. Confirm you have graphical issues as described here https://github.com/Ryzee119/OpenXenium/issues/2.
2. Program this code to the Arduino to inject new SMBus registers.
3. This is purely a PoC and the injected values will be lost each reboot, so its not particulary useful.
![enter image description here](https://raw.githubusercontent.com/Ryzee119/XcaliburInjector/master/image.jpg)
