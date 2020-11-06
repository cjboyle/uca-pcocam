# UCA Plugin for PCO CLHS Cameras

### Supports PCO Edge 4.2 and 5.5 cameras using the CameraLink High Speed protocol.

&nbsp;
&nbsp;
## Software Requirements

* Silicon Software microEnable kernel module for Linux (menable.ko)
* Silicon Software Runtime & SDK >= 5.4.4
    ```bash
    # load the environment (e.g. for SISO-RT-5.7.0)
    source /opt/SiliconSoftware/Runtime5.7.0/setup-siso-env.sh

    # install the environment for all users
    sudo install -m 644 /opt/SiliconSoftware/Runtime5.7.0/setup-siso-env.sh /etc/profile.d
    ```
* Applet libAcq_DualCLHSx1AreaRAW.so
   * Installed to ```$SISODIR5/dll/mE5-MA-AF2/``` and flashed to the frame-grabber using ```microDiagnostic```
* PCO CLHS SDK for Linux

    !!! TODO : check that ***ALL*** the libs are symlinked
    ```bash
    # in (TOP)/pco_camera/pco_clhs
    sudo make
    sudo symlink_pco -b  # symlinks libraries to ./bindyn
    sudo symlink_pco -u  # symlinks libraries to /usr/local/lib
    
    # update library paths
    sudo echo 'export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"\n' > \
            /etc/profile.d/setup-usr-local-lib.sh
    sudo chmod 644 /etc/profile.d/setup-usr-local-lib.sh
    # *OR*
    # symlink the libraries to an existing LDD path, e.g.
    sudo symlink_pco /usr/lib
    ```
* UFO-KIT's libuca
    * build and install as instructed


## Plugin Setup & Installation

```bash
# grab a copy of this repo in a convenient location
git clone https://github.lightsource.ca/cid/uca-pcoclhs.git
cd uca-pcoclhs

# build and install
make
sudo make install
```

## Features and Available Settings

## Known Issues and Workarounds

#### **Cannot select different frame-grabber and/or camera**

This is a TODO item, as it seems that ```libuca``` does not provide a method of adding constructor parameters (to my eyes at least, I'm still new to GLib). A method will come in the future to select a grabber and/or camera via properties *after* creating the UcaCamera instance. As it stands, though, the default grabber is the port #0 provided by the driver, and the default camera is the first device that can be found, regardless of its serial port number.

#### **Cannot change between Rolling and Global Shutter modes**

The process to change the camera shutter mode involves rebooting the camera, releasing memory, waiting up to a minute, and reconnecting to a (probably) different software serial port. Due to the unknowns that this may produce, it is recommended that the shutter mode setting be changed in ```pco_camera_grab```, then to create a fresh UCA camera instance.

## Troubleshooting

*NOTE 1*: When troubleshooting, if ```pco_clhs_svc``` or ```pco_clhs_mgr``` is already running, it will need to be terminated before calling ```pco_clhs_mgr``` in a new shell environment.

*NOTE 2*: If neither services are running, and ```pco_clhs_mgr``` still does not fully start, ensure that you are running with sufficient permissions and that the environment variables.

*NOTE 3*: If neither services are running, and ```pco_clhs_mgr``` outputs that it is already started, the Linux machine may need to be rebooted (to clear any orphaned semaphores).

#### **No cameras found**

* Ensure that the SISO GenICam Discovery Service is **not** running
    * ```psgrep $SISODIR5/bin/gs```
    * The service, used only by microDisplay(X), will interfere with all other camera connections (new or existing)
* Ensure that either ```pco_clhs_mgr``` or ```pco_clhs_svc``` is up and running.
* If ```pco_clhs_mgr``` shows a grabber is connected, but does not output camera info (might exit immediately), check the physical connection and power to the camera.

#### **Cannot connect to camera**
* If ```pco_clhs_mgr``` outputs the camera info, and shows 2 or more open connections to the camera, either: a) There is already a process running (e.g. UCA) with a lock on the camera; or, b) A previous successful camera connection crashed unexpectedly, in which case the ```pco_clhs_XXX``` service will need to be restarted (due to unreleased memory).