# UCA Plugin for PCO Cameras

Software plugin for UFO-KIT's Universal Camera Access library to support PCO Edge 4.2/5.5, Pixelfly, PCO4000, and Dimax cameras.

&nbsp;
&nbsp;
## Dependencies

### For all cameras
* UFO-KIT's libuca
   * acquire from https://github.com/ufo-kit/libuca 
   * build and install as instructed
* pco.linux SDK
   * Due to differences between some versions of common source files and libraries, we've installed the SDKs in separate locations:
     - /opt/PCO/pco_camera_clhs/(pco_clhs/ & pco_common/)
     - /opt/PCO/pco_camera_me4/(pco_me4/ & pco_common/)
     - /opt/PCO/pco_camera_usb/(pco_usb/ & pco_common/)

### For CLHS cameras
* Silicon Software microEnable kernel module for Linux (menable.ko)
* Silicon Software Runtime & SDK >= 5.4.4
    ```bash
    # load the environment (e.g. for SISO-RT-5.7.0)
    source /opt/SiliconSoftware/Runtime5.7.0/setup-siso-env.sh

    # install the environment for all users
    sudo install -m 644 /opt/SiliconSoftware/Runtime5.7.0/setup-siso-env.sh /etc/profile.d
    ```
* Applet libAcq_DualCLHSx1AreaRAW.so
   * Installed to ```$SISODIR5/dll/mE5-MA-AF2/``` (or equivalent model directory) and flashed to the frame-grabber using ```microDiagnostic```
* PCO CLHS SDK for Linux
    ```bash
    # in (path to pco.clhs SDK)/pco_camera/pco_clhs
    sudo make
    sudo symlink_pco -b  # symlinks libraries to ./bindyn
    sudo symlink_pco -u  # symlinks libraries to /usr/local/lib
    
    # update library paths
    sudo echo "export LD_LIBRARY_PATH=\"\$LD_LIBRARY_PATH:/usr/local/lib:$PWD/bindyn\"\n" > \
            /etc/profile.d/setup-pco-clhs-env.sh
    sudo chmod 644 /etc/profile.d/setup-pco-clhs-env.sh
    # *OR*
    # symlink the libraries to an existing LDD path, e.g.
    sudo symlink_pco /usr/lib
    ```

### For CameraLink/microEnable4 cameras
* Silicon Software microEnable kernel module for Linux (menable.ko)
* Silicon Software Runtime & SDK >= 5.4.4
    ```bash
    # load the environment (e.g. for SISO-RT-5.7.0)
    source /opt/SiliconSoftware/Runtime5.7.0/setup-siso-env.sh

    # install the environment for all users
    sudo install -m 644 /opt/SiliconSoftware/Runtime5.7.0/setup-siso-env.sh /etc/profile.d
    ```
* Applet ___________ (TODO, camera-dependent)
   * Installed to ```$SISODIR5/dll/YOUR_FRAMEGRABBER_CARD/``` (or equivalent model directory) and flashed to the frame-grabber using ```microDiagnostic```

### For USB cameras
* libusb
   * For most systems, install ```libusb-1.0``` and ```libusb-1.0-devel```
   * For some Yum systems, use instead ```libusbx``` and ```libusbx-devel```

---
## UCA plugin Setup & Installation

```bash
# grab a copy of this repo in a convenient location
git clone https://github.lightsource.ca/cid/uca-pcoclhs.git
cd uca-pcoclhs

# build and install all libs
make
sudo make install

# build and install libucapcoclhs.so only
make clhs
sudo make install-clhs

# build and install libucapcome4.so only
make me4
sudo make install-me4

# build and install libucapcousb.so only
make usb
sudo make install-usb
```

To verify the installation, the ```uca-info``` can be used to show the available plugins:
```bash
$ uca-info
Usage: uca-info [ mock, file, net, pcoclhs, pcousb ]
```

Use ```uca-info [plugin]``` and ```uca-grab -n 1 [plugin]``` to verify basic functionality.

---
## Available Settings

| Property | type, units | R/W | CLHS | ME4 | USB |
|:--------:|:-----------:|:---:|:----:|:---:|:---:|
| name | str | R | yes | yes | yes |
| version | str | R | yes | yes | yes |
| sensor-width | int, px | R | yes | yes | yes |
| sensor-height | int, px | R | yes | yes | yes |
| sensor-width-extended | int, px | R | yes | yes | yes |
| sensor-height-extended | int, px | R | yes | yes | yes |
| sensor-extended | bool | R | yes | yes | yes |
| sensor-bitdepth | int, bits | R | yes | yes | yes |
| sensor-pixel-width | float, m | R | yes | yes | yes |
| sensor-pixel-height | float, m | R | yes | yes | yes |
| sensor-pixelrate | float, Hz | RW | yes | yes | yes |
| sensor-pixelrates | \[float\], Hz | R | yes | yes | yes |
| sensor-horizontal-binning | int, step | RW | yes | yes | yes |
| sensor-vertical-binning | int, step | RW | yes | yes | yes |
| sensor-temperature | int, degC | R | yes | yes | yes |
| sensor-adcs | int, count | RW | no | yes | yes |
| roi-x0 | int, px | RW | yes | yes | yes |
| roi-y0 | int, px | RW | yes | yes | yes |
| roi-width | int, px | RW | yes | yes | yes |
| roi-height | int, px | RW | yes | yes | yes |
| roi-width-multiplier | int, step | RW | yes | yes | yes |
| roi-height-multiplier | int, step | RW | yes | yes | yes |
| exposure-time | float, s | RW | yes | yes | yes |
| delay-time | float, s | RW | yes | yes | yes |
| frame-grabber-timeout | int, ms | RW | yes | yes | yes |
| frames-per-second | float, fps | RW | yes | yes | yes |
| trigger-source | enum | RW | yes | yes | yes |
| double-image-mode | bool | RW | yes* | yes* | yes* |
| offset-mode | bool | RW | yes* | yes* | yes* |
| acquire-mode | bool | RW | yes* | yes* | yes* |
| fast-scan | bool | RW | yes* | yes* | yes* |
| cooling-point | int, degC | RW | yes* | yes* | yes* |
| cooling-point-default | int, degC | R | yes* | yes* | yes* |
| noise-filter | bool | RW | yes* | yes* | yes* |
| timestamp-mode | enum | RW | yes | yes | yes |
| is-recording | bool | R | yes | yes | yes |
| buffered | bool | RW | yes | yes | yes |
| num-buffers | int, count | RW | yes | yes | yes |
| record-mode | enum | RW | no | yes* | no |
| storage-mode | enum | RW | no | yes* | no |
| transfer-asynchronously | bool | RW | yes | yes | yes |
| edge-global-shutter | bool | RW | notimp | notimp | notimp |

\* May not be supported by specific camera models

| Method | CLHS | ME4 | USB |
|:------:|:----:|:---:|:---:|
| start_recording() | yes | yes | yes |
| stop_recording() | yes | yes | yes |
| start_readout() | no | yes* | no |
| stop_readout() | no | yes* | no |
| grab() | yes | yes | yes |
| grab_async() | yes | yes | yes |
| trigger() | yes | yes | yes |

---
## Known Issues, TODOs, and Workarounds

- [ ] Allow selecting a specific grabber and/or camera by port number 
- [ ] Allow changing a camera's shutter mode (rolling, global)
   - Use ```pco_switch_edge``` (or equivalent), then create a new UcaCamera instance (setting will persist reboots)
- [ ] Included static libraries for CLHS and ME4 cameras seem to have linker/LD_PRELOAD issues on RHEL systems
   - Use shared object libraries instead.

---
## Troubleshooting

*NOTE 1*: When troubleshooting, if ```pco_clhs_svc``` or ```pco_clhs_mgr``` is already running, it will need to be terminated before calling ```pco_clhs_mgr``` in a new shell environment.

*NOTE 2*: If neither services are running, and ```pco_clhs_mgr``` does not fully start, ensure that you are running with sufficient permissions and that the environment variables are set (running a process with ```sudo```/```dzdo``` will not load variables of the parent environment).

*NOTE 3*: If neither services are running, and ```pco_clhs_mgr``` outputs that it is already started, the Linux machine may need to be rebooted (to clear any orphaned semaphores in memory).

#### **No cameras found**

* Ensure that the SISO GenICam Discovery Service is **not** running
    * ```psgrep $SISODIR5/bin/gs```
    * The service, used only by microDisplay(X), will interfere with all other camera connections (new or existing)
* Ensure that either ```pco_clhs_mgr``` or ```pco_clhs_svc``` is up and running.
* If ```pco_clhs_mgr``` shows a grabber is connected, but does not output camera info (might exit immediately), check the physical connection and power to the camera.

#### **Cannot connect to camera**
* If ```pco_clhs_mgr``` outputs the camera info, and shows 2 or more open connections to the camera, it is probable that a previously successful camera connection crashed unexpectedly (e.g. segmentation fault), in which case the ```pco_clhs_mgr``` service will need to be restarted (due to unreleased memory). If the service fails to be restarted, follow the appropriate troubleshooting steps.

---
## License
Please see [LICENSE.txt](LICENSE.txt)

Sources and binaries under ```pco``` directory are from PCO AG and are under respective copyright (GPL2).
