/*
 * tc2_leia.cpp -- THE LEIA SR WEAVER for Time Crisis 2 (Display > 3D > Leia SR display: the Samsung Odyssey 3D and the other displays of
 * Leia's SR platform). Leia's SDK is C++ built with MSVC, the game is C built with MinGW: this DLL, built with MSVC (build.bat), puts a
 * plain C face on the few calls the game needs. The game loads it at run time (src/tc2_window.c), so a build without it, or a computer
 * without the SR platform, runs as before.
 *
 *   leia_open(hwnd)          connect to the SR Service and make an OpenGL weaver for the game's window (the GL context current)
 *   leia_display(rect, view) where the SR display is on the desktop, and the view size Leia recommends
 *   leia_set_view(tex, w, h) the side-by-side picture to weave: one GL texture 2w x h, the left eye in the left half
 *   leia_weave()             weave it into the bound framebuffer (the window), for the eyes the SR eye tracker sees now
 *   leia_close()
 *
 * No C++ exception leaves this file: each call catches and says what went wrong in leia_error().
 */
#include <windows.h>
#include <stdio.h>
#include <exception>
#include "sr/management/srcontext.h"
#include "sr/utility/exception.h"
#include "sr/weaver/glweaver.h"
#include "sr/world/display/display.h"

#define API extern "C" __declspec(dllexport)

static SR::SRContext *ctx;
static SR::IGLWeaver1 *weaver;
static char err[256];

API const char *leia_error(void) { return err; }

API void leia_close(void)
{
    try {
        if (weaver) weaver->destroy();
        if (ctx) SR::SRContext::deleteSRContext(ctx);
    } catch (...) {}
    weaver = nullptr; ctx = nullptr;
}

/* wait_s: how long to wait for the SR Service (it may still be starting) */
API int leia_open(HWND hwnd, double wait_s)
{
    err[0] = 0;
    try {
        const ULONGLONG t0 = GetTickCount64();
        while (!ctx) {
            try { ctx = SR::SRContext::create(); } catch (SR::ServerNotAvailableException &) {}
            if (ctx) break;
            if ((GetTickCount64() - t0) / 1000.0 > wait_s) { snprintf(err, sizeof err, "the SR Service does not answer (is the Leia SR platform running?)"); return 0; }
            Sleep(100);
        }
        if (SR::CreateGLWeaver(*ctx, hwnd, &weaver) != WeaverSuccess || !weaver) { snprintf(err, sizeof err, "no OpenGL weaver"); leia_close(); return 0; }
        ctx->initialize();                                   /* after the weaver (Leia's example: "Initialize context after creating weaver") */
        return 1;
    } catch (std::exception &e) { snprintf(err, sizeof err, "%s", e.what()); }
    catch (...) { snprintf(err, sizeof err, "an unknown error in the SR platform"); }
    leia_close();
    return 0;
}

/* rect: left, top, width, height of the SR display on the desktop; view: the recommended size of ONE eye's picture. 0 = no SR display */
API int leia_display(int rect[4], int view[2])
{
    try {
        LoadLibraryA("SimulatedRealityDisplays.dll");      /* TryGetDisplayManagerInstance finds the display API in it by name */
        SR::IDisplayManager *dm = ctx ? SR::TryGetDisplayManagerInstance(*ctx) : nullptr;
        SR::IDisplay *d = dm ? dm->getPrimaryActiveSRDisplay() : nullptr;
        if (!d || !d->isValid()) return 0;
        const SR_recti r = d->getLocation();
        rect[0] = (int)r.left; rect[1] = (int)r.top; rect[2] = (int)(r.right - r.left); rect[3] = (int)(r.bottom - r.top);
        view[0] = d->getRecommendedViewsTextureWidth(); view[1] = d->getRecommendedViewsTextureHeight();
        return rect[2] > 0 && rect[3] > 0;
    } catch (...) { return 0; }
}

API void leia_set_view(unsigned tex, int w, int h)
{
    try { if (weaver) weaver->setInputViewTexture((GLuint)tex, w, h, 0x8058 /* GL_RGBA8 */); } catch (...) {}
}

API void leia_weave(void)
{
    try { if (weaver) weaver->weave(); } catch (...) {}
}
