/*
 * image.h
 *
 *  Created on: 16 Apr 2020
 *      Author: jief
 */

#ifndef LIBEG_FILEOPERATION_H_
#define LIBEG_FILEOPERATION_H_

#include "../cpp_foundation/XBuffer.h"

EFI_STATUS InitializeUnicodeCollationProtocol (VOID);
BOOLEAN MetaiMatch (
                    IN CONST CHAR16   *String,
                    IN CONST CHAR16   *Pattern
                    );




EFI_STATUS egLoadFile(const EFI_FILE* BaseDir, IN CONST CHAR16 *FileName, XBuffer<UINT8>* buffer);
EFI_STATUS egSaveFile(const EFI_FILE* BaseDir OPTIONAL, IN CONST CHAR16 *FileName, IN CONST VOID *FileData, IN UINTN FileDataLength);
EFI_STATUS egMkDir(const EFI_FILE* BaseDir OPTIONAL, IN CONST CHAR16 *DirName);


XBool FileExists(const EFI_FILE *BaseDir, IN CONST CHAR16 *RelativePath);
XBool FileExists(const EFI_FILE *BaseDir, IN CONST XStringW& RelativePath);

inline EFI_DEVICE_PATH_PROTOCOL* FileDevicePath (IN EFI_HANDLE Device, IN CONST XStringW& FileName) { return FileDevicePath(Device, FileName.wc_str()); }

XBool DeleteFile(IN EFI_FILE *Root, IN CONST CHAR16 *RelativePath);

EFI_STATUS DirNextEntry(IN EFI_FILE *Directory, IN OUT EFI_FILE_INFO **DirEntry, IN UINTN FilterMode);

typedef struct {
  EFI_STATUS          LastStatus;
  EFI_FILE            *DirHandle;
  BOOLEAN             CloseDirHandle;
  EFI_FILE_INFO       *LastFileInfo;
} REFIT_DIR_ITER;


VOID    DirIterOpen(EFI_FILE *BaseDir, IN CONST CHAR16 *RelativePath OPTIONAL, OUT REFIT_DIR_ITER *DirIter);
BOOLEAN DirIterNext(IN OUT REFIT_DIR_ITER *DirIter, IN UINTN FilterMode, IN CONST CHAR16 *FilePattern OPTIONAL, OUT EFI_FILE_INFO **DirEntry);
EFI_STATUS DirIterClose(IN OUT REFIT_DIR_ITER *DirIter);


XStringW DevicePathToXStringW(const EFI_DEVICE_PATH_PROTOCOL *DevPath);
XStringW FileDevicePathToXStringW(const EFI_DEVICE_PATH_PROTOCOL *DevPath);
XStringW FileDevicePathFileToXStringW(const EFI_DEVICE_PATH_PROTOCOL *DevPath);
//UINTN   FileDevicePathNameLen(IN CONST FILEPATH_DEVICE_PATH  *FilePath);


#endif /* LIBEG_FILEOPERATION_H_ */
