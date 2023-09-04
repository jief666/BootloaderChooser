# BootloaderChooser
#### Choose on which efi you'd like to boot

This is the solution if you :
- are not sure on which file his EFI bios is booting
- are annoyed because clover and oc efi has to be named CloverX64.efi and OpenCore.efi. Hence, you can't easily see the version
- ares tired of renaming efi files to try different version (that's the consequences of the previous point).
- find not great the need to have a bootable Clover/OC backup on a USB stick in case something goes wrong at update or config modification.
- you also tired to often rename Clover folders to use them.

### What is it :

This tool is a menu that allows you to choose which bootloader you wanna load. It designed to choose between Clover and OpenCore.

You can name your efi file anything you want (Clover-v5119, Clover-v5122, OC-v0.6.1, etc.).

You can also have many folders in your EFI folder. Although to boot Clover from a folder named anything else than CLOVER, you'll need a new version. To boot OpenCore from a folder named anything else than OC, you'll need my version.


### Here is how it looks :

![Screenshot 2020-10-06 at 19 59 49](https://user-images.githubusercontent.com/3094233/95235634-85a9e380-080e-11eb-8cfe-0e2cc04ab903.png)


### How to install

- NOTE : For people who can create "specific file boot entry" in their bios, remove all of them.
- Put [BootX64.efi](efi_files/BootX64.efi) in folder EFI/BOOT and voilà, all your problems are solved : no more renaming anymore !

### Usage

The first time, you'll get the menu. Choose one bootloader.

The next times, you won't see the menu.  It'll immediately load the bootloader you chose before, without displaying a menu. You'll forgot it's even there !
When you need the menu back, just boot your computer with a key pressed.

### NOTE

Booting Clover and OC from a folder not named CLOVER or OC :

- Clover : you need Clover version 5126 or later. If you use an earlier Clover, BootloaderChooser will refuse to start it.
- OpenCore : you need a version 0.6.4 and later. ATTENTION, if you use a previous version of OC in a renamed folder, it will silently use config.plist, drivers, kext, etc... from EFI\OC instead.



###### Let me know what you think...
