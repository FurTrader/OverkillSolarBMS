# Overkill Solar Mobile App APK files

This folder contains the APK files for our mobile app, 
for users who prefer to sideload the app instead of using the Google Play Store.

Find the version you want, download the APK to your device, navigate to it in your file manager, and install it.    
You may have to enable permissions to install 3rd party apps.

### OTA updates

The app automatically receives OTA updates if it has internet access. When you load the APK you will initially have the version number of the last package release. If connected to the internet, it will silently update to the latest version, until the next new release.

If you do want to use the Play Store, it's here: https://play.google.com/store/apps/details?id=com.marchingband.overkillsolar

# Version Notes:

### VERSION 1.5.3 - Sept 5th, 2023
    - Bug Fix: NRC errors causing crash.

### VERSION 1.5.2 - Sept 4th, 2023
    - Bug Fix: ScrollView in tools menu should be full page.
    - Bug Fix: ScrollView in tools menu should be padded less for keyboard on Android.

### VERSION 1.5.1 - Sept 4th, 2023
    - Feature Added: Help button with link to open docs, and send diagnostic report via email.
    - Feature Added: New presets for Tesla.
    - Feature Added: Display delimiting commas in tools tab for large integers.
    - Feature Added: Auto-scroll to input fields when keypad opens.
    - Feature Added: Versioned preset pack auto updates intelligently.
    - Bug Fix: Respect BLE name character limit.

### VERSION 1.5.0 - July 20th, 2023
    - Feature Added: Tools Tab with Ohms Law Calculator and Resistor Color Calculator
### VERSION 1.4.1 - May 22nd, 2023
    - Feature Added: display NTC errors, alert on NTC errors, offer dialogue with NTC error info.
    - Bug Fix: NTC should show on settings tab when disabled.
    - Bug Fix: Current Calibration should write to BMS in units of 10mA.
    - Bug Fix: BMS should not enumerate in devices tab until MAC has been obtained.
### VERSION 1.4.0 - Apr 9th, 2023   
    - Upgrade: Compatibility with Android 13 and iOS 14, via upgrade to Expo SDK 46
    - Feature added: Export and Import .fig files
    - Feature added: Add Level 2 and 3 advanced protection controls
    - Bug fix: Correct permissions on Android, via manual AndroidManifest.xml authoring.
    - Bug fix: Cell numbering on Calibration page was incorrect
    - Bug fix: Negative numbers are impossible to enter on iOS, fixed by switching to alpha-numeric keyboard for fields that require negative numbers
    - Bug fix: Discharge Overcurrent register was inverted
    - Bug fix: Load and Save screens dont scroll
    - Bug fix: Devices screen lags with large number of BMSs, fixed by throttling Bluetooth Scan
    - Bug fix: Some unit labels were incorrect, and some values were displayed with incorrect precision
    - Bug fix: Occasional crash on iOS at launch, data race condition without proper error handling
### VERSION 1.3.2-V10 - Jan 27th, 2023    
    - Bug fix: Current Calibration should display mA’s, previously showed 10mA’s
    - Bug fix: Current units should be mA, previously was mAh
    - All current values should display with 1 decimal of precision
### VERSION 1.3.1-V10 - Jan 10th, 2023    
    - Bug fix: Max Power Discharge tracks the largest number (previously tracked the lowest)
### VERSION 1.3.0-V10 - Jan 10th, 2023   
    - Bug fix: clicking on “settings” when no BMS connected should display “no connectd BMS”
    - Feature added: Sessions Values displays both Max Charge Power and Max Discharge Power
### VERSION 1.2.2-V10 - Dec 16th, 2022   
    - Bug fix: Charge Under Current should read Discharge Over Current
### VERSION 1.2.1-V10 - Dec 8th, 2022   
    - Bug fix: BMS-write-needed indicators persist navigation, and reset on disconnect and on read-BMS
    - Bug fix: global ble variable should be instantiated at launch
### VERSION 1.2.1-V10 - Dec 8th, 2022   
    - Bug fix: BMS-write-needed indicators persist navigation, and reset on disconnect and on read-BMS
    - Bug fix: global ble variable should be instantiated at launch
### VERSION 1.2.0-V10 - Dec 6th, 2022    
    - Bug fix: Device displays MAC address on iOS, previously it showed UUID
    - Feature added: Configuration fields present gold border if they have been modified
    - Feature added: Write BMS button flashes if there are configuration changes that need to be written to BMS
    - Feature added: Temperature Units persist between app restarts, default unit changed to Celsius
### VERSION 1.1.1-V10 - Dec 6th, 2022    
    - Bug fix: Added runtime and manifest permissions on Android
### VERSION 1.1.1 - Nov 22nd, 2022    
    - Bug fix: Cell Balance Active indicators correspond to the correct cells, they were (still) incorrectly associated previously
    - known non-compatible devices:
        APPLICATION CRASH
        - LGE LG G Pad F 8.0 (Android 6.0)
        DEVICE SCAN FAILURE
        - Nokia g10 (Android 10)
        - Google Pixel 6 Pro (Android 13)
        - Google Pixel 4a (Android 13)
        - Galaxy S10+ (Android 12)
        - Moto g pure (Android 12)
        - Samsung s22 ultra
        - Samsung flip 5 (Android 12)
        - Galaxy A51 (Android 11)
        - Galaxy A52 5G (Android 12)
### VERSION 1.1.0 - Nov 21st, 2022    
        - Feature added: BMS configuration data automatically loads when the user navigates to the “settings” tab
        - UI update: Charge and Discharge indicator icons animate when active
        - Bug fix: Keyboard automatically dismisses when user presses “Write to BMS” patching a bug where numeric entry was ignored untill the user closed the keyboard
        - Bug fix: Load file and Save file titles were corrected, they were previously reversed
        - Bug fix: Cell Balance Active indicators correspond to the correct cells, they were incorrectly associated previously
        - known non-compatible devices:
        APPLICATION CRASH
        - LGE LG G Pad F 8.0 (Android 6.0)
        DEVICE SCAN FAILURE
        - Nokia g10 (Android 10)
        - Google Pixel 6 Pro (Android 13)
        - Google Pixel 4a (Android 13)
        - Galaxy S10+ (Android 12)
        - Moto g pure (Android 12)
        - Samsung s22 ultra
        - Samsung flip 5 (Android 12)
        - Galaxy A51 (Android 11)
        - Galaxy A52 5G (Android 12)
### VERSION 1.0.0 - Nov 17th, 2022    
        - inital release










