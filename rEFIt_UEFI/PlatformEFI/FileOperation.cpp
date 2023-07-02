/*
 * libeg/image.c
 * Image handling functions
 *
 * Copyright (c) 2006 Christoph Pfisterer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *  * Neither the name of Christoph Pfisterer nor the names of the
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <Platform.h>

#define MAX_FILE_SIZE (1024*1024*1024)

#ifndef DEBUG_ALL
#define DEBUG_IMG 1
#else
#define DEBUG_IMG DEBUG_ALL
#endif

#if DEBUG_IMG == 0
#define DBG(...)
#else
#define DBG(...) DebugLog(DEBUG_IMG, __VA_ARGS__)
#endif

#include "../Platform/FileOperation.h"

//
// Unicode collation protocol interface
//
EFI_UNICODE_COLLATION_PROTOCOL *mUnicodeCollation = NULL;

//
// file name manipulation
//
BOOLEAN
MetaiMatch (
            IN CONST CHAR16   *String,
            IN CONST CHAR16   *Pattern
            )
{
  if (!mUnicodeCollation) {
    // quick fix for driver loading on UEFIs without UnicodeCollation
    //return FALSE;
    return TRUE; //this is wrong anyway
  }
  return mUnicodeCollation->MetaiMatch (mUnicodeCollation, String, Pattern);
}


EFI_STATUS
InitializeUnicodeCollationProtocol (VOID)
{
  EFI_STATUS  Status;

  if (mUnicodeCollation != NULL) {
    return EFI_SUCCESS;
  }

  //
  // BUGBUG: Proper impelmentation is to locate all Unicode Collation Protocol
  // instances first and then select one which support English language.
  // Current implementation just pick the first instance.
  //
  Status = gBS->LocateProtocol (
                                gEfiUnicodeCollation2ProtocolGuid,
                                NULL,
                                (VOID **) &mUnicodeCollation
                                );
  if (EFI_ERROR(Status)) {
    Status = gBS->LocateProtocol (
                                  gEfiUnicodeCollationProtocolGuid,
                                  NULL,
                                  (VOID **) &mUnicodeCollation
                                  );

  }
  return Status;
}




//
// Basic file operations should be separated into separate file
//
EFI_STATUS egLoadFile(const EFI_FILE* BaseDir, IN CONST CHAR16 *FileName, XBuffer<UINT8>* xBuffer)
{
  EFI_STATUS          Status = EFI_NOT_FOUND;
  EFI_FILE_HANDLE     FileHandle = 0;
  EFI_FILE_INFO       *FileInfo;
  uint64_t              ReadSize;
//  UINTN               BufferSize;
//  UINT8               *Buffer;

  if (!BaseDir) {
    return EFI_UNSUPPORTED;
  }

  Status = BaseDir->Open(BaseDir, &FileHandle, FileName, EFI_FILE_MODE_READ, 0); // const missing in EFI_FILE_HANDLE->Open
  if (EFI_ERROR(Status) || !FileHandle) {
    return EFI_UNSUPPORTED;
  }

  FileInfo = EfiLibFileInfo(FileHandle);
  if (FileInfo == NULL) {
    FileHandle->Close(FileHandle);
    return EFI_NOT_READY;
  }
  ReadSize = FileInfo->FileSize;
  if (ReadSize > MAX_FILE_SIZE)
    ReadSize = MAX_FILE_SIZE;
  FreePool(FileInfo);

  xBuffer->setSize(ReadSize, 0);
//  BufferSize = (UINTN)ReadSize;   // was limited to 1 GB above, so this is safe
//  Buffer = (UINT8 *) AllocatePool (BufferSize);
//  if (Buffer == NULL) {
//    FileHandle->Close(FileHandle);
//    Status = EFI_OUT_OF_RESOURCES;
//    goto Error;
//  }

  UINTN xBufferSize = xBuffer->size();
  Status = FileHandle->Read(FileHandle, &xBufferSize, xBuffer->dataSized(ReadSize));
  FileHandle->Close(FileHandle);
  if (EFI_ERROR(Status)) {
    xBuffer->setEmpty();
    return Status;
  }
  if ( xBufferSize != xBuffer->size() ) panic("xBufferSize != xBuffer->size()");
  return EFI_SUCCESS;
}

EFI_STATUS egSaveFile(const EFI_FILE* BaseDir OPTIONAL, IN CONST CHAR16 *FileName,
                      IN CONST VOID *FileData, IN UINTN FileDataLength)
{
  EFI_STATUS          Status;
  EFI_FILE_HANDLE     FileHandle;
  UINTN               BufferSize;
  BOOLEAN             CreateNew = TRUE;
  CONST CHAR16        *p = FileName + StrLen(FileName);
  CHAR16              DirName[256];
  UINTN               dirNameLen;

  if (BaseDir == NULL) {
    return EFI_UNSUPPORTED;
  }
    
  // syscl - make directory if not exist
  while (*p != L'\\' && p > FileName) {
    // find the first '\\' traverse from the end to head of FileName
    p -= 1;
  }
  if ( p > FileName )
  {
    dirNameLen = p - FileName;
    StrnCpyS(DirName, 256, FileName, dirNameLen);
    DirName[dirNameLen] = L'\0';
    Status = BaseDir->Open(BaseDir, &FileHandle, DirName,
                             EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, EFI_FILE_DIRECTORY);

    if (EFI_ERROR(Status)) {
        // make dir
  //    DBG("no dir %s\n", efiStrError(Status));
        Status = BaseDir->Open(BaseDir, &FileHandle, DirName,
                                 EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, EFI_FILE_DIRECTORY);
  //    DBG("cant make dir %s\n", efiStrError(Status));
    }
    // end of folder checking
  }
  // Delete existing file if it exists
  Status = BaseDir->Open(BaseDir, &FileHandle, FileName,
                         EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);
  if (!EFI_ERROR(Status)) {
    Status = FileHandle->Delete(FileHandle);
    if (Status == EFI_WARN_DELETE_FAILURE) {
      //This is READ_ONLY file system
      CreateNew = FALSE; // will write into existing file (Slice - ???)
//      DBG("RO FS %s\n", efiStrError(Status));
    }
  }

  if (CreateNew) {
    // Write new file
    Status = BaseDir->Open(BaseDir, &FileHandle, FileName,
                           EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);
    if (EFI_ERROR(Status)) {
//      DBG("no write %s\n", efiStrError(Status));
      return Status;
    }
  } else {
    //to write into existing file we must sure it size larger then our data
    EFI_FILE_INFO *Info = EfiLibFileInfo(FileHandle);
    if (Info) {
      if (Info->FileSize < FileDataLength) {
//        DBG("no old file %s\n", efiStrError(Status));
        return EFI_NOT_FOUND;
      }
      FreePool(Info);
    }
  }

  if (!FileHandle) {
//    DBG("no FileHandle %s\n", efiStrError(Status));
    return EFI_DEVICE_ERROR;
  }

  BufferSize = FileDataLength;
  Status = FileHandle->Write(FileHandle, &BufferSize, (VOID*)FileData); // CONST missing in EFI_FILE_HANDLE->write
  FileHandle->Close(FileHandle);
//  DBG("not written %s\n", efiStrError(Status));
  return Status;
}


EFI_STATUS egMkDir(const EFI_FILE* BaseDir OPTIONAL, IN CHAR16 *DirName)
{
  EFI_STATUS          Status;
  EFI_FILE_HANDLE     FileHandle;

  //DBG("Looking up dir assets (%ls):", DirName);

  if (BaseDir == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = BaseDir->Open(BaseDir, &FileHandle, DirName,
                         EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, EFI_FILE_DIRECTORY);

  if (EFI_ERROR(Status)) {
    // Write new dir
    //DBG("%s, attempt to create one:", efiStrError(Status));
    Status = BaseDir->Open(BaseDir, &FileHandle, DirName,
                           EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, EFI_FILE_DIRECTORY);
  }

  //DBG(" %s\n", efiStrError(Status));
  return Status;
}


//
// file and dir functions
//

XBool FileExists(const EFI_FILE *Root, IN CONST CHAR16 *RelativePath)
{
  EFI_STATUS  Status;
  EFI_FILE    *TestFile = NULL;

  Status = Root->Open(Root, &TestFile, RelativePath, EFI_FILE_MODE_READ, 0);
  if (Status == EFI_SUCCESS) {
    if (TestFile && TestFile->Close) {
      TestFile->Close(TestFile);
    }
    return true;
  }
  return false;
}

XBool FileExists(const EFI_FILE *Root, IN CONST XStringW& RelativePath)
{
  return FileExists(Root, RelativePath.wc_str());
}

XBool DeleteFile(IN EFI_FILE *Root, IN CONST CHAR16 *RelativePath)
{
  EFI_STATUS  Status;
  EFI_FILE    *File;
  EFI_FILE_INFO   *FileInfo;

  //DBG("DeleteFile: %ls\n", RelativePath);
  // open file for read/write to see if it exists, need write for delete
  Status = Root->Open(Root, &File, RelativePath, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);
  //DBG(" Open: %s\n", efiStrError(Status));
  if (Status == EFI_SUCCESS) {
    // exists - check if it is a file
    FileInfo = EfiLibFileInfo(File);
    if (FileInfo == NULL) {
      // error
      //DBG(" FileInfo is NULL\n");
      File->Close(File);
      return false;
    }
    //DBG(" FileInfo attr: %hhX\n", FileInfo->Attribute);
    if ((FileInfo->Attribute & EFI_FILE_DIRECTORY) == EFI_FILE_DIRECTORY) {
      // it's directory - return error
      //DBG(" File is DIR\n");
      FreePool(FileInfo);
      File->Close(File);
      return false;
    }
    FreePool(FileInfo);
    // it's a file - delete it
    //DBG(" File is file\n");
    Status = File->Delete(File);
    //DBG(" Delete: %s\n", efiStrError(Status));

    return Status == EFI_SUCCESS;
  }
  return false;
}

EFI_STATUS DirNextEntry(IN EFI_FILE *Directory, IN OUT EFI_FILE_INFO **DirEntry, IN UINTN FilterMode)
{
  EFI_STATUS Status;
  VOID *Buffer;
  UINTN LastBufferSize, BufferSize;
  INTN IterCount;

  for (;;) {

    // free pointer from last call
    if (*DirEntry != NULL) {
      FreePool(*DirEntry);
      *DirEntry = NULL;
    }

    // read next directory entry
    LastBufferSize = BufferSize = 256;
    Buffer = (__typeof__(Buffer))AllocateZeroPool(BufferSize);
    for (IterCount = 0; ; IterCount++) {
      Status = Directory->Read(Directory, &BufferSize, Buffer);
      if (Status != EFI_BUFFER_TOO_SMALL || IterCount >= 4)
        break;
      if (BufferSize <= LastBufferSize) {
      DBG("FS Driver requests bad buffer size %llu (was %llu), using %llu instead\n", BufferSize, LastBufferSize, LastBufferSize * 2);
        BufferSize = LastBufferSize * 2;
#if REFIT_DEBUG > 0
      } else {
      DBG("Reallocating buffer from %llu to %llu\n", LastBufferSize, BufferSize);
#endif
      }
      Buffer = (__typeof__(Buffer))EfiReallocatePool(Buffer, LastBufferSize, BufferSize);
      LastBufferSize = BufferSize;
    }
    if (EFI_ERROR(Status)) {
      FreePool(Buffer);
      break;
    }

    // check for end of listing
    if (BufferSize == 0) {    // end of directory listing
      FreePool(Buffer);
      break;
    }

    // entry is ready to be returned
    *DirEntry = (EFI_FILE_INFO *)Buffer;
    if (*DirEntry) {
      // filter results
      if (FilterMode == 1) {   // only return directories
        if (((*DirEntry)->Attribute & EFI_FILE_DIRECTORY))
          break;
      } else if (FilterMode == 2) {   // only return files
        if (((*DirEntry)->Attribute & EFI_FILE_DIRECTORY) == 0)
          break;
      } else                   // no filter or unknown filter -> return everything
        break;
    }
  }
  return Status;
}

VOID DirIterOpen(EFI_FILE *BaseDir, IN CONST CHAR16 *RelativePath OPTIONAL, OUT REFIT_DIR_ITER *DirIter)
{
  if (RelativePath == NULL) {
    DirIter->LastStatus = EFI_SUCCESS;
    DirIter->DirHandle = BaseDir;
    DirIter->CloseDirHandle = FALSE;
  } else {
    DirIter->LastStatus = BaseDir->Open(BaseDir, &(DirIter->DirHandle), RelativePath, EFI_FILE_MODE_READ, 0);
    DirIter->CloseDirHandle = EFI_ERROR(DirIter->LastStatus) ? FALSE : TRUE;
  }
  DirIter->LastFileInfo = NULL;
}

BOOLEAN DirIterNext(IN OUT REFIT_DIR_ITER *DirIter, IN UINTN FilterMode, IN CONST CHAR16 *FilePattern OPTIONAL,
                    OUT EFI_FILE_INFO **DirEntry)
{
  if (DirIter->LastFileInfo != NULL) {
    FreePool(DirIter->LastFileInfo);
    DirIter->LastFileInfo = NULL;
  }

  if (EFI_ERROR(DirIter->LastStatus))
    return FALSE;   // stop iteration

  for (;;) {
    DirIter->LastStatus = DirNextEntry(DirIter->DirHandle, &(DirIter->LastFileInfo), FilterMode);
    if (EFI_ERROR(DirIter->LastStatus))
      return FALSE;
    if (DirIter->LastFileInfo == NULL)  // end of listing
      return FALSE;
    if (FilePattern != NULL) {
      if ((DirIter->LastFileInfo->Attribute & EFI_FILE_DIRECTORY))
        break;
      if (MetaiMatch(DirIter->LastFileInfo->FileName, FilePattern))
        break;
      // else continue loop
    } else
      break;
  }

  *DirEntry = DirIter->LastFileInfo;
  return TRUE;
}

EFI_STATUS DirIterClose(IN OUT REFIT_DIR_ITER *DirIter)
{
  if (DirIter->LastFileInfo != NULL) {
    FreePool(DirIter->LastFileInfo);
    DirIter->LastFileInfo = NULL;
  }
  if (DirIter->CloseDirHandle)
    DirIter->DirHandle->Close(DirIter->DirHandle);
  return DirIter->LastStatus;
}




/**
 This function converts an input device structure to a Unicode string.

 @param DevPath                  A pointer to the device path structure.

 @return A new allocated Unicode string that represents the device path.

 **/
XStringW DevicePathToXStringW (
    const EFI_DEVICE_PATH_PROTOCOL     *DevPath
  )
{
  CHAR16* DevicePathStr = ConvertDevicePathToText (DevPath, TRUE, TRUE);
  XStringW returnValue;
  returnValue.stealValueFrom(DevicePathStr); // do not FreePool FilePath, it's now owned by returnValue
  return returnValue;

}

//
// Aptio UEFI returns File DevPath as 2 nodes (dir, file)
// and DevicePathToStr connects them with /, but we need '\\'
XStringW FileDevicePathToXStringW(const EFI_DEVICE_PATH_PROTOCOL *DevPath)
{
  CHAR16      *FilePath;
  CHAR16      *Char;
  CONST CHAR16      *Tail;

  FilePath = ConvertDevicePathToText(DevPath, TRUE, TRUE);
  // fix / into '\\'
  if (FilePath != NULL) {
    for (Char = FilePath; *Char != L'\0'; Char++) {
      if (*Char == L'/') {
        *Char = L'\\';
      }
    }
  }
  // "\\\\" into '\\'
  Char = (CHAR16*)StrStr(FilePath, L"\\\\"); // cast is ok because FilePath is not const, and we know that StrStr returns a pointer in FilePath. Will disappear when using a string object instead of CHAR16*
  while (Char != NULL) {
//    StrCpyS(Char, 4, Char + 1);  //can't overlap
    Tail = Char + 1;
    while (*Char != 0) {
      *(Char++) = *(Tail++);
    }
    Char = (CHAR16*)StrStr(FilePath, L"\\\\"); // cast is ok because FilePath is not const, and we know that StrStr returns a pointer in FilePath. Will disappear when using a string object instead of CHAR16*
  }
  XStringW returnValue;
  returnValue.stealValueFrom(FilePath); // do not FreePool FilePath, it's now owned by returnValue
  return returnValue;
}

XStringW FileDevicePathFileToXStringW(const EFI_DEVICE_PATH_PROTOCOL *DevPath)
{
  EFI_DEVICE_PATH_PROTOCOL *Node;

  if (DevPath == NULL) {
    return NullXStringW;
  }

  Node = (EFI_DEVICE_PATH_PROTOCOL *)DevPath;
  while (!IsDevicePathEnd(Node)) {
    if ((Node->Type == MEDIA_DEVICE_PATH) &&
        (Node->SubType == MEDIA_FILEPATH_DP)) {
      return FileDevicePathToXStringW(Node);
    }
    Node = NextDevicePathNode(Node);
  }
  return NullXStringW;
}


/* EOF */
