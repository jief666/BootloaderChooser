/** @file
  Provides Mach-O parsing helper functions.

Copyright (c) 2016 - 2018, Download-Fritz.  All rights reserved.<BR>
This program and the accompanying materials are licensed and made available
under the terms and conditions of the BSD License which accompanies this
distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef OC_MACHO_LIB_H_
#define OC_MACHO_LIB_H_

#include <IndustryStandard/AppleMachoImage.h>

///
/// Default assumed page size.
///
#define MACHO_PAGE_SIZE 4096U

///
/// Macro to align by default assumed page size.
///
#define MACHO_ALIGN(x) ALIGN_VALUE((x), MACHO_PAGE_SIZE)

///
/// Context used to refer to a Mach-O.  This struct is exposed for reference
/// only.  Members are not guaranteed to be sane.
///
typedef struct {
  MACH_HEADER_64        *MachHeader;
  UINT32                FileSize;
  UINT32                ContainerOffset;
  MACH_SYMTAB_COMMAND   *Symtab;
  MACH_NLIST_64         *SymbolTable;
  CHAR8                 *StringTable;
  MACH_DYSYMTAB_COMMAND *DySymtab;
  MACH_NLIST_64         *IndirectSymbolTable;
  MACH_RELOCATION_INFO  *LocalRelocations;
  MACH_RELOCATION_INFO  *ExternRelocations;
} OC_MACHO_CONTEXT;

/**
  Initializes a Mach-O Context.

  @param[out] Context          Mach-O Context to initialize.
  @param[in]  FileData         Pointer to the file's expected Mach-O header.
  @param[in]  FileSize         File size of FileData.
  @param[in]  ContainerOffset  The amount of Bytes the Mach-O header is offset
                               from the base (container, e.g. KC) of the file.

  @return  Whether Context has been initialized successfully.

**/
BOOLEAN
MachoInitializeContext (
  OUT OC_MACHO_CONTEXT  *Context,
  IN  VOID              *FileData,
  IN  UINT32            FileSize,
  IN  UINT32            ContainerOffset
  );

/**
  Returns the Mach-O Header structure.

  @param[in,out] Context  Context of the Mach-O.

**/
MACH_HEADER_64 *
MachoGetMachHeader64 (
  IN OUT OC_MACHO_CONTEXT  *Context
  );

/**
  Returns the Mach-O's file size.

  @param[in,out] Context  Context of the Mach-O.

**/
UINT32
MachoGetFileSize (
  IN OUT OC_MACHO_CONTEXT  *Context
  );

/**
  Returns the Mach-O's virtual address space size.

  @param[out] Context   Context of the Mach-O.

**/
UINT32
MachoGetVmSize64 (
  IN OUT OC_MACHO_CONTEXT  *Context
  );

/**
  Returns the last virtual address of a Mach-O.

  @param[in] Context  Context of the Mach-O.

  @retval 0  The binary is malformed.

**/
UINT64
MachoGetLastAddress64 (
  IN OUT OC_MACHO_CONTEXT  *Context
  );

/**
  Retrieves the first UUID Load Command.

  @param[in,out] Context  Context of the Mach-O.

  @retval NULL  NULL is returned on failure.

**/
MACH_UUID_COMMAND *
MachoGetUuid64 (
  IN OUT OC_MACHO_CONTEXT  *Context
  );

/**
  Retrieves the first segment by the name of SegmentName.

  @param[in,out] Context      Context of the Mach-O.
  @param[in]     SegmentName  Segment name to search for.

  @retval NULL  NULL is returned on failure.

**/
MACH_SEGMENT_COMMAND_64 *
MachoGetSegmentByName64 (
  IN OUT OC_MACHO_CONTEXT  *Context,
  IN     CONST CHAR8       *SegmentName
  );

/**
  Retrieves the first section by the name of SectionName.

  @param[in,out] Context      Context of the Mach-O.
  @param[in]     Segment      Segment to search in.
  @param[in]     SectionName  Section name to search for.

  @retval NULL  NULL is returned on failure.

**/
MACH_SECTION_64 *
MachoGetSectionByName64 (
  IN OUT OC_MACHO_CONTEXT         *Context,
  IN     MACH_SEGMENT_COMMAND_64  *Segment,
  IN     CONST CHAR8              *SectionName
  );

/**
  Retrieves a section within a segment by the name of SegmentName.

  @param[in,out] Context      Context of the Mach-O.
  @param[in]     SegmentName  The name of the segment to search in.
  @param[in]     SectionName  The name of the section to search for.

  @retval NULL  NULL is returned on failure.

**/
MACH_SECTION_64 *
MachoGetSegmentSectionByName64 (
  IN OUT OC_MACHO_CONTEXT  *Context,
  IN     CONST CHAR8       *SegmentName,
  IN     CONST CHAR8       *SectionName
  );

/**
  Retrieves the next segment.

  @param[in,out] Context  Context of the Mach-O.
  @param[in]     Segment  Segment to retrieve the successor of.
                          if NULL, the first segment is returned.

  @retal NULL  NULL is returned on failure.

**/
MACH_SEGMENT_COMMAND_64 *
MachoGetNextSegment64 (
  IN OUT OC_MACHO_CONTEXT               *Context,
  IN     CONST MACH_SEGMENT_COMMAND_64  *Segment  OPTIONAL
  );

/**
  Retrieves the next section of a segment.

  @param[in,out] Context  Context of the Mach-O.
  @param[in]     Segment  The segment to get the section of.
  @param[in]     Section  The section to get the successor of.

  @retval NULL  NULL is returned on failure.

**/
MACH_SECTION_64 *
MachoGetNextSection64 (
  IN OUT OC_MACHO_CONTEXT         *Context,
  IN     MACH_SEGMENT_COMMAND_64  *Segment,
  IN     MACH_SECTION_64          *Section  OPTIONAL
  );

/**
  Retrieves a section by its index.

  @param[in,out] Context  Context of the Mach-O.
  @param[in]     Index    Index of the section to retrieve.

  @retval NULL  NULL is returned on failure.

**/
MACH_SECTION_64 *
MachoGetSectionByIndex64 (
  IN OUT OC_MACHO_CONTEXT  *Context,
  IN     UINT32            Index
  );

/**
  Retrieves a section by its address.

  @param[in,out] Context  Context of the Mach-O.
  @param[in]     Address  Address of the section to retrieve.

  @retval NULL  NULL is returned on failure.

**/
MACH_SECTION_64 *
MachoGetSectionByAddress64 (
  IN OUT OC_MACHO_CONTEXT  *Context,
  IN     UINT64            Address
  );

/**
  Merge Mach-O segments into one with lowest protection.

  @param[in,out] Context  Context of the Mach-O.
  @param[in]     Prefix   Segment prefix to merge.

  @retval TRUE on success

**/
BOOLEAN
MachoMergeSegments64 (
  IN OUT OC_MACHO_CONTEXT     *Context,
  IN     CONST CHAR8          *Prefix
  );

/**
  Returns whether Symbol describes a section.

  @param[in] Symbol  Symbol to evaluate.

**/
BOOLEAN
MachoSymbolIsSection (
  IN CONST MACH_NLIST_64  *Symbol
  );

/**
  Returns whether Symbol is defined.

  @param[in] Symbol  Symbol to evaluate.

**/
BOOLEAN
MachoSymbolIsDefined (
  IN CONST MACH_NLIST_64  *Symbol
  );

/**
  Returns whether Symbol is defined locally.

  @param[in,out] Context  Context of the Mach-O.
  @param[in]     Symbol   Symbol to evaluate.

**/
BOOLEAN
MachoSymbolIsLocalDefined (
  IN OUT OC_MACHO_CONTEXT     *Context,
  IN     CONST MACH_NLIST_64  *Symbol
  );

/**
  Retrieves a locally defined symbol by its name.

  @param[in] Context  Context of the Mach-O.
  @param[in] Name     Name of the symbol to locate.

**/
MACH_NLIST_64 *
MachoGetLocalDefinedSymbolByName (
  IN OUT OC_MACHO_CONTEXT  *Context,
  IN     CONST CHAR8       *Name
  );

/**
  Retrieves a symbol by its index.

  @param[in] Context  Context of the Mach-O.
  @param[in] Index    Index of the symbol to locate.

  @retval NULL  NULL is returned on failure.

**/
MACH_NLIST_64 *
MachoGetSymbolByIndex64 (
  IN OUT OC_MACHO_CONTEXT  *Context,
  IN     UINT32            Index
  );

/**
  Retrieves Symbol's name.

  @param[in,out] Context  Context of the Mach-O.
  @param[in]     Symbol   Symbol to retrieve the name of.

  @retval symbol name.

**/
CONST CHAR8 *
MachoGetSymbolName64 (
  IN OUT OC_MACHO_CONTEXT     *Context,
  IN     CONST MACH_NLIST_64  *Symbol
  );

/**
  Retrieves Symbol's name.

  @param[in,out] Context  Context of the Mach-O.
  @param[in]     Symbol   Indirect symbol to retrieve the name of.

  @retval NULL  NULL is returned on failure.

**/
CONST CHAR8 *
MachoGetIndirectSymbolName64 (
  IN OUT OC_MACHO_CONTEXT     *Context,
  IN     CONST MACH_NLIST_64  *Symbol
  );

/**
  Returns whether the symbol's value is a valid address within the Mach-O
  referenced to by Context.

  @param[in,out] Context  Context of the Mach-O.
  @param[in]     Symbol   Symbol to verify the value of.

**/
BOOLEAN
MachoIsSymbolValueInRange64 (
  IN OUT OC_MACHO_CONTEXT     *Context,
  IN     CONST MACH_NLIST_64  *Symbol
  );

/**
  Retrieves the symbol referenced by the Relocation targeting Address.

  @param[in,out] Context  Context of the Mach-O.
  @param[in]     Address  Address to search for.
  @param[out]    Symbol   Buffer to output the symbol referenced by the
                          Relocation into.  The output is undefined when FALSE
                          is returned.  May be NULL.

  @returns  Whether the Relocation exists.

**/
BOOLEAN
MachoGetSymbolByRelocationOffset64 (
  IN OUT OC_MACHO_CONTEXT  *Context,
  IN     UINT64            Address,
  OUT    MACH_NLIST_64     **Symbol
  );

/**
  Retrieves the symbol referenced by the extern Relocation targeting Address.

  @param[in,out] Context  Context of the Mach-O.
  @param[in]     Address  Address to search for.
  @param[out]    Symbol   Buffer to output the symbol referenced by the
                          Relocation into.  The output is undefined when FALSE
                          is returned.  May be NULL.

  @returns  Whether the Relocation exists.

**/
BOOLEAN
MachoGetSymbolByExternRelocationOffset64 (
  IN OUT OC_MACHO_CONTEXT  *Context,
  IN     UINT64            Address,
  OUT    MACH_NLIST_64     **Symbol
  );

/**
  Relocate Symbol to be against LinkAddress.

  @param[in,out] Context      Context of the Mach-O.
  @param[in]     LinkAddress  The address to be linked against.
  @param[in,out] Symbol       The symbol to be relocated.

  @returns  Whether the operation has been completed successfully.

**/
BOOLEAN
MachoRelocateSymbol64 (
  IN OUT OC_MACHO_CONTEXT  *Context,
  IN     UINT64            LinkAddress,
  IN OUT MACH_NLIST_64     *Symbol
  );

/**
  Retrieves the Mach-O file offset of the address pointed to by Symbol.

  @param[in,out] Context     Context of the Mach-O.
  @param[in]     Symbol      Symbol to retrieve the offset of.
  @param[out]    FileOffset  Pointer the file offset is returned into.
                             If FALSE is returned, the output is undefined.
  @param[out]    MaxSize     Maximum data safely available from FileOffset.

  @retval 0  0 is returned on failure.

**/
BOOLEAN
MachoSymbolGetFileOffset64 (
  IN OUT OC_MACHO_CONTEXT      *Context,
  IN     CONST  MACH_NLIST_64  *Symbol,
  OUT    UINT32                *FileOffset,
  OUT    UINT32                *MaxSize OPTIONAL
  );

/**
  Returns whether Name is pure virtual.

  @param[in] Name  The name to evaluate.

**/
BOOLEAN
MachoSymbolNameIsPureVirtual (
  IN CONST CHAR8  *Name
  );

/**
  Returns whether Name is a Padslot.

  @param[in] Name  The name to evaluate.

**/
BOOLEAN
MachoSymbolNameIsPadslot (
  IN CONST CHAR8  *Name
  );

/**
  Returns whether SymbolName defines a Super Metaclass Pointer.

  @param[in,out] Context     Context of the Mach-O.
  @param[in]     SymbolName  The symbol name to check.

**/
BOOLEAN
MachoSymbolNameIsSmcp64 (
  IN OUT OC_MACHO_CONTEXT  *Context,
  IN     CONST CHAR8       *SymbolName
  );

/**
  Returns whether SymbolName defines a Super Metaclass Pointer.

  @param[in,out] Context     Context of the Mach-O.
  @param[in]     SymbolName  The symbol name to check.

**/
BOOLEAN
MachoSymbolNameIsMetaclassPointer64 (
  IN OUT OC_MACHO_CONTEXT  *Context,
  IN     CONST CHAR8       *SymbolName
  );

/**
  Retrieves the class name of a Super Meta Class Pointer.

  @param[in,out] Context        Context of the Mach-O.
  @param[in]     SmcpName       SMCP Symbol name to get the class name of.
  @param[in]     ClassNameSize  The size of ClassName.
  @param[out]    ClassName      The output buffer for the class name.

  @returns  Whether the name has been retrieved successfully.

**/
BOOLEAN
MachoGetClassNameFromSuperMetaClassPointer (
  IN OUT OC_MACHO_CONTEXT     *Context,
  IN     CONST CHAR8          *SmcpName,
  IN     UINTN                ClassNameSize,
  OUT    CHAR8                *ClassName
  );

/**
  Retrieves the class name of a VTable.

  @param[out] VtableName  The VTable's name.

**/
CONST CHAR8 *
MachoGetClassNameFromVtableName (
  IN  CONST CHAR8  *VtableName
  );

/**
  Retrieves the function prefix of a class name.

  @param[in]  ClassName           The class name to evaluate.
  @param[in]  FunctionPrefixSize  The size of FunctionPrefix.
  @param[out] FunctionPrefix      The output buffer for the function prefix.

  @returns  Whether the name has been retrieved successfully.

**/
BOOLEAN
MachoGetFunctionPrefixFromClassName (
  IN  CONST CHAR8  *ClassName,
  IN  UINTN        FunctionPrefixSize,
  OUT CHAR8        *FunctionPrefix
  );

/**
  Retrieves the class name of a Meta Class Pointer.

  @param[in,out] Context             Context of the Mach-O.
  @param[in]     MetaClassName       MCP Symbol name to get the class name of.
  @param[in]     ClassNameSize       The size of ClassName.
  @param[out]    ClassName           The output buffer for the class name.

  @returns  Whether the name has been retrieved successfully.

**/
BOOLEAN
MachoGetClassNameFromMetaClassPointer (
  IN OUT OC_MACHO_CONTEXT     *Context,
  IN     CONST CHAR8          *MetaClassName,
  IN     UINTN                ClassNameSize,
  OUT    CHAR8                *ClassName
  );

/**
  Retrieves the VTable name of a class name.

  @param[in]  ClassName       Class name to get the VTable name of.
  @param[in]  VtableNameSize  The size of VtableName.
  @param[out] VtableName      The output buffer for the VTable name.

  @returns  Whether the name has been retrieved successfully.

**/
BOOLEAN
MachoGetVtableNameFromClassName (
  IN  CONST CHAR8  *ClassName,
  IN  UINTN        VtableNameSize,
  OUT CHAR8        *VtableName
  );

/**
  Retrieves the Meta VTable name of a class name.

  @param[in]  ClassName       Class name to get the Meta VTable name of.
  @param[in]  VtableNameSize  The size of VtableName.
  @param[out] VtableName      The output buffer for the VTable name.

  @returns  Whether the name has been retrieved successfully.

**/
BOOLEAN
MachoGetMetaVtableNameFromClassName (
  IN  CONST CHAR8  *ClassName,
  IN  UINTN        VtableNameSize,
  OUT CHAR8        *VtableName
  );

/**
  Retrieves the final symbol name of a class name.

  @param[in]  ClassName            Class name to get the final symbol name of.
  @param[in]  FinalSymbolNameSize  The size of FinalSymbolName.
  @param[out] FinalSymbolName      The output buffer for the final symbol name.

  @returns  Whether the name has been retrieved successfully.

**/
BOOLEAN
MachoGetFinalSymbolNameFromClassName (
  IN  CONST CHAR8  *ClassName,
  IN  UINTN        FinalSymbolNameSize,
  OUT CHAR8        *FinalSymbolName
  );

/**
  Returns whether SymbolName defines a VTable.

  @param[in] SymbolName  The symbol name to check.

**/
BOOLEAN
MachoSymbolNameIsVtable64 (
  IN CONST CHAR8  *SymbolName
  );

/**
  Returns whether the symbol name describes a C++ symbol.

  @param[in] Name  The name to evaluate.

**/
BOOLEAN
MachoSymbolNameIsCxx (
  IN CONST CHAR8  *Name
  );

/**
  Retrieves Metaclass symbol of a SMCP.

  @param[in,out] Context  Context of the Mach-O.
  @param[in]     Smcp     The SMCP to evaluate.

  @retval NULL  NULL is returned on failure.

**/
MACH_NLIST_64 *
MachoGetMetaclassSymbolFromSmcpSymbol64 (
  IN OUT OC_MACHO_CONTEXT     *Context,
  IN     CONST MACH_NLIST_64  *Smcp
  );

/**
  Retrieves VTable and Meta VTable of a SMCP.
  Logically matches XNU's get_vtable_syms_from_smcp.

  @param[in,out] Context      Context of the Mach-O.
  @param[in]     SmcpName     SMCP Symbol mame to retrieve the VTables from.
  @param[out]    Vtable       Output buffer for the VTable symbol pointer.
  @param[out]    MetaVtable   Output buffer for the Meta VTable symbol pointer.

**/
BOOLEAN
MachoGetVtableSymbolsFromSmcp64 (
  IN OUT OC_MACHO_CONTEXT     *Context,
  IN     CONST CHAR8          *SmcpName,
  OUT    CONST MACH_NLIST_64  **Vtable,
  OUT    CONST MACH_NLIST_64  **MetaVtable
  );

/**
  Returns whether the Relocation's type indicates a Pair for the Intel 64
  platform.
  
  @param[in] Type  The Relocation's type to verify.

**/
BOOLEAN
MachoRelocationIsPairIntel64 (
  IN UINT8  Type
  );

/**
  Returns whether the Relocation's type matches a Pair's for the Intel 64
  platform.
  
  @param[in] Type  The Relocation's type to verify.

**/
BOOLEAN
MachoIsRelocationPairTypeIntel64 (
  IN UINT8  Type
  );

/**
  Returns whether the Relocation shall be preserved for the Intel 64 platform.
  
  @param[in] Type  The Relocation's type to verify.

**/
BOOLEAN
MachoPreserveRelocationIntel64 (
  IN UINT8  Type
  );

/*
  Initialises Context with the symbol tables of SymsContext.

  @param[in,out] Context      The context to initialise the symbol info of.
  @param[in]     SymsContext  The context to retrieve the symbol tables from.

  @returns  Whether the operation was successful.
*/
BOOLEAN
MachoInitialiseSymtabsExternal64 (
  IN OUT OC_MACHO_CONTEXT  *Context,
  IN     OC_MACHO_CONTEXT  *SymsContext
  );

/**
  Obtain symbol tables.

  @param[in]     Context              Context of the Mach-O.
  @param[out]    SymbolTable          Symbol table.
  @param[out]    StringTable          String table for that symbol table.
  @param[out]    LocalSymbols         Local symbol table.
  @param[out]    NumLocalSymbols      Number of symbols in local symbol table.
  @param[out]    ExternalSymbols      External symbol table.
  @param[out]    NumExternalSymbols   Number of symbols in external symbol table.
  @param[out]    UndefinedSymbols     Undefined symbol table.
  @param[out]    NumUndefinedSymbols  Number of symbols in undefined symbol table.

  @return number of symbols in symbol table or 0.
**/
UINT32
MachoGetSymbolTable (
  IN OUT OC_MACHO_CONTEXT     *Context,
     OUT CONST MACH_NLIST_64  **SymbolTable,
     OUT CONST CHAR8          **StringTable OPTIONAL,
     OUT CONST MACH_NLIST_64  **LocalSymbols OPTIONAL,
     OUT UINT32               *NumLocalSymbols OPTIONAL,
     OUT CONST MACH_NLIST_64  **ExternalSymbols OPTIONAL,
     OUT UINT32               *NumExternalSymbols OPTIONAL,
     OUT CONST MACH_NLIST_64  **UndefinedSymbols OPTIONAL,
     OUT UINT32               *NumUndefinedSymbols OPTIONAL
  );

/**
  Obtain indirect symbol table.

  @param[in]     Context              Context of the Mach-O.
  @param[in,out] SymbolTable          Indirect symbol table.

  @return number of symbols in indirect symbol table or 0.
**/
UINT32
MachoGetIndirectSymbolTable (
  IN OUT OC_MACHO_CONTEXT     *Context,
     OUT CONST MACH_NLIST_64  **SymbolTable
  );

/**
  Returns a pointer to the Mach-O file at the specified virtual address.

  @param[in,out] Context  Context of the Mach-O.
  @param[in]     Address  Virtual address to look up.    
  @param[out]    MaxSize  Maximum data safely available from FileOffset.
                          If NULL is returned, the output is undefined.

**/
VOID *
MachoGetFilePointerByAddress64 (
  IN OUT OC_MACHO_CONTEXT  *Context,
  IN     UINT64            Address,
  OUT    UINT32            *MaxSize OPTIONAL
  );

/**
  Expand Mach-O image to Destination (make segment file sizes equal to vm sizes).

  @param[in]  Context          Context of the Mach-O.
  @param[out] Destination      Output buffer.
  @param[in]  DestinationSize  Output buffer maximum size.
  @param[in]  Strip            Output with stripped prelink commands.

  @returns  New image size or 0 on failure.

**/
UINT32
MachoExpandImage64 (
  IN  OC_MACHO_CONTEXT   *Context,
  OUT UINT8              *Destination,
  IN  UINT32             DestinationSize,
  IN  BOOLEAN            Strip
  );

/**
  Find Mach-O entry point from LC_UNIXTHREAD loader command.
  This command does not verify Mach-O and assumes it is valid.

  @param[in]  Image  Loaded Mach-O image.

  @returns  Entry point or 0.
**/
UINT64
MachoRuntimeGetEntryAddress (
  IN VOID  *Image
  );

/**
  Retrieves the next Load Command of type LoadCommandType.

  @param[in,out] Context          Context of the Mach-O.
  @param[in]     LoadCommandType  Type of the Load Command to retrieve.
  @param[in]     LoadCommand      Previous Load Command.
                                  If NULL, the first match is returned.

  @retval NULL  NULL is returned on failure.
**/
MACH_LOAD_COMMAND *
MachoGetNextCommand64 (
  IN OUT OC_MACHO_CONTEXT         *Context,
  IN     MACH_LOAD_COMMAND_TYPE   LoadCommandType,
  IN     CONST MACH_LOAD_COMMAND  *LoadCommand  OPTIONAL
  );

#endif // OC_MACHO_LIB_H
