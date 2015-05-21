// just used by setup.rul InstallShield stuff -- taken from dsetup.h in DirectX 5 SDK

// DSETUP flags. DirectX 5.0 apps should use these flags only.
#define DSETUP_DDRAWDRV         0x00000008      /* install DirectDraw Drivers           */
#define DSETUP_DSOUNDDRV        0x00000010      /* install DirectSound Drivers          */
#define DSETUP_DXCORE           0x00010000	/* install DirectX runtime              */
#define DSETUP_DIRECTX  (DSETUP_DXCORE|DSETUP_DDRAWDRV|DSETUP_DSOUNDDRV)
#define DSETUP_TESTINSTALL      0x00020000      /* just test install, don't do anything */

// DSETUP Error Codes, must remain compatible with previous setup.
#define DSETUPERR_SUCCESS_RESTART        1
#define DSETUPERR_SUCCESS                0
#define DSETUPERR_BADWINDOWSVERSION     -1
#define DSETUPERR_SOURCEFILENOTFOUND    -2
#define DSETUPERR_BADSOURCESIZE         -3
#define DSETUPERR_BADSOURCETIME         -4
#define DSETUPERR_NOCOPY                -5
#define DSETUPERR_OUTOFDISKSPACE        -6
#define DSETUPERR_CANTFINDINF           -7
#define DSETUPERR_CANTFINDDIR           -8
#define DSETUPERR_INTERNAL              -9
#define DSETUPERR_NTWITHNO3D            -10  /* REM: obsolete, you'll never see this */
#define DSETUPERR_UNKNOWNOS             -11
#define DSETUPERR_USERHITCANCEL         -12
#define DSETUPERR_NOTPREINSTALLEDONNT   -13

