/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       dsutil.cpp
 *  Content:    Routines for dealing with sounds from resources
 *
 *
 ***************************************************************************/

#include "stdafx.h" // new on 291002 for mini-dumps

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
//#if TT_MICROSOFT
//#include <dsound.h>
// changed above to the following on 040500 since NT 4.0 otherwise can't play sounds
// note prior to 042500 the search paths found the following and not the mssdk version (DX 7.0 just now)
//#include "C:\Program Files\Microsoft Visual Studio\VC98\Include\dsound.h"
//#else
// Borland seems not to work otherwise - note dsound.h is a copy
#include <dsound.h> // was "..." prior to 231102
//#include "c:\tt\dsound.h" // experiment 231102
//#endif

//turns out the following is a feature made obsolete by DirectX 7.0 - code re-written to no longer use DSBCAPS_CTRLDEFAULT
// but it does again on 050500 to see if this is what caused NT 4.0 machines to lose sound
// using VC 6.0 dsound.h files rather than DX7 to see if this helps with Sound problem
//#include "c:\mssdk\include\dsound.h"
// for some reason this is missing from DX 7.0 SDK but was in earlier versions
// but for now I don't use pan or frequency controls so volume is good enough
//#define DSBCAPS_CTRLDEFAULT         DSBCAPS_CTRLVOLUME

#include "dsutil.h"
#include "wave.h"

static const char c_szWAV[] = "WAV";

///////////////////////////////////////////////////////////////////////////////
//
// DSLoadSoundBuffer
//
///////////////////////////////////////////////////////////////////////////////

IDirectSoundBuffer *DSLoadSoundBuffer(IDirectSound *pDS, LPCTSTR lpName,
                                      DWORD *wave_size)
{
    IDirectSoundBuffer *pDSB = NULL;
    DSBUFFERDESC dsBD = {0};
    BYTE *pbWaveData;

    // hmodule wasn't an arg and was NULL
    if (DSGetWaveResource(NULL, lpName, &dsBD.lpwfxFormat, &pbWaveData, &dsBD.dwBufferBytes))
    {
        dsBD.dwSize = sizeof(dsBD);
//        dsBD.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLDEFAULT;
		  // replaced the above with the following on 210400 since DirectSound documentation recommends it
		  // restored on 050500 since this change meant no sound on NT 4
		  // and switched back once again on 231102
		  // MS documentation says static is rarely used anymore and that you should only enable capabilities you need
		  dsBD.dwFlags = DSBCAPS_CTRLVOLUME; // DSBCAPS_STATIC; // experiment 231102 | ; //  DSBCAPS_CTRLFREQUENCY|DSBCAPS_CTRLPAN not used though might be some day
        *wave_size = dsBD.dwBufferBytes;

        if (SUCCEEDED(IDirectSound_CreateSoundBuffer(pDS, &dsBD, &pDSB, NULL)))
        {
            if (!DSFillSoundBuffer(pDSB, pbWaveData, dsBD.dwBufferBytes))
            {
                IDirectSoundBuffer_Release(pDSB);
                pDSB = NULL;
            }
        }
        else
        {
            pDSB = NULL;
        }
    }

    return pDSB;
}

// added by Ken Kahn based upon DSLoadSoundBuffer
IDirectSoundBuffer *DSLoadSoundFromMemory(IDirectSound *pDS, BYTE *bytes) {
  DWORD cbSize; // was UINT prior to 291002 - when converted to CPP
//  DWORD pcSamples;
  IDirectSoundBuffer *pDSB = NULL;
  DSBUFFERDESC dsBD = {0};
  BYTE *pbWaveData;
  if (DSParseWaveResource(bytes,&dsBD.lpwfxFormat,&pbWaveData, &cbSize)) {
     dsBD.dwSize = sizeof(dsBD);
//     dsBD.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLDEFAULT;
	  // replaced the above with the following on 210400 since DirectSound documentation recommends it
	  // restored on 050500 since this change meant no sound on NT 4
	  // and switched back once again on 231102
	  dsBD.dwFlags = DSBCAPS_CTRLVOLUME; // DSBCAPS_STATIC;  // experiment 231102 | ;
	  //  DSBCAPS_CTRLFREQUENCY|DSBCAPS_CTRLPAN not used though might be some day 
	  dsBD.dwBufferBytes = cbSize;
     if (SUCCEEDED(IDirectSound_CreateSoundBuffer(pDS, &dsBD, &pDSB, NULL))) {
        if (!DSFillSoundBuffer(pDSB, pbWaveData, dsBD.dwBufferBytes)) {
           IDirectSoundBuffer_Release(pDSB);
           pDSB = NULL;
           }
     } else {
        pDSB = NULL;
     }
  }
  return(pDSB);
};

// added by Ken Kahn based upon DSLoadSoundBuffer
IDirectSoundBuffer *DSLoadSoundFromFile(IDirectSound *pDS, char *file_name, HRESULT *result) {
  UINT cbSize;
  DWORD pcSamples;
  IDirectSoundBuffer *pDSB = NULL;
  DSBUFFERDESC dsBD = {0};
  BYTE *pbWaveData;
  if (WaveLoadFile(file_name,&cbSize,&pcSamples,&dsBD.lpwfxFormat,&pbWaveData) == 0) {
	  // commented out on 170401 since this broke all user sounds
//	  if (dsBD.dwBufferBytes == 0) return(NULL); 
	  // new on 110401 for better error handling since the debugging version of DSOUND doesn't like 0 long buffers
//	  memset((char *) &dsBD,0,sizeof(dsBD)); // added on 140699 just to be safe
     dsBD.dwSize = sizeof(dsBD);
//     dsBD.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLDEFAULT;
	  // replaced the above with the following on 210400 since DirectSound documentation recommends it
	  // restored on 050500 since this change meant no sound on NT 4
	  // and switched back once again on 231102
	  dsBD.dwFlags = DSBCAPS_CTRLVOLUME; // DSBCAPS_STATIC; // experiment 231102 | ; 
	  //  DSBCAPS_CTRLFREQUENCY|DSBCAPS_CTRLPAN not used though might be some day
     dsBD.dwBufferBytes = cbSize;
	  *result = IDirectSound_CreateSoundBuffer(pDS, &dsBD, &pDSB, NULL);
     if (SUCCEEDED(*result)) {
        if (!DSFillSoundBuffer(pDSB, pbWaveData, dsBD.dwBufferBytes)) {
           IDirectSoundBuffer_Release(pDSB);
           pDSB = NULL;
           }
     } else {
        pDSB = NULL;
     };
     // clean stuff created by WaveLoadFile
     GlobalFree(pbWaveData);
     GlobalFree(dsBD.lpwfxFormat);
  }
  return(pDSB);
};

///////////////////////////////////////////////////////////////////////////////
//
// DSReloadSoundBuffer
//
///////////////////////////////////////////////////////////////////////////////

BOOL DSReloadSoundBuffer(IDirectSoundBuffer *pDSB, LPCTSTR lpName)
{
    BOOL result=FALSE;
    BYTE *pbWaveData;
    DWORD cbWaveSize;

    if (DSGetWaveResource(NULL, lpName, NULL, &pbWaveData, &cbWaveSize))
    {
        if (SUCCEEDED(IDirectSoundBuffer_Restore(pDSB)) &&
            DSFillSoundBuffer(pDSB, pbWaveData, cbWaveSize))
        {
            result = TRUE;
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
//
// DSGetWaveResource
//
///////////////////////////////////////////////////////////////////////////////

BOOL DSGetWaveResource(HMODULE hModule, LPCTSTR lpName,
    WAVEFORMATEX **ppWaveHeader, BYTE **ppbWaveData, DWORD *pcbWaveSize)
{
    HRSRC hResInfo;
    HGLOBAL hResData;
    void *pvRes;

    if (((hResInfo = FindResource(hModule, lpName, c_szWAV)) != NULL) &&
        ((hResData = LoadResource(hModule, hResInfo)) != NULL) &&
        ((pvRes = LockResource(hResData)) != NULL) &&
        DSParseWaveResource(pvRes, ppWaveHeader, ppbWaveData, pcbWaveSize))
    {
        return TRUE;
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// SndObj fns
///////////////////////////////////////////////////////////////////////////////

SNDOBJ *SndObjCreate(IDirectSound *pDS, LPCTSTR lpName, int iConcurrent)
{
    SNDOBJ *pSO = NULL;
    LPWAVEFORMATEX pWaveHeader;
    BYTE *pbData;
    DWORD cbData; // was UINT prior to 291002

    if (DSGetWaveResource(NULL, lpName, &pWaveHeader, &pbData, &cbData))
    {
        if (iConcurrent < 1)
            iConcurrent = 1;

        if ((pSO = (SNDOBJ *)LocalAlloc(LPTR, sizeof(SNDOBJ) +
            (iConcurrent-1) * sizeof(IDirectSoundBuffer *))) != NULL)
        {
            int i;
            DWORD ignore_size;

            pSO->iAlloc = iConcurrent;
            pSO->pbWaveData = pbData;
            pSO->cbWaveSize = cbData;
            pSO->Buffers[0] = DSLoadSoundBuffer(pDS, lpName, &ignore_size);

            for (i=1; i<pSO->iAlloc; i++)
            {
                if (FAILED(IDirectSound_DuplicateSoundBuffer(pDS,
                    pSO->Buffers[0], &pSO->Buffers[i])))
                {
                    pSO->Buffers[i] = DSLoadSoundBuffer(pDS, lpName, &ignore_size);
                    if (!pSO->Buffers[i]) {
                        SndObjDestroy(pSO);
                        pSO = NULL;
                        break;
                    }
                }
            }
        }
    }

    return pSO;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SndObjDestroy(SNDOBJ *pSO)
{
    if (pSO)
    {
        int i;

        for (i=0; i<pSO->iAlloc; i++)
        {
            if (pSO->Buffers[i])
            {
                IDirectSoundBuffer_Release(pSO->Buffers[i]);
                pSO->Buffers[i] = NULL;
            }
        }
        LocalFree((HANDLE)pSO);
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

IDirectSoundBuffer *SndObjGetFreeBuffer(SNDOBJ *pSO)
{
    IDirectSoundBuffer *pDSB;

    if (pSO == NULL)
        return NULL;

    if (pDSB = pSO->Buffers[pSO->iCurrent])
    {
        HRESULT hres;
        DWORD dwStatus;

        hres = IDirectSoundBuffer_GetStatus(pDSB, &dwStatus);

        if (FAILED(hres))
            dwStatus = 0;

        if ((dwStatus & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING)
        {
            if (pSO->iAlloc > 1)
            {
                if (++pSO->iCurrent >= pSO->iAlloc)
                    pSO->iCurrent = 0;

                pDSB = pSO->Buffers[pSO->iCurrent];
                hres = IDirectSoundBuffer_GetStatus(pDSB, &dwStatus);

                if (SUCCEEDED(hres) && (dwStatus & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING)
                {
                    IDirectSoundBuffer_Stop(pDSB);
                    IDirectSoundBuffer_SetCurrentPosition(pDSB, 0);
                }
            }
            else
            {
                pDSB = NULL;
            }
        }

        if (pDSB && (dwStatus & DSBSTATUS_BUFFERLOST))
        {
            if (FAILED(IDirectSoundBuffer_Restore(pDSB)) ||
                !DSFillSoundBuffer(pDSB, pSO->pbWaveData, pSO->cbWaveSize))
            {
                pDSB = NULL;
            }
        }
    }

    return pDSB;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BOOL SndObjPlay(SNDOBJ *pSO, DWORD dwPlayFlags)
{
    BOOL result = FALSE;

    if (pSO == NULL)
        return FALSE;

    if ((!(dwPlayFlags & DSBPLAY_LOOPING) || (pSO->iAlloc == 1)))
    {
        IDirectSoundBuffer *pDSB = SndObjGetFreeBuffer(pSO);
        if (pDSB != NULL) {
            result = SUCCEEDED(IDirectSoundBuffer_Play(pDSB, 0, 0, dwPlayFlags));
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BOOL SndObjStop(SNDOBJ *pSO)
{
    int i;

    if (pSO == NULL)
        return FALSE;

    for (i=0; i<pSO->iAlloc; i++)
    {
        IDirectSoundBuffer_Stop(pSO->Buffers[i]);
        IDirectSoundBuffer_SetCurrentPosition(pSO->Buffers[i], 0);
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BOOL DSFillSoundBuffer(IDirectSoundBuffer *pDSB, BYTE *pbWaveData, DWORD cbWaveSize)
{
    if (pDSB && pbWaveData && cbWaveSize)
    {
        LPVOID pMem1, pMem2;
        DWORD dwSize1, dwSize2;

        HRESULT result = IDirectSoundBuffer_Lock(pDSB, 0, cbWaveSize, &pMem1, &dwSize1, &pMem2, &dwSize2, 0);
		  if (SUCCEEDED(result)) {
            CopyMemory(pMem1, pbWaveData, dwSize1);

            if ( 0 != dwSize2 )
                CopyMemory(pMem2, pbWaveData+dwSize1, dwSize2);

            IDirectSoundBuffer_Unlock(pDSB, pMem1, dwSize1, pMem2, dwSize2);
            return TRUE;
        } else {
/* needs declarations to compile - not worth it now
#if TT_DEBUG_ON
			  switch (result) {
				  case DSERR_BUFFERLOST:
					  tt_error_file() << "DSERR_BUFFERLOST";
					  break;
					case DSERR_INVALIDCALL:
					  tt_error_file() << "DSERR_INVALIDCALL";
					  break;
					case DSERR_INVALIDPARAM:
					  tt_error_file() << "DSERR_INVALIDPARAM";
					  break;
					case DSERR_PRIOLEVELNEEDED:
					  tt_error_file() << "DSERR_PRIOLEVELNEEDED";
					  break;
			  };
#endif
			  */
		  };
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BOOL DSParseWaveResource(void *pvRes, WAVEFORMATEX **ppWaveHeader, BYTE **ppbWaveData,DWORD *pcbWaveSize)
{
    DWORD *pdw;
    DWORD *pdwEnd;
    DWORD dwRiff;
    DWORD dwType;
    DWORD dwLength;

    if (ppWaveHeader)
        *ppWaveHeader = NULL;

    if (ppbWaveData)
        *ppbWaveData = NULL;

    if (pcbWaveSize)
        *pcbWaveSize = 0;

    pdw = (DWORD *)pvRes;
    dwRiff = *pdw++;
    dwLength = *pdw++;
    dwType = *pdw++;

    if (dwRiff != mmioFOURCC('R', 'I', 'F', 'F'))
        goto exit;      // not even RIFF

    if (dwType != mmioFOURCC('W', 'A', 'V', 'E'))
        goto exit;      // not a WAV

    pdwEnd = (DWORD *)((BYTE *)pdw + dwLength-4);

    while (pdw < pdwEnd)
    {
        dwType = *pdw++;
        dwLength = *pdw++;

        switch (dwType)
        {
        case mmioFOURCC('f', 'm', 't', ' '):
            if (ppWaveHeader && !*ppWaveHeader)
            {
                if (dwLength < sizeof(WAVEFORMAT))
                    goto exit;      // not a WAV

                *ppWaveHeader = (WAVEFORMATEX *)pdw;

                if ((!ppbWaveData || *ppbWaveData) &&
                    (!pcbWaveSize || *pcbWaveSize))
                {
                    return TRUE;
                }
            }
            break;

        case mmioFOURCC('d', 'a', 't', 'a'):
            if ((ppbWaveData && !*ppbWaveData) ||
                (pcbWaveSize && !*pcbWaveSize))
            {
                if (ppbWaveData)
                    *ppbWaveData = (LPBYTE)pdw;

                if (pcbWaveSize)
                    *pcbWaveSize = dwLength;

                if (!ppWaveHeader || *ppWaveHeader)
                    return TRUE;
            }
            break;
        }

        pdw = (DWORD *)((BYTE *)pdw + ((dwLength+1)&~1));
    }

exit:
    return FALSE;
}
