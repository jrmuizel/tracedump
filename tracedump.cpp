// tracedump.cpp : Defines the entry point for the console application.

#define INITGUID
#include <Windows.h>
#include <evntrace.h>
#include <evntcons.h>
#include <stdio.h>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include "events.h"

using namespace std;

VOID WINAPI ProcessEvent(PEVENT_RECORD pEvent);

void hexdump(unsigned char *data, int length)
{
    for (int i = 0; i < length; i++) {
        printf("%02x ", data[i]);
        if (i % 8 == 7)
            printf("\n");

    }
}

static map<ULONGLONG, pair<__int64, DxCreateDestAlloc>> live;
static map<ULONGLONG, __int64> liveTextures;

inline std::wstring convert(const std::string& as)
{
    // deal with trivial case of empty string
    if (as.empty())    return std::wstring();

    // determine required length of new string
    size_t reqLength = ::MultiByteToWideChar(CP_UTF8, 0, as.c_str(), (int)as.length(), 0, 0);

    // construct new string of required length
    std::wstring ret(reqLength, L'\0');

    // convert old string to new string
    ::MultiByteToWideChar(CP_UTF8, 0, as.c_str(), (int)as.length(), &ret[0], (int)ret.length());

    // return new string ( compiler should optimize this away )
    return ret;
}

int main(int argc, char* argv[])
{
    EVENT_TRACE_LOGFILE Logfile;
    ZeroMemory(&Logfile, sizeof(Logfile));
    wstring fileName = L"C:\\Users\\joe\\Downloads\\Merged-dmajor\\Merged.etl";
    if (argc > 1) {
        fileName = convert(string(argv[1]));
    }
    Logfile.LogFileName = (wchar_t*)fileName.c_str();
    //Logfile.LogFileName = L"C:\\Program Files (x86)\\Windows Kits\\8.1\\Windows Performance Toolkit\\gpuview\\traces\\Chrome-aqua.etl";
    Logfile.EventRecordCallback = ProcessEvent;
    Logfile.ProcessTraceMode = PROCESS_TRACE_MODE_EVENT_RECORD;
    TRACEHANDLE trace = OpenTrace(&Logfile);

    ProcessTrace(&trace, 1, nullptr, nullptr);
    {
        vector<__int64> remainingAllocs;
        for (const auto &alloc : liveTextures) {
            remainingAllocs.push_back(alloc.second);
        }

        sort(remainingAllocs.begin(), remainingAllocs.end());
        /*, [](const pair<__int64, DxCreateDestAlloc>& a, const pair<__int64, DxCreateDestAlloc>& b)
        {
        return a.first < b.first;
        });*/

        for (const auto &alloc : remainingAllocs) {
            printf("%lld\n", alloc);
        }
    }

    vector<pair<__int64, DxCreateDestAlloc>> remainingAllocs;
    for (auto alloc : live) {
        //printf("%llx %lldd %d\n", alloc.first, alloc.second.first, alloc.second.second);
        remainingAllocs.push_back(alloc.second);
    }

    sort(remainingAllocs.begin(), remainingAllocs.end(), [](const pair<__int64, DxCreateDestAlloc>& a, const pair<__int64, DxCreateDestAlloc>& b)
    {
        return a.first < b.first;
    });

    for (const auto &alloc : remainingAllocs) {
        const DxCreateDestAlloc &info = alloc.second;
        printf("%lld %d %d %x\n", alloc.first,
            info.Format,
            info.ukSize,
            info.uk2,
            info.uk3
            );
    }

	return 0;
}


VOID WINAPI ProcessEvent(PEVENT_RECORD pEvent)
{
    static bool firstEvent = true;
    static __int64 firstTime;
    if (firstEvent) {
        firstEvent = false;
        firstTime = pEvent->EventHeader.TimeStamp.QuadPart;
    }

    __int64 offsetTime = pEvent->EventHeader.TimeStamp.QuadPart - firstTime;




    bool bits32 = pEvent->EventHeader.Flags & EVENT_HEADER_FLAG_32_BIT_HEADER;

    if (IsEqualGUID(pEvent->EventHeader.ProviderId, kDirect3D11Texture2D)) {


        int op = pEvent->EventHeader.EventDescriptor.Opcode;
        int taskId = pEvent->EventHeader.EventDescriptor.Task;
        int Id = pEvent->EventHeader.EventDescriptor.Id;

        static LONGLONG total;
        int size;
       
        if (taskId == 6 && op != 0) {
            /*printf("Event size:%x 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x} (id %x op %x Time %lld, UserDataLength: %x)\n", pEvent->EventHeader.Size,
                pEvent->EventHeader.ProviderId.Data1,
                pEvent->EventHeader.ProviderId.Data2,
                pEvent->EventHeader.ProviderId.Data3,
                pEvent->EventHeader.ProviderId.Data4[0],
                pEvent->EventHeader.ProviderId.Data4[1],
                pEvent->EventHeader.ProviderId.Data4[2],
                pEvent->EventHeader.ProviderId.Data4[3],
                pEvent->EventHeader.ProviderId.Data4[4],
                pEvent->EventHeader.ProviderId.Data4[5],
                pEvent->EventHeader.ProviderId.Data4[6],
                pEvent->EventHeader.ProviderId.Data4[7],
                pEvent->EventHeader.EventDescriptor.Task,
                pEvent->EventHeader.EventDescriptor.Opcode,
                pEvent->EventHeader.TimeStamp.QuadPart - firstTime,
                pEvent->UserDataLength);*/
            //printf("")
            if (bits32) {
                Direct3D11Texture2D *alloc = (Direct3D11Texture2D*)pEvent->UserData;
                if (op == kDirect3D11Texture2DDestroyTexture) {
                    size = -(int)(alloc->Width * alloc->Height);
                    liveTextures.erase(alloc->pID3D11Resource);
                }
                else {
                    size = (alloc->Width * alloc->Height);
                    liveTextures.insert(make_pair(alloc->pID3D11Resource, offsetTime));

                }
                total += size;
                printf("%lld %lld %d %d %d %d\n", offsetTime, total, size, alloc->Width, alloc->Height, alloc->Format);

            }
            else {
                Direct3D11Texture2D64 *alloc = (Direct3D11Texture2D64*)pEvent->UserData;
                if (op == kDirect3D11Texture2DDestroyTexture) {
                    size = -(int)(alloc->Width * alloc->Height);
                    //live.erase(alloc->AdapterAllocation);
                }
                else {
                    size = (alloc->Width * alloc->Height);
                    //live.insert(make_pair(alloc->AdapterAllocation, make_pair(offsetTime, *alloc)));

                }
                total += size;
                printf("%lld %lld %d %d %d %d\n", offsetTime, total, size, alloc->Width, alloc->Height, alloc->Format);

            }
            //printf("")
            //if (alloc->Format)
            //hexdump((unsigned char*)pEvent->ExtendedData->DataPtr, pEvent->ExtendedData->DataSize);

        }
    }
    return;

    printf("32bit :%d\n", bits32);

    if (IsEqualGUID(pEvent->EventHeader.ProviderId, kDXCreateDestAllocation)) {
        int op = pEvent->EventHeader.EventDescriptor.Opcode;
        DxCreateDestAlloc *alloc = (DxCreateDestAlloc*)pEvent->UserData;
        static LONGLONG total;
        int size;

        if (op == kDxDestAllocationEnd) {
            size = -(int)alloc->ukSize;
            live.erase(alloc->AdapterAllocation);
        }
        else {
            size = alloc->ukSize;
            live.insert(make_pair(alloc->AdapterAllocation, make_pair(offsetTime, *alloc)));

        }
        total += size;
        //printf("")
        //if (alloc->Format)
        printf("%lld %lld %d %d\n", offsetTime, total, size, alloc->Format);

        //hexdump((unsigned char*)pEvent->UserData, pEvent->UserDataLength);
    }

    if (IsEqualGUID(pEvent->EventHeader.ProviderId, EventTraceGuid) &&
        pEvent->EventHeader.EventDescriptor.Opcode == EVENT_TRACE_TYPE_INFO)
    {
        printf("skip\n");
        ; // Skip this event.
    }
}