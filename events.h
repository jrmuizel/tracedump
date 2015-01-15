
#include <Windows.h>

DEFINE_GUID(kDXCreateDestAllocation,
    0xe2e07a20, 0xdb80, 0x476b, 0xb7, 0x96, 0xe3, 0x8, 0x14, 0xbe, 0x10, 0x26);
enum {
    kDxCreateAllocationStart = 1,
    kDxDestAllocationEnd = 2,
    kDxCreateAllocationDCStart = 3,
};

struct DxCreateDestAlloc
{
    ULONG PID; // close has 28, 03, 00...., open has 50 10 00..., close has 58 03
    ULONG unknown;
    ULONGLONG Device;
    ULONGLONG Adapter;
    ULONG uk2; // cpuVisible=1, cpuVisible | Swizzled = 81, cpuVisible | cached = 5, Protected FromEndOfSegment LinkMirrored = 8001048, Shared = 28000000
    ULONG ukSize;
    ULONG uk3; // this could be the high part of size
    ULONG ukAlignment;
    ULONG ukSupportedReadSegment;
    ULONG ukSupportedWriteSegment;
    ULONG ukPreferedSegment;
    ULONG uk5b; // 
    ULONG uk6a;
    ULONG Priority;
    ULONGLONG AdapterAllocation; // VidMmGlobalAlloc
    ULONGLONG DxgAdapterAlloc;

    ULONGLONG uk8; // zeros

    ULONG uk9;
    ULONG UsageFlags; // Vertex = 0x20

    ULONG Format;
    ULONG SwizzledFormat;

    ULONG ukByteOffset;
    ULONG Width;

    ULONG Height;
    ULONG Pitch;

    ULONG Depth;
    ULONG SlicePitch;
};


DEFINE_GUID(kDXOpenCloseAllocation,
    0x9c94956d, 0xb0fb, 0x4ef3, 0x94, 0x82, 0x96, 0x6b, 0xb4, 0xc0, 0x6b, 0x0);
enum {
    kDxAllocationOpen = 1,
    kDxAllocationClose = 2,
};

struct DxAlloc
{
    ULONG PID; // close has 28, 03, 00...., open has 50 10 00..., close has 58 03
    ULONG unknown;
    ULONGLONG Device;
    ULONGLONG unknown2;
    ULONGLONG GlobalAlloc;
    ULONGLONG Alloc;
    ULONGLONG DXGRESOURCE;
    ULONGLONG DXGSHAREDRESOURCE;
    ULONG ThunkResource;
    ULONG ThunkAllocation;
    ULONGLONG RuntimeAPIPointer;
};


DEFINE_GUID(kDirect3D11Texture2D,
    0xdb6f6ddb, 0xac77, 0x4e88, 0x82, 0x53, 0x81, 0x9d, 0xf9, 0xbb, 0xf1, 0x40);
enum {
    kDirect3D11Texture2DRotateTexture2DIdentities = 0,
    kDirect3D11Texture2DCreateTexture = 1,
    kDirect3D11Texture2DDestroyTexture = 2,
    kDirect3D11Texture2DReportTexture = 3,
};

struct Direct3D11Texture2D
{
    ULONG pID3D11Resource;
    ULONG pIDXGISurface;
    ULONG pID3D11Device;
    ULONG Dimension;
    ULONG Usage;
    ULONG Width;
    ULONG Height;
    ULONG Depth;
    ULONG MipLevels;
    ULONG ArraySize;
    ULONG Format;
    ULONG SampleCount;
    ULONG SampleQuality;
    ULONG BindFlags;
    ULONG CPUAccessFlags;
    ULONG MiscFlags;
    ULONG StructureByteStride;
    ULONG hKMResource;
    ULONG hUMResource;
    /* I don't understand the length interaction with these fields
    ULONG unk;
    ULONG UMResourceMiscFlags;
    */
};

struct Direct3D11Texture2D64
{
    ULONGLONG pID3D11Resource;
    ULONGLONG pIDXGISurface;
    ULONGLONG pID3D11Device;
    ULONG Dimension;
    ULONG Usage;
    ULONG Width;
    ULONG Height;
    ULONG Depth;
    ULONG MipLevels;
    ULONG ArraySize;
    ULONG Format;
    ULONG SampleCount;
    ULONG SampleQuality;
    ULONG BindFlags;
    ULONG CPUAccessFlags;
    ULONG MiscFlags;
    ULONG StructureByteStride;
    ULONG hKMResource;
    ULONG hUMResource;
    /* I don't understand the length interaction with these fields
    ULONG unk;
    ULONG UMResourceMiscFlags;
    */
};

