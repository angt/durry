#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "xf86.h"
#include "xf86_OSproc.h"

#include "xf86Cursor.h"
#include "xf86cmap.h"
#include "xf86fbman.h"
#include "xf86Crtc.h"

#include "mipointer.h"
#include "micmap.h"
#include "fb.h"

#include "compat-api.h"

#define DURRY_NAME "DURRY"
#define DURRY_DRIVER_NAME "durry"
#define DURRY_DRIVER_VERSION 4000

#define DURRY_MAJOR_VERSION PACKAGE_VERSION_MAJOR
#define DURRY_MINOR_VERSION PACKAGE_VERSION_MINOR
#define DURRY_PATCHLEVEL PACKAGE_VERSION_PATCHLEVEL

#define DURRY_WIDTH  4096
#define DURRY_HEIGHT 4096

#ifndef HW_SKIP_CONSOLE
#define HW_SKIP_CONSOLE 4
#endif

static SymTabRec chipsets[] = {
    {  0, DURRY_DRIVER_NAME },
    { -1, NULL },
};

static const OptionInfoRec options[] = {
    {  0, "SWcursor", OPTV_BOOLEAN, {0}, FALSE },
    { -1,  NULL,      OPTV_NONE,    {0}, FALSE },
};

typedef struct durryRec
{
    OptionInfoPtr Options;
    Bool swCursor;

    CloseScreenProcPtr CloseScreen;
    xf86CursorInfoPtr CursorInfo;

    Bool durryHWCursorShown;
    int cursorX, cursorY;
    int cursorFG, cursorBG;

    Bool screenSaver;
    LOCO colors[256];
    pointer *FBBase;
} DURRYRec, *DURRYPtr;

#define DURRYPTR(p) ((DURRYPtr)((p)->driverPrivate))

static void
durryShowCursor (ScrnInfoPtr pScrn)
{
    DURRYPtr dPtr = DURRYPTR(pScrn);
    dPtr->durryHWCursorShown = TRUE;
}

static void
durryHideCursor (ScrnInfoPtr pScrn)
{
    DURRYPtr dPtr = DURRYPTR(pScrn);
    dPtr->durryHWCursorShown = FALSE;
}

static void
durrySetCursorPosition (ScrnInfoPtr pScrn, int x, int y)
{
    DURRYPtr dPtr = DURRYPTR(pScrn);
    dPtr->cursorX = x;
    dPtr->cursorY = y;
}

static void
durrySetCursorColors (ScrnInfoPtr pScrn, int bg, int fg)
{
    DURRYPtr dPtr = DURRYPTR(pScrn);
    dPtr->cursorFG = fg;
    dPtr->cursorBG = bg;
}

static void
durryLoadCursorImage (ScrnInfoPtr pScrn, unsigned char *src)
{
}

static Bool
durryUseHWCursor (ScreenPtr pScreen, CursorPtr pCurs)
{
    DURRYPtr dPtr = DURRYPTR(xf86ScreenToScrn(pScreen));
    return !dPtr->swCursor;
}

static Bool
DURRYCursorInit (ScreenPtr pScreen)
{
    DURRYPtr dPtr = DURRYPTR(xf86ScreenToScrn(pScreen));

    xf86CursorInfoPtr infoPtr = xf86CreateCursorInfoRec();

    if (!infoPtr)
        return FALSE;

    dPtr->CursorInfo = infoPtr;

    infoPtr->MaxHeight = 64;
    infoPtr->MaxWidth = 64;
    infoPtr->Flags = HARDWARE_CURSOR_TRUECOLOR_AT_8BPP;

    infoPtr->SetCursorColors = durrySetCursorColors;
    infoPtr->SetCursorPosition = durrySetCursorPosition;
    infoPtr->LoadCursorImage = durryLoadCursorImage;
    infoPtr->HideCursor = durryHideCursor;
    infoPtr->ShowCursor = durryShowCursor;
    infoPtr->UseHWCursor = durryUseHWCursor;

    return xf86InitCursor(pScreen, infoPtr);
}

static Bool
DURRYGetRec (ScrnInfoPtr pScrn)
{
    if (pScrn->driverPrivate)
        return TRUE;

    pScrn->driverPrivate = xnfcalloc(sizeof(DURRYRec), 1);

    if (!pScrn->driverPrivate)
        return FALSE;

    return TRUE;
}

static void
DURRYFreeRec (ScrnInfoPtr pScrn)
{
    if (pScrn->driverPrivate) {
        free(pScrn->driverPrivate);
        pScrn->driverPrivate = NULL;
        return;
    }

}

static Bool
config_resize (ScrnInfoPtr pScrn, int width, int height)
{
    ScreenPtr pScreen = pScrn->pScreen;

    if (pScrn->virtualX>DURRY_WIDTH)
        pScrn->virtualX = DURRY_WIDTH;

    if (pScrn->virtualY>DURRY_HEIGHT)
        pScrn->virtualY = DURRY_HEIGHT;

    if (pScrn->virtualX==width && pScrn->virtualY==height)
        return TRUE;

    pScrn->virtualX = width;
    pScrn->virtualY = height;
    pScrn->displayWidth = DURRY_WIDTH;

    PixmapPtr pixmap = pScreen->GetScreenPixmap(pScreen);
    pScreen->ModifyPixmapHeader(pixmap, width, height, pScrn->depth, BitsPerPixel(pScrn->depth), 0, NULL);

    return TRUE;
}

static const xf86CrtcConfigFuncsRec config_funcs = {
    config_resize
};

static void
crtc_dpms (xf86CrtcPtr crtc, int mode)
{
}

static Bool
crtc_set_mode_major (xf86CrtcPtr crtc, DisplayModePtr mode, Rotation rotation, int x, int y)
{
    return TRUE;
}

static void
crtc_gamma_set (xf86CrtcPtr crtc, CARD16 *red, CARD16 *green, CARD16 *blue, int size)
{
}

static void
crtc_destroy (xf86CrtcPtr crtc)
{
}

static const xf86CrtcFuncsRec crtc_funcs = {
    .dpms = crtc_dpms,
    .set_mode_major = crtc_set_mode_major,
    .gamma_set = crtc_gamma_set,
    .destroy = crtc_destroy,
};

static void
output_create_resources (xf86OutputPtr output)
{
}

static void
output_dpms (xf86OutputPtr output, int dpms)
{
}

static void
output_destroy (xf86OutputPtr output)
{
}

static DisplayModePtr
output_get_modes (xf86OutputPtr output)
{
    return NULL;
}

static Bool
output_mode_valid (xf86OutputPtr output, DisplayModePtr pModes)
{
    return MODE_OK;
}

static Bool
output_get_property (xf86OutputPtr output, Atom property)
{
    return FALSE;
}

static Bool
output_set_property (xf86OutputPtr output, Atom property, RRPropertyValuePtr value)
{
    return TRUE;
}

static xf86OutputStatus
output_detect (xf86OutputPtr output)
{
    return XF86OutputStatusDisconnected;
}

static const xf86OutputFuncsRec output_funcs = {
    .create_resources = output_create_resources,
#ifdef RANDR_12_INTERFACE
    .set_property = output_set_property,
    .get_property = output_get_property,
#endif
    .dpms = output_dpms,
    .detect = output_detect,
    .mode_valid = output_mode_valid,
    .get_modes = output_get_modes,
    .destroy = output_destroy,
};

static Bool
DURRYPreInit (ScrnInfoPtr pScrn, int flags)
{
    GDevPtr device = xf86GetEntityInfo(pScrn->entityList[0])->device;

    if (flags & PROBE_DETECT)
        return TRUE;

    if (!DURRYGetRec(pScrn))
        return FALSE;

    DURRYPtr dPtr = DURRYPTR(pScrn);
    pScrn->chipset = (char *)xf86TokenToString(chipsets, 0);
    pScrn->monitor = pScrn->confScreen->monitor;

    if (!xf86SetDepthBpp(pScrn, 0, 0, 0, Support32bppFb|PreferConvert24to32|SupportConvert24to32))
        goto fail;

    pScrn->rgbBits = 8;
    pScrn->displayWidth = DURRY_WIDTH;

    if (pScrn->depth!=24) {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                "Given depth (%d) is not supported by this driver\n",
                pScrn->depth);
        goto fail;
    }

    xf86PrintDepthBpp(pScrn);

    rgb zeros = {0, 0, 0};

    if (!xf86SetWeight(pScrn, zeros, zeros))
        goto fail;

    if (!xf86SetDefaultVisual(pScrn, -1))
        goto fail;

    Gamma gamma = {0.0, 0.0, 0.0};

    if (!xf86SetGamma(pScrn, gamma))
        goto fail;

    xf86CollectOptions(pScrn, device->options);

    dPtr->Options = malloc(sizeof(options));

    if (!dPtr->Options)
        goto fail;

    memcpy(dPtr->Options, options, sizeof(options));

    xf86ProcessOptions(pScrn->scrnIndex, pScrn->options, dPtr->Options);
    xf86GetOptValBool(dPtr->Options, 0, &dPtr->swCursor);

    xf86CrtcConfigInit(pScrn, &config_funcs);
    xf86CrtcSetSizeRange(pScrn, 8, 8, DURRY_WIDTH, DURRY_HEIGHT);

    xf86CrtcPtr crtc = xf86CrtcCreate(pScrn, &crtc_funcs);
    xf86OutputPtr output = xf86OutputCreate(pScrn, &output_funcs, DURRY_NAME);

    if (!output) {  // XXX need free
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Couldn't create output\n");
        goto fail;
    }

    output->mm_width = 1000;
    output->mm_height = 1000;
    output->interlaceAllowed = FALSE;
    output->subpixel_order = SubPixelNone;
    output->possible_crtcs = ~((1<<0)-1);
    output->possible_clones = ~((1<<0)-1);

    xf86InitialConfiguration(pScrn, TRUE);

    if (!pScrn->modes) {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "No valid modes found\n");
        goto fail;
    }

    pScrn->currentMode = pScrn->modes;

    xf86SetDpi(pScrn, 0, 0);

    if (!xf86LoadSubModule(pScrn, "fb"))
        goto fail;

    if (!dPtr->swCursor) {
        if (!xf86LoadSubModule(pScrn, "ramdac"))
            goto fail;
    }

    pScrn->memPhysBase = 0;
    pScrn->fbOffset = 0;

    return TRUE;

fail:
    if (dPtr) {
        if (dPtr->Options) {
            free(dPtr->Options);
            dPtr->Options = NULL;
        }
    }
    DURRYFreeRec(pScrn);
    return FALSE;
}

static void
DURRYLoadPalette (ScrnInfoPtr pScrn, int numColors, int *indices, LOCO *colors, VisualPtr pVisual)
{
    DURRYPtr dPtr = DURRYPTR(pScrn);

    for(int i=0; i<numColors; i++) {
        int index = indices[i];
        dPtr->colors[index].red = colors[index].red;
        dPtr->colors[index].green = colors[index].green;
        dPtr->colors[index].blue = colors[index].blue;
    }
}

static Bool
DURRYSaveScreen (ScreenPtr pScreen, int mode)
{
    ScrnInfoPtr pScrn = NULL;
    DURRYPtr dPtr;

    if (pScreen) {
        pScrn = xf86ScreenToScrn(pScreen);
        dPtr = DURRYPTR(pScrn);
        dPtr->screenSaver = xf86IsUnblank(mode);
    }

    return TRUE;
}

static Bool
DURRYCloseScreen (CLOSE_SCREEN_ARGS_DECL)
{
    ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
    DURRYPtr dPtr = DURRYPTR(pScrn);

    if (dPtr->FBBase) { // XXX check vtSema ?
        free(dPtr->FBBase);
        dPtr->FBBase = NULL;
    }

    if (dPtr->CursorInfo)
        xf86DestroyCursorInfoRec(dPtr->CursorInfo);

    pScrn->vtSema = FALSE;
    pScreen->CloseScreen = dPtr->CloseScreen;

    return (*pScreen->CloseScreen)(CLOSE_SCREEN_ARGS);
}

static Bool
DURRYScreenInit (SCREEN_INIT_ARGS_DECL)
{
    ScrnInfoPtr pScrn;
    DURRYPtr dPtr;
    VisualPtr visual;

    pScrn = xf86ScreenToScrn(pScreen);
    dPtr = DURRYPTR(pScrn);

    size_t size = DURRY_WIDTH*DURRY_HEIGHT*4;

    pScrn->videoRam = size/1024;
    dPtr->FBBase = malloc(size);

    if (!dPtr->FBBase)
        return FALSE;

    miClearVisualTypes();

    if (!miSetVisualTypes(pScrn->depth,
                miGetDefaultVisualMask(pScrn->depth),
                pScrn->rgbBits, pScrn->defaultVisual))
        return FALSE;

    if (!miSetPixmapDepths ())
        return FALSE;

    if (!fbScreenInit(pScreen, dPtr->FBBase,
                pScrn->virtualX, pScrn->virtualY,
                pScrn->xDpi, pScrn->yDpi,
                pScrn->displayWidth, pScrn->bitsPerPixel))
        return FALSE;

    if (pScrn->depth > 8) {
        visual = pScreen->visuals + pScreen->numVisuals;
        while (--visual >= pScreen->visuals) {
            if ((visual->class|DynamicClass)==DirectColor) {
                visual->offsetRed = pScrn->offset.red;
                visual->offsetGreen = pScrn->offset.green;
                visual->offsetBlue = pScrn->offset.blue;
                visual->redMask = pScrn->mask.red;
                visual->greenMask = pScrn->mask.green;
                visual->blueMask = pScrn->mask.blue;
            }
        }
    }

    fbPictureInit(pScreen, 0, 0);

    xf86SetBlackWhitePixels(pScreen);

    if (dPtr->swCursor)
        xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "Using Software Cursor\n");

    BoxRec box;
    box.x1 = 0;
    box.y1 = 0;
    box.x2 = DURRY_WIDTH;
    box.y2 = DURRY_HEIGHT;
    xf86InitFBManager(pScreen, &box);

    xf86SetBackingStore(pScreen);
    xf86SetSilkenMouse(pScreen);

    miDCInitialize(pScreen, xf86GetPointerScreenFuncs());

    if (!dPtr->swCursor) {
        if (!DURRYCursorInit(pScreen))
            return FALSE;
    }

 // pScrn->vtSema = TRUE;

    pScreen->SaveScreen = DURRYSaveScreen;

    dPtr->CloseScreen = pScreen->CloseScreen;
    pScreen->CloseScreen = DURRYCloseScreen;

    if (!xf86CrtcScreenInit(pScreen))
        return FALSE;

 // xf86RandR12SetRotations(pScreen, RR_Rotate_All|RR_Reflect_All);
 // xf86RandR12SetTransformSupport(pScreen, TRUE);

    if(!miCreateDefColormap(pScreen))
        return FALSE;

    if (!xf86HandleColormaps(pScreen, 256, pScrn->rgbBits, DURRYLoadPalette,
                NULL, CMAP_PALETTED_TRUECOLOR|CMAP_RELOAD_ON_MODE_SWITCH))
        return FALSE;

 // and DPMS ?

    if (serverGeneration==1)
        xf86ShowUnusedOptions(pScrn->scrnIndex, pScrn->options);

    return TRUE;
}

static Bool
DURRYSwitchMode (SWITCH_MODE_ARGS_DECL)
{
    return TRUE;
}

static void
DURRYAdjustFrame (ADJUST_FRAME_ARGS_DECL)
{
}

static Bool
DURRYEnterVT (VT_FUNC_ARGS_DECL)
{
    return TRUE;
}

static void
DURRYLeaveVT (VT_FUNC_ARGS_DECL)
{
}

static void
DURRYFreeScreen (FREE_SCREEN_ARGS_DECL)
{
    SCRN_INFO_PTR(arg);
    DURRYFreeRec(pScrn);
}

static ModeStatus
DURRYValidMode (SCRN_ARG_TYPE arg, DisplayModePtr mode, Bool verbose, int flags)
{
    return MODE_OK;
}

static Bool
DURRYProbe (DriverPtr drv, int flags)
{
    Bool found = FALSE;
    GDevPtr *devSections;

    if (flags & PROBE_DETECT)
        return FALSE;

    int n = xf86MatchDevice(DURRY_DRIVER_NAME, &devSections);

    if (n<=0)
        return FALSE;

    for (int i = 0; i<n; i++) {
        int ei = xf86ClaimNoSlot(drv, 0, devSections[i], TRUE);
        ScrnInfoPtr pScrn = xf86AllocateScreen(drv, 0);

        if (!pScrn)
            continue;

        xf86AddEntityToScreen(pScrn, ei);
        pScrn->driverVersion = DURRY_DRIVER_VERSION;
        pScrn->driverName    = DURRY_DRIVER_NAME;
        pScrn->name          = DURRY_NAME;
        pScrn->Probe         = DURRYProbe;
        pScrn->PreInit       = DURRYPreInit;
        pScrn->ScreenInit    = DURRYScreenInit;
        pScrn->SwitchMode    = DURRYSwitchMode;
        pScrn->AdjustFrame   = DURRYAdjustFrame;
        pScrn->EnterVT       = DURRYEnterVT;
        pScrn->LeaveVT       = DURRYLeaveVT;
        pScrn->FreeScreen    = DURRYFreeScreen;
        pScrn->ValidMode     = DURRYValidMode;

        found = TRUE;
    }

    return found;
}

static const OptionInfoRec *
DURRYAvailableOptions (int chipid, int busid)
{
    return options;
}

static void
DURRYIdentify (int flags)
{
    xf86PrintChipsets(DURRY_NAME, "Driver", chipsets);
}

static Bool
durryDriverFunc (ScrnInfoPtr pScrn, xorgDriverFuncOp op, pointer ptr)
{
    CARD32 *flag;

    switch (op) {
        case GET_REQUIRED_HW_INTERFACES:
            flag = (CARD32*)ptr;
            (*flag) = HW_SKIP_CONSOLE;
            return TRUE;
        default:
            return FALSE;
    }
}

_X_EXPORT DriverRec DURRY = {
    DURRY_DRIVER_VERSION,
    DURRY_DRIVER_NAME,
    DURRYIdentify,
    DURRYProbe,
    DURRYAvailableOptions,
    NULL,
    0,
    durryDriverFunc
};

#ifdef XFree86LOADER

static pointer
durrySetup (pointer module, pointer opts, int *errmaj, int *errmin)
{
    static Bool done = FALSE;

    if (!done) {
        done = TRUE;
        xf86AddDriver(&DURRY, module, HaveDriverFuncs);
        return (pointer)1;
    }

    if (errmaj)
        *errmaj = LDR_ONCEONLY;

    return NULL;
}

static MODULESETUPPROTO(durrySetup);

static XF86ModuleVersionInfo durryVersRec =
{
    DURRY_DRIVER_NAME,
    MODULEVENDORSTRING,
    MODINFOSTRING1,
    MODINFOSTRING2,
    XORG_VERSION_CURRENT,
    DURRY_MAJOR_VERSION, DURRY_MINOR_VERSION, DURRY_PATCHLEVEL,
    ABI_CLASS_VIDEODRV,
    ABI_VIDEODRV_VERSION,
    MOD_CLASS_VIDEODRV,
    {0,0,0,0}
};

_X_EXPORT XF86ModuleData durryModuleData = { &durryVersRec, durrySetup, NULL };

#endif
